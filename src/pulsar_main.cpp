#include "pulsar_main.h"
#include "pulsar_generated_post_headers.h"

#define STB_SPRINTF_STATIC 1
#define STB_SPRINTF_IMPLEMENTATION 1
#include "external/stb_sprintf.h"

#include "pulsar_assets.cpp"
#include "pulsar_audio_mixer.cpp"
#include "pulsar_render_commands.cpp"
#include "pulsar_gjk.cpp"
#include "pulsar_entity.cpp"
#include "pulsar_sim.cpp"
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

        game_state->test_music = get_sound_by_name(&game_state->assets, "test_music");
        game_state->test_sound = get_sound_by_name(&game_state->assets, "test_sound");

        {
            v2* verts = push_array(&game_state->permanent_arena, 4, v2, no_clear());
            verts[0] = { -0.2f, 0.0f };
            verts[1] = {  0.2f, 0.0f };
            verts[2] = {  0.2f, 1.0f };
            verts[3] = { -0.2f, 1.0f };
            game_state->player_collision = polygon(4, verts);
        }

        game_state->active_level = allocate_level(&game_state->permanent_arena, "Default Level");

        game_state->foreground_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        game_state->background_color = vec4(0.2f, 0.3f, 0.5f, 1.0f);

        game_state->editor_state = allocate_editor(game_state, render_commands, game_state->active_level);

        switch_gamemode(game_state, GameMode_Editor);

        initialize_render_context(&game_state->render_context, render_commands, 30.0f);

        memory->initialized = true;
    }

    v2 mouse_p = vec2(input->mouse_x, input->mouse_y);
    f32 frame_dt = input->frame_dt;

    RenderContext* render_context = &game_state->render_context;

    render_worldspace(render_context, 30.0f);
    game_state->render_context.camera_rotation_arm = vec2(1, 0);

#if 0
    game_state->rotation += dt;
    if (game_state->rotation > PI_32) {
        game_state->rotation -= TAU_32;
    }
    game_state->render_context.camera_rotation_arm = arm2(game_state->rotation);
#endif

    push_clear(render_context, game_state->background_color);

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

    EditorState* editor = game_state->editor_state;

    if (was_pressed(input->debug_fkeys[2])) {
        editor->shown = !editor->shown;
    }

    execute_editor(game_state, editor, input, memory->debug_info.frame_history);

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

    //
    // Post-Sim logic
    //

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
                    push_image(render_context, transform, entity->sprite, entity->color);
                    push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), entity->camera_zone)), entity->color, ShapeRenderMode_Outline);

                    f32 aspect_ratio = cast(f32) width / cast(f32) height;
                    f32 zone_height = entity->camera_zone.y;
                    AxisAlignedBox2 visible_zone = aab_center_dim(vec2(0, 0), vec2(aspect_ratio*zone_height, zone_height));
                    push_shape(render_context, transform, rectangle(visible_zone), entity->color*vec4(1, 1, 1, 0.5f), ShapeRenderMode_Outline);
                } break;

                case EntityType_Checkpoint: {
                    push_image(render_context, transform, entity->sprite, entity->color);
                    push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), entity->checkpoint_zone)), game_state->last_activated_checkpoint == entity ? COLOR_GREEN : COLOR_RED, ShapeRenderMode_Outline);
                } break;

                default: {
                    if (entity->sprite) {
                        push_image(render_context, transform, entity->sprite, entity->color);
                    } else {
                        push_shape(render_context, transform, entity->collision, entity->color);
                    }
                } break;
            }
        }

        if (entity->sprite) {
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
