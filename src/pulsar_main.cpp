#include "pulsar_main.h"
#include "pulsar_generated_post_headers.h"

#include "pulsar_assets.cpp"
#include "pulsar_audio_mixer.cpp"
#include "pulsar_render_commands.cpp"
#include "pulsar_gjk.cpp"
#include "pulsar_entity.cpp"
#include "pulsar_editor.cpp"

// @TODO: Decouple file paths from level loading

inline Level* allocate_level(MemoryArena* arena, char* name) {
    Level* result = push_struct(arena, Level);

    // @Note: The 0th entity is reserved as the null entity
    result->first_available_guid = 1;
    result->entity_count = 1;

    size_t name_length = cstr_length(name);
    assert(name_length <= ARRAY_COUNT(result->name));
    result->name_length = cast(u32) name_length;
    copy(result->name_length, name, result->name);

    return result;
}

struct SerializeResult {
    union {
        u64   data_value;
        void* data_ptr;
    };
    b32 data_is_ptr;
    u32 size;
};

#define SERIALIZE_CALLBACK(name) SerializeResult name(GameState* game_state, u32 data_size, void* data_ptr, b32 deserialize)
typedef SERIALIZE_CALLBACK(SerializeCallback);

struct Serializable {
    SerializeCallback* callback;

    char* name;
    u32 name_length;
    s32 type; // @Note: Why does the compiler want EntityType to be signed? @TODO: could use C++11 enum type specifiers if I made the code generator not barf on them
    u32 offset;
    u32 size;
};

inline Serializable create_serializable(u32 name_length, char* name, s32 type, u32 offset, u32 size, SerializeCallback* callback = 0) {
    Serializable result;
    result.callback = callback;
    result.name = name;
    result.name_length = name_length;
    result.type = type;
    result.offset = offset;
    result.size = size;
    return result;
}

