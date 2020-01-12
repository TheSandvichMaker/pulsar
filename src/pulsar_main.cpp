#include "pulsar_main.h"
#include "pulsar_generated_post_headers.h"

#define STB_SPRINTF_STATIC 1
#define STB_SPRINTF_IMPLEMENTATION 1
#include "external/stb_sprintf.h"

inline void dbg_text(char* format_string, ...) {
    va_list va_args;
    va_start(va_args, format_string);

    char buffer[8192];
    stbsp_vsnprintf(buffer, sizeof(buffer), format_string, va_args);

    va_end(va_args);

    platform.debug_print(buffer);
}

inline Level* allocate_level(MemoryArena* arena, char* name) {
    Level* result = push_struct(arena, Level);

    // @Note: The 0th entity is reserved as the null entity
    result->first_available_guid = 1;
    result->entity_count = 1;

    size_t name_length = cstr_length(name);
    result->name.len = name_length;
    result->name.data = name;

    return result;
}

#pragma pack(push, 1)
struct LevFileHeader {
    struct {
        char magic[4];

#define LEV_VERSION 0
        u32 version;

        u32 header_size;
    } prelude;

    u32 entity_count;
    u32 entity_member_count;
};
#pragma pack(pop)

internal void write_level_to_disk(GameState* game_state, Level* level, char* level_name) {
    TemporaryMemory temp = begin_temporary_memory(&game_state->transient_arena);

    u8* stream = begin_linear_buffer(&game_state->transient_arena, u8, align_no_clear(1));

    // @TODO: I should have some kind of formalized way to write byte streams to memory arenas
#define write_stream(size, data) { u8* buf = lb_push_n(stream, size); copy(size, data, buf); }

    LevFileHeader header;

    header.prelude.magic[0] = 'l';
    header.prelude.magic[1] = 'e';
    header.prelude.magic[2] = 'v';
    header.prelude.magic[3] = 'l';

    header.prelude.version = LEV_VERSION;
    header.prelude.header_size = sizeof(header);

    header.entity_count = level->entity_count;
    header.entity_member_count = members_count(Entity);

    write_stream(sizeof(header), &header);

    for (u32 entity_index = 0; entity_index < level->entity_count; entity_index++) {
        Entity* entity = level->entities + entity_index;
        for (u32 member_index = 0; member_index < members_count(Entity); member_index++) {
            MemberDefinition member = members_of(Entity)[member_index];
            void** member_ptr = member_ptr(*entity, member);

            write_stream(sizeof(u32), &member.name_length);
            write_stream(member.name_length, member.name);
            write_stream(sizeof(u32), &member.size);
            write_stream(member.size, member_ptr);
        }
    }

#undef write_stream

    platform.write_entire_file(level_name, cast(u32) lb_count(stream), stream);

    end_linear_buffer(stream);

    end_temporary_memory(temp);
}

inline MemberDefinition* find_member_by_name(u32 name_length, char* name) {
    MemberDefinition* result = 0;

    for (u32 member_index = 0; member_index < members_count(Entity); member_index++) {
        MemberDefinition* member = members_of(Entity) + member_index;
        if (strings_are_equal(wrap_string(name_length, name), wrap_string(member->name_length, member->name))) {
            result = member;
            break;
        }
    }

    return result;
}