#define SERIALIZABLE(struct_type, identifying_type, member, ...) \
    create_serializable(sizeof(#member) - 1, #member, identifying_type, offsetof(struct_type, member), sizeof(cast(struct_type*) 0)->member, ##__VA_ARGS__)

internal SERIALIZE_CALLBACK(serialize_AssetID) {
    SerializeResult result;

    if (deserialize) {
        String asset_name = wrap_string(data_size, cast(char*) data_ptr);
        result.data_value = get_asset_id_by_name(&game_state->assets, asset_name).value;
        result.data_is_ptr = false;
        result.size = sizeof(AssetID);
    } else {
        result.size = 0;

        AssetID id = *(cast(AssetID*) data_ptr);
        if (id.value) {
            Asset* asset = get_asset(&game_state->assets, id);
            if (asset) {
                result.data_ptr = asset->name.data;
                result.data_is_ptr = true;
                result.size = cast(u32) asset->name.len;
            }
        }
    }

    return result;
}

global Serializable entity_serializables[] = {
    // @Note: All entities (I'm using EntityType_Count to indicate all entity types)
    SERIALIZABLE(Entity, EntityType_Count, guid),
    SERIALIZABLE(Entity, EntityType_Count, type),
    SERIALIZABLE(Entity, EntityType_Count, flags),
    SERIALIZABLE(Entity, EntityType_Count, dead),
    SERIALIZABLE(Entity, EntityType_Count, p),
    SERIALIZABLE(Entity, EntityType_Count, collision),
    SERIALIZABLE(Entity, EntityType_Count, sprite, serialize_AssetID),
    SERIALIZABLE(Entity, EntityType_Count, color),
    // @Note: Player
    // ...
    // @Note: Wall
    SERIALIZABLE(Entity, EntityType_Wall, behaviour),
    SERIALIZABLE(Entity, EntityType_Wall, midi_note),
    SERIALIZABLE(Entity, EntityType_Wall, end_p),
    SERIALIZABLE(Entity, EntityType_Wall, movement_speed_ms),
    // @Note: Soundtrack Player
    SERIALIZABLE(Entity, EntityType_SoundtrackPlayer, soundtrack_id, serialize_AssetID),
    SERIALIZABLE(Entity, EntityType_SoundtrackPlayer, playback_flags),
    SERIALIZABLE(Entity, EntityType_SoundtrackPlayer, audible_zone),
    SERIALIZABLE(Entity, EntityType_SoundtrackPlayer, horz_fade_region),
    SERIALIZABLE(Entity, EntityType_SoundtrackPlayer, vert_fade_region),
    // @Note: Camera Zone
    SERIALIZABLE(Entity, EntityType_CameraZone, active_region),
    SERIALIZABLE(Entity, EntityType_CameraZone, view_region_height),
    SERIALIZABLE(Entity, EntityType_CameraZone, camera_rotation_arm),
    // @Note: Checkpoint
    SERIALIZABLE(Entity, EntityType_Checkpoint, checkpoint_zone),
    SERIALIZABLE(Entity, EntityType_Checkpoint, most_recent_player_position),
};

/* Lev file layout:
 * LevFileHeader header;
 * for each entity serialized:
 *     u32 member_count; // number of members to follow
 *     for member_count:
 *         u32  member_name_length;
 *         char member_name[member_name_length];
 *         u32  member_data_size;
 *         u8   member_data[member_data_size];
 */

#pragma pack(push, 1)
struct LevFileHeader {
    struct {
        char magic[4];

#define LEV_VERSION 0
        u32 version;

        u32 header_size;
    } prelude;

    u32 entity_count;
};
#pragma pack(pop)

internal void write_level_to_disk(GameState* game_state, Level* level, String level_name) {
    MemoryArena* arena = &game_state->transient_arena;

    TemporaryMemory temp = begin_temporary_memory(arena);

    LinearBuffer<u8>* stream = begin_linear_buffer<u8>(arena);

    // @TODO: I should have some kind of nice way to write byte streams to memory arenas
#define write_stream(size, data) { u8* buf = lb_push_n(stream, size); copy(size, data, buf); }

    LevFileHeader header;

    header.prelude.magic[0] = 'l';
    header.prelude.magic[1] = 'e';
    header.prelude.magic[2] = 'v';
    header.prelude.magic[3] = 'l';

    header.prelude.version = LEV_VERSION;
    header.prelude.header_size = sizeof(header);

    header.entity_count = level->entity_count;

    write_stream(sizeof(header), &header);

    for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
        Entity* entity = level->entities + entity_index;

        u32* place_of_member_count = cast(u32*) lb_push_n(stream, sizeof(u32));
        u32 member_count = 0;

        for (u32 ser_index = 0; ser_index < ARRAY_COUNT(entity_serializables); ser_index++) {
            Serializable* ser = entity_serializables + ser_index;
            if (ser->type == EntityType_Count || ser->type == entity->type) {
                void** data_ptr = cast(void**) (cast(u8*) entity + ser->offset);

                SerializeResult ser_data;
                if (ser->callback) {
                    ser_data = ser->callback(game_state, ser->size, data_ptr, false);
                } else {
                    ser_data.data_ptr = data_ptr;
                    ser_data.data_is_ptr = true;
                    ser_data.size = ser->size;
                }

                if (ser_data.size > 0) {
                    member_count++;

                    write_stream(sizeof(u32), &ser->name_length);
                    write_stream(ser->name_length, ser->name);

                    if (ser_data.data_is_ptr) {
                        write_stream(sizeof(u32), &ser_data.size);
                        write_stream(ser_data.size, ser_data.data_ptr);
                    } else {
                        write_stream(sizeof(u32), &ser_data.size);
                        write_stream(ser_data.size, &ser_data.data_value);
                    }
                }
            }
        }

        *place_of_member_count = member_count;
    }

#undef write_stream

    end_linear_buffer(stream);

    char* temp_level_name_cstr = push_string_and_null_terminate(arena, level_name.len, level_name.data);
    platform.write_entire_file(temp_level_name_cstr, cast(u32) stream->count, stream->data);

    log_print(LogLevel_Info, "Saved level '%.*s' to disk", PRINTF_STRING(level_name));

    end_temporary_memory(temp);
}

inline Serializable* find_serializable_by_name(u32 serializable_count, Serializable* serializables, u32 name_length, char* name) {
    Serializable* result = 0;

    for (u32 ser_index = 0; ser_index < serializable_count; ser_index++) {
        Serializable* ser = serializables + ser_index;
        if (strings_are_equal(wrap_string(name_length, name), wrap_string(ser->name_length, ser->name))) {
            result = ser;
            break;
        }
    }

    return result;
}

internal b32 load_level_from_disk(GameState* game_state, Level* level, String level_name) {
    assert(level);

    MemoryArena* arena = &game_state->transient_arena;

    b32 level_load_error = false;

    TemporaryMemory temp = begin_temporary_memory(arena);

    // @TODO: Start using String consistently to avoid this kind of sillyness
    char* temp_level_name_cstr = push_string_and_null_terminate(arena, level_name.len, level_name.data);
    EntireFile file = platform.read_entire_file(temp_level_name_cstr, allocator(arena_allocator, arena));
    if (file.size > 0) {
        u8* stream = cast(u8*) file.data;
        u8* stream_end = stream + file.size;

#define read_stream(size) (assert(stream + (size) <= stream_end), stream += (size), stream - (size))
        LevFileHeader* header = cast(LevFileHeader*) stream;
        assert(header->prelude.header_size <= sizeof(*header));

        stream += header->prelude.header_size;

        if (header->prelude.magic[0] == 'l' &&
            header->prelude.magic[1] == 'e' &&
            header->prelude.magic[2] == 'v' &&
            header->prelude.magic[3] == 'l'
        ) {
            if (header->prelude.version != LEV_VERSION) {
                log_print(LogLevel_Warn, "Level Load Warning: Version mismatch: %u in file, expected %u", header->prelude.version, LEV_VERSION);
            }

            assert(level_name.len <= ARRAY_COUNT(level->name));
            level->name_length = cast(u32) level_name.len;
            copy(level_name.len, level_name.data, level->name);

            level->entity_count = header->entity_count;
            level->first_available_guid = 1;

            for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
                Entity* entity = level->entities + entity_index;

                u32 member_count = *(cast(u32*) read_stream(sizeof(u32)));

                for (u32 member_index = 0; member_index < member_count; member_index++) {
                    u32 member_name_length = *(cast(u32*) read_stream(sizeof(u32)));
                    assert(member_name_length);
                    char* member_name = cast(char*) read_stream(member_name_length);

                    Serializable* ser = find_serializable_by_name(ARRAY_COUNT(entity_serializables), entity_serializables, member_name_length, member_name);
                    if (ser) {
                        void** data_dest = cast(void**) (cast(u8*) entity + ser->offset);
                        u32 data_size = *(cast(u32*) read_stream(sizeof(u32)));

                        void* data_source = read_stream(data_size);

                        SerializeResult ser_data;
                        if (ser->callback) {
                            ser_data = ser->callback(game_state, data_size, data_source, true);
                        } else {
                            ser_data.data_ptr = data_source;
                            ser_data.data_is_ptr = true;
                            ser_data.size = data_size;
                        }

                        if (ser_data.size && ser->size == ser_data.size) {
                            if (ser_data.data_is_ptr) {
                                copy(ser_data.size, ser_data.data_ptr, data_dest);
                            } else {
                                assert(ser_data.size <= sizeof(ser_data.data_value));
                                copy(ser_data.size, &ser_data.data_value, data_dest);
                            }

                            if (strings_are_equal(ser->name, "guid")) {
                                if (entity->guid.value > level->first_available_guid) {
                                    level->first_available_guid = entity->guid.value + 1;
                                }
                            }
                        } else {
                            level_load_error = true;
                            log_print(LogLevel_Error, "Level Load Error: Member size mismatch: %u in file, expected %u", data_size, ser->size);
                            goto level_load_end;
                        }
                    } else {
                        log_print(LogLevel_Warn, "Level Load Warning: Could not find matching member '%.*s'", member_name_length, member_name);
                    }
                }
            }
        } else {
            level_load_error = true;
            log_print(LogLevel_Error, "Level Load Error: Header did not start with magic value 'levl'");
            goto level_load_end;
        }
#undef read_stream
    } else {
        level_load_error = true;
        log_print(LogLevel_Error, "Level Load Error: Could not open file '%.*s'", PRINTF_STRING(level_name));
        goto level_load_end;
    }

level_load_end:
    if (level_load_error) {
        level->entity_count = 1;
        level->first_available_guid = 1;
    } else {
        log_print(LogLevel_Info, "Successfully loaded level '%.*s'", PRINTF_STRING(level_name));
    }

    end_temporary_memory(temp);

    return !level_load_error;
}

inline PlayingMidi* play_midi(GameState* game_state, MidiTrack* track, u32 flags = 0, PlayingSound* sync_sound = 0, SoundtrackID source_soundtrack = { 0 }) {
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
    playing_midi->source_soundtrack = source_soundtrack;
    playing_midi->sync_sound = sync_sound;
    playing_midi->sync_sound->synced_midi = playing_midi;

    return playing_midi;
}

inline PlayingSound* play_soundtrack(GameState* game_state, SoundtrackID soundtrack_id, u32 flags) {
    PlayingSound* result = 0;
    Soundtrack* soundtrack = get_soundtrack(&game_state->assets, soundtrack_id);
    if (soundtrack) {
        Sound* sound = get_sound(&game_state->assets, soundtrack->sound);
        if (sound) {
            result = play_sound(&game_state->audio_mixer, sound, flags);
            for (u32 midi_index = 0; midi_index < soundtrack->midi_track_count; midi_index++) {
                MidiTrack* track = get_midi(&game_state->assets, soundtrack->midi_tracks[midi_index]);
                play_midi(game_state, track, flags, result, soundtrack_id);
            }
        }
    }

    return result;
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

internal void load_level_into_game_state(GameState* game_state, Level* level) {
    game_state->active_level = level;

    for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
        Entity* source = level->entities + entity_index;
        Entity* dest = game_state->entities + entity_index;

        *dest = *source;

        if (dest->type == EntityType_Player) {
            if (!game_state->player) {
                game_state->player = dest;
            } else {
                // You can only have one player!
                INVALID_CODE_PATH;
            }
        }
    }

    assert(game_state->player);

    Entity* player = game_state->player;

    game_state->last_activated_checkpoint = 0;
    f32 best_checkpoint_distance_sq = FLT_MAX;

    for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
        Entity* entity = game_state->entities + entity_index;

        if (entity->type == EntityType_Wall) {
            entity->start_p = entity->p;
        } else if (entity->type == EntityType_Checkpoint) {
            f32 distance_sq = length_sq(entity->p - player->p);
            if (distance_sq < best_checkpoint_distance_sq) {
                best_checkpoint_distance_sq = distance_sq;
                game_state->last_activated_checkpoint = entity;
            }
        }
    }

    game_state->entity_count = level->entity_count;
}