internal b32 load_level_from_disk(GameState* game_state, Level* level, char* level_name) {
    assert(level);

    b32 level_load_error = false;

    TemporaryMemory temp = begin_temporary_memory(&game_state->transient_arena);

    EntireFile file = platform.read_entire_file(level_name, allocator(arena_allocator, &game_state->transient_arena));
    if (file.size > 0) {
        u8* stream = cast(u8*) file.data;
        u8* stream_end = stream + file.size;

        while (stream < stream_end) {
#define read_stream(size) (assert(stream + (size) <= stream_end), stream += (size), stream - (size))
            LevFileHeader* header = cast(LevFileHeader*) stream;
            assert(header->prelude.header_size <= sizeof(*header));

            stream += header->prelude.header_size;

            if (header->prelude.magic[0] == 'l' &&
                header->prelude.magic[1] == 'e' &&
                header->prelude.magic[2] == 'v' &&
                header->prelude.magic[3] == 'l'
            ) {
                if (header->entity_member_count != members_count(Entity)) {
                    dbg_text("Level Load Warning: Member count mismatch: %u in file, expected %u\n", header->entity_member_count, members_count(Entity));
                }

                level->name = wrap_cstr(level_name);
                level->entity_count = header->entity_count;
                level->first_available_guid = 1;

                for (u32 entity_index = 0; entity_index < level->entity_count; entity_index++) {
                    Entity* entity = level->entities + entity_index;

                    for (u32 member_index = 0; member_index < header->entity_member_count; member_index++) {
                        u32 member_name_length = *(cast(u32*) read_stream(sizeof(u32)));
                        assert(member_name_length);
                        char* member_name = cast(char*) read_stream(member_name_length);

                        MemberDefinition* member = find_member_by_name(member_name_length, member_name);
                        if (member) {
                            void** member_ptr = member_ptr(*entity, *member);
                            u32 member_size = *(cast(u32*) read_stream(sizeof(u32)));

                            if (member_size && member->size == member_size) {
                                copy(member_size, read_stream(member_size), member_ptr);

                                if (member->type == member_type(EntityID) && strings_are_equal(member->name, "guid")) {
                                    if (entity->guid.value > level->first_available_guid) {
                                        level->first_available_guid = entity->guid.value + 1;
                                    }
                                }
                            } else {
                                level_load_error = true;
                                dbg_text("Level Load Error: Member size mismatch: %u in file, expected %u\n", member_size, member->size);
                                goto level_load_end;
                            }
                        } else {
                            dbg_text("Level Load Warning: Could not find matching member '%.*s'\n", member_name_length, member_name);
                        }
                    }
                }
            } else {
                level_load_error = true;
                dbg_text("Level Load Error: Header did not start with magic value 'levl'\n");
                goto level_load_end;
            }
#undef read_stream
        }
    } else {
        level_load_error = true;
        dbg_text("Level Load Error: Could not open file '%s'\n", level_name);
        goto level_load_end;
    }

level_load_end:
    if (level_load_error) {
        level->entity_count = 0;
        level->first_available_guid = 1;
    }

    end_temporary_memory(temp);

    return !level_load_error;
}

#include "pulsar_assets.cpp"
#include "pulsar_audio_mixer.cpp"
#include "pulsar_render_commands.cpp"
#include "pulsar_gjk.cpp"
#include "pulsar_entity.cpp"
#include "pulsar_editor.cpp"

global v2 arrow_verts[] = { { 0, -0.05f }, { 0.8f, -0.05f }, { 0.8f, -0.2f }, { 1.0f, 0.0f }, { 0.8f, 0.2f }, { 0.8f, 0.05f }, { 0, 0.05f } };
global Shape2D arrow = polygon(ARRAY_COUNT(arrow_verts), arrow_verts);

inline void dbg_draw_arrow(v2 start, v2 end, v4 color = vec4(1, 1, 1, 1)) {
    f32 scale = length(end - start);
    v2 dir = normalize_or_zero(end - start);
    Transform2D t = transform2d(start);
    t.scale = vec2(scale, scale);
    t.rotation_arm = dir;

    push_shape(&dbg_game_state->render_context, t, arrow, color, ShapeRenderMode_Outline);
}

#define MAX_COLLISION_ITERATIONS 4
inline PlayingMidi* play_midi(GameState* game_state, MidiTrack* track, u32 flags = 0, PlayingSound* sync_sound = 0) {
    if (!game_state->first_free_playing_midi) {
        game_state->first_free_playing_midi = push_struct(&game_state->permanent_arena, PlayingMidi);
        game_state->first_free_playing_midi->next_free = 0;
    }

    PlayingMidi* playing_midi = game_state->first_free_playing_midi;
    game_state->first_free_playing_midi = playing_midi->next_free;

    playing_midi->next = game_state->first_playing_midi;
    game_state->first_playing_midi = playing_midi;

    zero_struct(*playing_midi);
    playing_midi->track = track;
    playing_midi->flags = flags;
    playing_midi->sync_sound = sync_sound;
    playing_midi->sync_sound->synced_midi = playing_midi;

    return playing_midi;
}

inline void play_soundtrack(GameState* game_state, Soundtrack* soundtrack, u32 flags) {
    Sound* sound = get_sound(&game_state->assets, soundtrack->sound);
    PlayingSound* playing_sound = play_sound(&game_state->audio_mixer, sound, flags);
    for (u32 midi_index = 0; midi_index < soundtrack->midi_track_count; midi_index++) {
        MidiTrack* track = get_midi(&game_state->assets, soundtrack->midi_tracks[midi_index]);
        play_midi(game_state, track, flags, playing_sound);
    }
}

inline void stop_all_midi_tracks(GameState* game_state) {
    PlayingMidi* last_playing_midi = game_state->first_playing_midi;
    if (last_playing_midi) {
        while (last_playing_midi->next) {
            last_playing_midi = last_playing_midi->next;
        }
        last_playing_midi->next = game_state->first_free_playing_midi;
        game_state->first_free_playing_midi = game_state->first_playing_midi;
        game_state->first_playing_midi = 0;
    }

    game_state->midi_event_buffer_count = 0;
}