inline void switch_gamemode(GameState* game_state, GameMode game_mode) {
    if (game_state->game_mode == GameMode_Editor && game_mode != game_state->game_mode) {
        EditorState* editor = game_state->editor_state;
        editor->camera_p_on_exit = game_state->render_context.camera_p;
    }

    switch (game_mode) {
        case GameMode_StartScreen: {
        } break;

        case GameMode_Ingame: {
            load_level_into_game_state(game_state, game_state->active_level);
        } break;

        case GameMode_Editor: {
            EditorState* editor = game_state->editor_state;
            game_state->render_context.camera_p = editor->camera_p_on_exit;

            editor->shown = true;

            stop_all_sounds(&game_state->audio_mixer);
            stop_all_midi_tracks(game_state);
            game_state->entity_count = 0;
            game_state->player = 0;
        } break;
    }

    game_state->game_mode = game_mode;
}

struct CameraView {
    v2 camera_p;
    v2 camera_rotation_arm;
    f32 vfov;
};

inline CameraView get_camera_view(RenderContext* render_context, Entity* camera_zone, Entity* camera_target) {
    CameraView result;

    f32 aspect_ratio = get_aspect_ratio(render_context);

    v2 rel_camera_p = camera_target->p - camera_zone->p;
    v2 view_region = vec2(aspect_ratio*camera_zone->view_region_height, camera_zone->view_region_height);
    v2 movement_zone = max(vec2(0, 0), camera_zone->active_region - view_region);
    rel_camera_p = clamp(rel_camera_p, -0.5f*movement_zone, 0.5f*movement_zone);

    result.camera_p = camera_zone->p + rel_camera_p;
    result.camera_rotation_arm = camera_zone->camera_rotation_arm;
    result.vfov = view_region.y;

    return result;
}