inline void switch_gamemode(GameState* game_state, GameMode game_mode) {
    switch (game_mode) {
        case GameMode_Ingame: {
            if (game_state->game_mode == GameMode_Editor) {
                EditorState* editor = game_state->editor_state;
                editor->camera_p_on_exit = game_state->render_context.camera_p;
            }

            game_state->last_activated_checkpoint = 0;

            Level* level = game_state->active_level;
            for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
                Entity* source = level->entities + entity_index;
                Entity* dest = game_state->entities + entity_index;

                *dest = *source;

                // dest->guid = { entity_index };

                if (dest->type == EntityType_Player) {
                    if (!game_state->player) {
                        game_state->player = dest;
                    } else {
                        // You can only have one player!
                        INVALID_CODE_PATH;
                    }
                }
            }
            game_state->entity_count = level->entity_count;
        } break;

        case GameMode_Editor: {
            if (game_state->game_mode == GameMode_Ingame) {
                EditorState* editor = game_state->editor_state;
                game_state->render_context.camera_p = editor->camera_p_on_exit;
            }

            stop_all_sounds(&game_state->audio_mixer);
            stop_all_midi_tracks(game_state);
            game_state->entity_count = 0;
            game_state->player = 0;
        } break;
    }

    game_state->game_mode = game_mode;
}