internal void load_level(GameState* game_state, String level_name) {
    if (load_level_from_disk(game_state, game_state->background_level, level_name)) {
        Level* temp = game_state->active_level;
        game_state->active_level = game_state->background_level;
        game_state->background_level = temp;

        if (game_state->editor_state) {
            load_level_into_editor(game_state->editor_state, game_state->active_level);
        }
    }
}

internal GAME_UPDATE_AND_RENDER(game_update_and_render) {
    assert(memory->permanent_storage_size >= sizeof(GameState));

    game_config = memory->config;
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
    f32 aspect_ratio = cast(f32) width / cast(f32) height;

    if (!memory->initialized) {
        initialize_arena(&game_state->permanent_arena, memory->permanent_storage_size - sizeof(GameState), cast(u8*) memory->permanent_storage + sizeof(GameState));
        initialize_arena(&game_state->transient_arena, memory->transient_storage_size, memory->transient_storage);

        initialize_audio_mixer(&game_state->audio_mixer, &game_state->permanent_arena);

        // @TODO: Make the load_assets routine ignorant of the platform's file system
        load_assets(&game_state->assets, &game_state->transient_arena, "assets.pla");

        game_state->console_font = get_font_by_name(&game_state->assets, string_literal("editor_font"));
        game_state->console_state = push_struct(&game_state->permanent_arena, ConsoleState);
        initialize_render_context(&game_state->console_state->rc, render_commands, 1.0f);

        game_state->background_level = allocate_level(&game_state->permanent_arena, "background level");
        game_state->active_level = allocate_level(&game_state->permanent_arena, "no level");

        game_state->foreground_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        game_state->background_color = vec4(0.3f, 0.2f, 0.4f, 1.0f);

        game_state->editor_state = allocate_editor(game_state, render_commands);

        if (game_config.startup_level.len) {
            load_level(game_state, game_config.startup_level);
        }

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

    execute_console(game_state, game_state->console_state, input);

    if (game_state->game_mode == GameMode_StartScreen) {
        render_screenspace(render_context);
        push_clear(render_context, game_state->background_color);
    } else {
        render_worldspace(render_context, 30.0f);
        game_state->render_context.camera_rotation_arm = vec2(1, 0);

        push_clear(render_context, game_state->background_color);

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
            simulate_entities(game_state, input, frame_dt);

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

        if (game_state->game_mode == GameMode_Ingame) {
            Entity* camera_target = game_state->camera_target;
            Entity* camera_zone = game_state->active_camera_zone;
            if (camera_target && camera_zone) {
                if (!is_in_region(camera_zone->active_region + camera_target->collision, camera_target->p - camera_zone->p)) {
                    camera_zone = game_state->active_camera_zone = 0;
                }
            }

            if (camera_target) {
                game_state->render_context.camera_p = camera_target->p;

                if (camera_zone) {
                    Entity* prev_camera_zone = game_state->previous_camera_zone;
                    if (prev_camera_zone && game_state->camera_transition_t < 1.0f) {
                        game_state->mid_camera_transition = true;

                        CameraView prev_view = get_camera_view(render_context, prev_camera_zone, camera_target);
                        CameraView view = get_camera_view(render_context, camera_zone, camera_target);

                        f32 t = smootherstep(game_state->camera_transition_t);

                        render_context->camera_p = lerp(prev_view.camera_p, view.camera_p, t);
                        render_context->camera_rotation_arm = lerp(prev_view.camera_rotation_arm, view.camera_rotation_arm, t); // @TODO: This is obviously not how rotation goes.

                        render_worldspace(render_context, lerp(prev_view.vfov, view.vfov, t));

                        game_state->camera_transition_t += frame_dt / game_config.camera_transition_speed;
                    } else {
                        game_state->mid_camera_transition = false;

                        CameraView view = get_camera_view(render_context, camera_zone, camera_target);

                        render_context->camera_p = view.camera_p;
                        render_context->camera_rotation_arm = view.camera_rotation_arm;

                        render_worldspace(render_context, view.vfov);
                    }
                }
            }
        }

        EditorState* editor = game_state->editor_state;

        if (was_pressed(input->debug_fkeys[2])) {
            editor->shown = !editor->shown;
        }

        if (was_pressed(input->debug_fkeys[3])) {
            editor->show_statistics = !editor->show_statistics;
        }

        execute_editor(game_state, editor, input, memory->debug_info.frame_history);

        u32 render_entity_count = 0;
        Entity* render_entities = 0;
        if (game_state->game_mode == GameMode_Ingame) {
            render_entity_count = game_state->entity_count;
            render_entities = game_state->entities;
        } else if (game_state->game_mode == GameMode_Editor) {
            render_entity_count = game_state->active_level->entity_count;
            render_entities = game_state->active_level->entities;
        }

        for (u32 entity_index = 1; entity_index < render_entity_count; entity_index++) {
            Entity* entity = render_entities + entity_index;
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
                        Image* sprite = get_image(&game_state->assets, entity->sprite);
                        if (sprite) {
                            push_image(render_context, transform, sprite, entity->color);
                        } else {
                            INVALID_CODE_PATH;
                        }
                        push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), entity->active_region)), entity->color, ShapeRenderMode_Outline);
                        push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), vec2(aspect_ratio*entity->view_region_height, entity->view_region_height))), entity->color*vec4(1, 1, 1, 0.5f), ShapeRenderMode_Outline);
                    } break;

                    case EntityType_Checkpoint: {
                        Image* sprite = get_image(&game_state->assets, entity->sprite);
                        if (sprite) {
                            push_image(render_context, transform, sprite, entity->color);
                        } else {
                            INVALID_CODE_PATH;
                        }
                        push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), entity->checkpoint_zone)), game_state->last_activated_checkpoint == entity ? COLOR_GREEN : COLOR_RED, ShapeRenderMode_Outline);
                    } break;

                    case EntityType_SoundtrackPlayer: {
                        Image* sprite = get_image(&game_state->assets, entity->sprite);
                        if (sprite) {
                            push_image(render_context, transform, sprite, entity->color);
                        } else {
                            INVALID_CODE_PATH;
                        }
                        push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), entity->audible_zone + vec2(entity->horz_fade_region, entity->vert_fade_region))), COLOR_GREEN, ShapeRenderMode_Outline);
                        push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), entity->audible_zone)), COLOR_RED, ShapeRenderMode_Outline);
                    }

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
                if (entity->flags & EntityFlag_Invisible) {
                    entity->color.a *= 0.25f;
                }
            }
        }

        if (game_state->player && game_state->player->killed_this_frame) {
            game_state->player->dead = true;
            game_state->player->support = 0;
            game_state->player->killed_this_frame = false;
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