internal GAME_UPDATE_AND_RENDER(game_update_and_render) {
    assert(memory->permanent_storage_size >= sizeof(GameState));

    platform = memory->platform_api;

    //
    // Initialization
    //

    GameState* game_state = cast(GameState*) memory->permanent_storage;

#if PULSAR_DEBUG
    dbg_game_state = game_state;
#endif

    u32 width = render_commands->width;
    u32 height = render_commands->height;

    if (!memory->initialized) {
        initialize_arena(&game_state->permanent_arena, memory->permanent_storage_size - sizeof(GameState), cast(u8*) memory->permanent_storage + sizeof(GameState));
        initialize_arena(&game_state->transient_arena, memory->transient_storage_size, memory->transient_storage);

        initialize_audio_mixer(&game_state->audio_mixer, &game_state->permanent_arena);

        // @TODO: Make the load_assets routine ignorant of the platform's file system
        load_assets(&game_state->assets, &game_state->transient_arena, "assets.pla");

        game_state->active_level = allocate_level(&game_state->permanent_arena, "levels/debug_level.lev");

        game_state->foreground_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        game_state->background_color = vec4(0.3f, 0.2f, 0.4f, 1.0f);

        game_state->editor_state = allocate_editor(game_state, render_commands, game_state->active_level);

        switch_gamemode(game_state, GameMode_Editor);

        initialize_render_context(&game_state->render_context, render_commands, 30.0f);

        // PlayingSound* test_tone = play_synth(&game_state->audio_mixer, synth_test_tone);
        // change_volume(test_tone, vec2(0.25f, 0.25f));

        memory->initialized = true;
    }

    //
    // Pre-Sim
    //

    v2 mouse_p = vec2(input->mouse_x, input->mouse_y);
    f32 frame_dt = input->frame_dt;
    game_state->frame_dt = frame_dt;

    RenderContext* render_context = &game_state->render_context;

    render_worldspace(render_context, 30.0f);
    game_state->render_context.camera_rotation_arm = vec2(1, 0);

    push_clear(render_context, game_state->background_color);

    if (game_state->game_mode == GameMode_Ingame) {
        Entity* camera_target = game_state->camera_target;
        if (camera_target) {
            game_state->render_context.camera_p = camera_target->p;
        }

        if (game_state->active_camera_zone) {
            render_context->camera_p = game_state->active_camera_zone->p;
            render_context->camera_rotation_arm = game_state->active_camera_zone->camera_rotation_arm;
            render_worldspace(render_context, game_state->active_camera_zone->camera_zone.y);
        }
    }

    if (game_state->player && game_state->player->dead) {
        if (game_state->player_respawn_timer > 0.0f) {
            game_state->player_respawn_timer -= frame_dt;
        } else {
            game_state->player->dead = false;
            if (game_state->last_activated_checkpoint) {
                game_state->player->p = game_state->last_activated_checkpoint->most_recent_player_position;
                game_state->player->dp = game_state->player->ddp = vec2(0, 0);
            } else {
                // @Note: I suspect I won't have this case, instead you just always start with the first checkpoint activated.
                INVALID_CODE_PATH;
            }
        }
    }

    //
    // Sim
    //

    if (game_state->game_mode == GameMode_Ingame) {
        run_simulation(game_state, input, frame_dt);

        if (was_pressed(input->debug_fkeys[1])) {
            switch_gamemode(game_state, GameMode_Editor);
        }
    } else if (game_state->game_mode == GameMode_Editor) {
        if (was_pressed(input->debug_fkeys[1])) {
            switch_gamemode(game_state, GameMode_Ingame);
        }
    }

    //
    // Post-Sim
    //


    EditorState* editor = game_state->editor_state;

    if (was_pressed(input->debug_fkeys[2])) {
        editor->shown = !editor->shown;
    }

    execute_editor(game_state, editor, input, memory->debug_info.frame_history);
    if (game_state->camera_target && game_state->active_camera_zone) {
        if (!is_in_aab(aab_center_dim(game_state->active_camera_zone->p, game_state->active_camera_zone->camera_zone), game_state->camera_target->p)) {
            game_state->active_camera_zone = 0;
        }
    }

    u32 active_entity_count = 0;
    Entity* active_entities = 0;
    if (game_state->game_mode == GameMode_Ingame) {
        active_entity_count = game_state->entity_count;
        active_entities = game_state->entities;
    } else if (game_state->game_mode == GameMode_Editor) {
        active_entity_count = game_state->active_level->entity_count;
        active_entities = game_state->active_level->entities;
    } else {
        INVALID_CODE_PATH;
    }

    for (u32 entity_index = 1; entity_index < active_entity_count; entity_index++) {
        Entity* entity = active_entities + entity_index;
        assert(entity->type != EntityType_Null);

        //
        // Rendering
        //

        b32 should_draw = !entity->dead && !(entity->flags & EntityFlag_Invisible);

        if (editor->shown) {
            should_draw = !entity->dead;
            if (game_state->game_mode == GameMode_Editor) {
                should_draw = true;
            }
        }

        if (should_draw) {
            Transform2D transform = transform2d(entity->p);
            switch (entity->type) {
                case EntityType_CameraZone: {
                    transform.rotation_arm = entity->camera_rotation_arm;
                    {
                        Image* sprite = get_image(&game_state->assets, entity->sprite);
                        if (sprite) {
                            push_image(render_context, transform, sprite, entity->color);
                        } else {
                            INVALID_CODE_PATH;
                        }
                    }
                    push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), entity->camera_zone)), entity->color, ShapeRenderMode_Outline);

                    f32 aspect_ratio = cast(f32) width / cast(f32) height;
                    f32 zone_height = entity->camera_zone.y;
                    AxisAlignedBox2 visible_zone = aab_center_dim(vec2(0, 0), vec2(aspect_ratio*zone_height, zone_height));
                    push_shape(render_context, transform, rectangle(visible_zone), entity->color*vec4(1, 1, 1, 0.5f), ShapeRenderMode_Outline);
                } break;

                case EntityType_Checkpoint: {
                    {
                        Image* sprite = get_image(&game_state->assets, entity->sprite);
                        if (sprite) {
                            push_image(render_context, transform, sprite, entity->color);
                        } else {
                            INVALID_CODE_PATH;
                        }
                    }
                    push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), entity->checkpoint_zone)), game_state->last_activated_checkpoint == entity ? COLOR_GREEN : COLOR_RED, ShapeRenderMode_Outline);
                } break;

                default: {
                    if (entity->sprite.value) {
                        Image* sprite = get_image(&game_state->assets, entity->sprite);
                        if (sprite) {
                            push_image(render_context, transform, sprite, entity->color);
                        } else {
                            INVALID_CODE_PATH;
                        }
                    } else {
                        push_shape(render_context, transform, rectangle(entity->collision), entity->color);
                    }
                } break;
            }
        }

        if (entity->sprite.value) {
            entity->color = COLOR_WHITE;
        } else {
            entity->color = (entity->flags & EntityFlag_Hazard) ? COLOR_RED : game_state->foreground_color;
        }
    }
}

internal GAME_GET_SOUND(game_get_sound) {
    assert(memory->initialized);
    GameState* game_state = cast(GameState*) memory->permanent_storage;

    output_playing_sounds(&game_state->audio_mixer, sound_buffer, &game_state->transient_arena);
}

internal GAME_POST_RENDER(game_post_render) {
    assert(memory->initialized);
    GameState* game_state = cast(GameState*) memory->permanent_storage;

    check_arena(&game_state->permanent_arena);
    check_arena(&game_state->transient_arena);
}
