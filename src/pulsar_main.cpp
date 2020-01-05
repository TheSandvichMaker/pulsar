#include "pulsar_code_generator.h"
#include "pulsar_generated_pre_headers.h"
#include "pulsar_main.h"
#include "pulsar_generated_post_headers.h"

#define STB_SPRINTF_STATIC 1
#define STB_SPRINTF_IMPLEMENTATION 1
#include "stb_sprintf.h"

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

    push_shape(&dbg_game_state->render_group, t, arrow, color, ShapeRenderMode_Outline);
}

#define MAX_COLLISION_ITERATIONS 4
inline void physics_move(GameState* game_state, Entity* entity, v2 ddp, f32 dt) {
    /*
     * @TODO: Figure out some of the cases in which the collision handling will
     * fail, which I'm sure there are some, such as extreme velocities
     * (which might happen in cases of frame drops if I choose to support
     * dynamic timesteps). One suspicion I have is that if an entity penetrates
     * deep enough into another, the collision might resolve to put it on the other
     * side, allowing very fast entities to clip through each other.
     */

    f32 epsilon = 1.0e-3f;

    if (entity->flags & EntityFlag_Physical) {
        f32 gravity = 9.8f;
        ddp.y -= gravity;

        entity->support = 0;

        if (on_ground(entity)) {
            ddp.x -= entity->friction_of_last_touched_surface*entity->dp.x;
            entity->flags &= ~EntityFlag_OnGround;
            // f32 off_ground_time = 0.2f;
            // if (entity->off_ground_timer < off_ground_time) {
            //     entity->off_ground_timer += dt;
            // } else {
            //     entity->flags &= ~EntityFlag_OnGround;
            // }
        }

        v2 total_delta = 0.5f*ddp*square(dt) + entity->dp*dt;
        entity->dp += ddp*dt;

        entity->friction_of_last_touched_surface = 0.0f;

        f32 t_left = 1.0f;
        v2 delta = t_left*total_delta;

        if (entity->flags & EntityFlag_Collides) {
            // @TODO: Optimized spatial indexing of sorts?
            for (u32 test_entity_index = 0; test_entity_index < game_state->entity_count; test_entity_index++) {
                Entity* test_entity = game_state->entities + test_entity_index;
                if (entity != test_entity && (test_entity->flags & EntityFlag_Collides) && !(test_entity->flags & EntityFlag_Physical)) {
                    test_entity->sticking_entity = 0;
                    Transform2D t = transform2d(entity->p, vec2(1.0f, 1.0f), delta);
                    Transform2D test_t = transform2d(test_entity->p);

                    CollisionInfo collision;
                    if (gjk_intersect(t, entity->collision, test_t, test_entity->collision, &collision, &game_state->transient_arena)) {
                        entity->friction_of_last_touched_surface = test_entity->surface_friction;

                        f32 theta_times_length_of_delta = dot(collision.vector, delta);

                        collision.depth += epsilon;

                        if (collision.vector.y < -0.707f) {
                            if (!on_ground(entity)) {
                                entity->flags |= EntityFlag_OnGround;
                            }
                            entity->off_ground_timer = 0.0f;
                            entity->support = test_entity;
                            test_entity->sticking_entity = entity;
                        }

                        // dbg_draw_arrow(entity->p, entity->p + collision.vector*collision.depth*100.0f, vec4(1, 0, 0, 1));

#if 0
                        if (theta_times_length_of_delta > 0.0f) {
                            f32 penetration_along_delta = collision.depth / theta_times_length_of_delta;
                            v2 legal_move = delta*(1.0f - penetration_along_delta);
                            delta *= penetration_along_delta;
                            delta -= collision.vector*dot(delta, collision.vector);
                            delta += legal_move;
                        } else {
#endif
                            delta -= collision.vector*collision.depth;
                        // }

                        entity->dp -= (collision.vector*collision.depth)/dt;
                        // entity->dp -= collision.vector*dot(entity->dp, collision.vector);
                    }
#if 0
                    u32 iteration = 0;
                    do {
                        CollisionInfo collision;
                        if (gjk_intersect(t, entity->collision, test_t, test_entity->collision, &collision, &game_state->transient_arena)) {
                            dbg_draw_arrow(entity->p, entity->p + collision.vector*25.0f, vec4(1, 0, 1, 1));
                            if (collision.vector.y < -0.707f) {
                                if (!on_ground(entity)) {
                                    entity->flags |= EntityFlag_OnGround;
                                }
                                entity->off_ground_timer = 0.0f;
                                test_entity->sticking_entity = entity;
                            }
                            f32 depth = collision.depth + epsilon;
                            f32 theta_times_length_of_delta = dot(collision.vector, delta);
                            if (theta_times_length_of_delta == 0) {
                                delta = -collision.vector*depth;
                                break;
                            } else {
                                f32 penetration_along_delta = depth / theta_times_length_of_delta;
                                f32 t_spent = max(0.0f, 1.0f - penetration_along_delta);
                                t_left -= t_spent;
                                if (t_left > 1.0f || t_left < 0.0f) {
                                    __debugbreak();
                                }

                                entity->p   += t_spent*delta;
                                entity->dp  -= collision.vector*dot(entity->dp, collision.vector);
                                total_delta -= collision.vector*dot(total_delta, collision.vector);
                            }
                        } else {
                            break;
                        }

                        delta = t_left*total_delta;

                        if (length(delta) > 100.0f) {
                            __debugbreak();
                        }

                        t.offset = entity->p;
                        t.sweep = delta;

                        iteration++;
                    } while (iteration < MAX_COLLISION_ITERATIONS && t_left > epsilon);
#endif
                }
            }
        }

        entity->p += delta;
    }
}

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

internal u32 parse_utf8_codepoint(char* input_string, u32* out_codepoint) {
    u32 num_bytes = 0;
    char* at = input_string;
    u32 codepoint = 0;

    if (!at[0]) {
        // @Note: We've been null terminated.
    } else {
        u8 utf8_mask[] = {
            (1 << 7) - 1,
            (1 << 5) - 1,
            (1 << 4) - 1,
            (1 << 3) - 1,
        };
        u8 utf8_matching_value[] = { 0, 0xC0, 0xE0, 0xF0 };
        if ((*at & ~utf8_mask[0]) == utf8_matching_value[0]) {
            num_bytes = 1;
        } else if ((u8)(*at & ~utf8_mask[1]) == utf8_matching_value[1]) {
            num_bytes = 2;
        } else if ((u8)(*at & ~utf8_mask[2]) == utf8_matching_value[2]) {
            num_bytes = 3;
        } else if ((u8)(*at & ~utf8_mask[3]) == utf8_matching_value[3]) {
            num_bytes = 4;
        } else {
            INVALID_CODE_PATH;
        }

        u32 offset = 6 * (num_bytes - 1);
        for (u32 byte_index = 0; byte_index < num_bytes; byte_index++) {
            if (byte_index == 0) {
                codepoint = (*at & utf8_mask[num_bytes-1]) << offset;
            } else {
                if (*at != 0) {
                    codepoint |= (*at & ((1 << 6) - 1)) << offset;
                } else {
                    // @Note: You don't really want to  assert on a gibberish
                    // bit of unicode, but it's here to draw my attention.
                    INVALID_CODE_PATH;
                }
            }
            offset -= 6;
            at++;
        }
    }

    *out_codepoint = codepoint;
    return num_bytes;
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
}

inline void switch_gamemode(GameState* game_state, GameMode game_mode) {
    switch (game_mode) {
        case GameMode_Ingame: {
            if (game_state->game_mode == GameMode_Editor) {
                EditorState* editor = game_state->editor_state;
                editor->camera_p_on_exit = game_state->render_group.camera_p;
            }

            Level* level = game_state->active_level;
            for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
                game_state->entities[entity_index] = level->entities[entity_index];
            }
            game_state->entity_count = level->entity_count;
        } break;

        case GameMode_Editor: {
            if (game_state->game_mode == GameMode_Ingame) {
                EditorState* editor = game_state->editor_state;
                game_state->render_group.camera_p = editor->camera_p_on_exit;
            }

            stop_all_sounds(&game_state->audio_mixer);
            game_state->entity_count = 0;
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
        game_state->background_color = vec4(0.5f, 0.2f, 0.2f, 1.0f);

        game_state->editor_state = allocate_editor(game_state, render_commands, game_state->active_level);

        switch_gamemode(game_state, GameMode_Editor);

        initialize_render_group(&game_state->render_group, render_commands, 30.0f);

        memory->initialized = true;
    }

    v2 mouse_p = vec2(input->mouse_x, input->mouse_y);
    f32 dt = input->frame_dt;
    f32 rcp_dt = 1.0f / dt;

    RenderGroup* render_group = &game_state->render_group;

    render_worldspace(render_group, 30.0f);
    game_state->render_group.camera_rotation_arm = vec2(1, 0);

#if 0
    game_state->rotation += dt;
    if (game_state->rotation > PI_32) {
        game_state->rotation -= TAU_32;
    }
    game_state->render_group.camera_rotation_arm = arm2(game_state->rotation);
#endif

    push_clear(render_group, game_state->background_color);

    if (game_state->game_mode == GameMode_Ingame) {
        game_state->midi_event_buffer_count = 0;
        for (PlayingMidi** playing_midi_ptr = &game_state->first_playing_midi; *playing_midi_ptr;) {
            PlayingMidi* playing_midi = *playing_midi_ptr;

            MidiTrack* track = playing_midi->track;
            if (playing_midi->event_index < track->event_count) {
                if (playing_midi->sync_sound) {
                    assert(track->ticks_per_second == 48000);
                    playing_midi->tick_timer = playing_midi->sync_sound->samples_played;
                }

                u32 ticks_for_frame = round_f32_to_u32((cast(f32) track->ticks_per_second)*dt);
                playing_midi->tick_timer += ticks_for_frame;

                MidiEvent event = track->events[playing_midi->event_index];
                while (event.absolute_time_in_ticks <= playing_midi->tick_timer && playing_midi->event_index < track->event_count) {
                    playing_midi->event_index++;

                    ActiveMidiEvent* active_event = game_state->midi_event_buffer + game_state->midi_event_buffer_count++;
                    active_event->midi_event = event;
                    f32 timing_into_frame = cast(f32) (playing_midi->tick_timer - event.absolute_time_in_ticks) / cast(f32) ticks_for_frame;
                    active_event->dt_left = dt*(1.0f - timing_into_frame);

                    assert(game_state->midi_event_buffer_count < ARRAY_COUNT(game_state->midi_event_buffer));

                    event = track->events[playing_midi->event_index];
                }
            }

            if (playing_midi->event_index >= track->event_count) {
                assert(playing_midi->event_index == track->event_count);
                if (playing_midi->flags & Playback_Looping) {
                    playing_midi->event_index = 0;
                    playing_midi->tick_timer = 0;
                    playing_midi_ptr = &playing_midi->next;
                } else {
                    *playing_midi_ptr = playing_midi->next;
                    playing_midi->next = game_state->first_free_playing_midi;
                    game_state->first_free_playing_midi = playing_midi;
                }
            } else {
                playing_midi_ptr = &playing_midi->next;
            }
        }

        //
        // Gameplay Logic
        //

        if (game_state->game_mode == GameMode_Ingame) {
            Entity* camera_target = game_state->camera_target;
            if (camera_target) {
                render_group->camera_p = camera_target->p;
            }
        }

        simulate_entity_logic(game_state, input, dt);

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

    execute_editor(game_state, editor, input);

    //
    // Physics & Rendering
    //

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

    if (game_state->game_mode == GameMode_Ingame) {
        for (u32 entity_index = 1; entity_index < active_entity_count; entity_index++) {
            Entity* entity = active_entities + entity_index;
            assert(entity->type != EntityType_Null);

            if (!(entity->flags & EntityFlag_Physical)) {
                entity->p += dt*entity->dp;
                if (entity->sticking_entity) {
                    // entity->sticking_entity->p += dt*entity->dp;
                    // entity->sticking_entity->dp += entity->dp;
                }
            }
        }

        for (u32 entity_index = 1; entity_index < active_entity_count; entity_index++) {
            Entity* entity = active_entities + entity_index;
            assert(entity->type != EntityType_Null);

            // if (entity->flags & EntityFlag_Physical) {
            //     physics_move(game_state, entity, entity->ddp, entity->sim_dt);
            // }

            if (entity->type == EntityType_Player) {
                /*
                 * @TODO: Figure out some of the cases in which the collision handling will
                 * fail, which I'm sure there are some, such as _very_ extreme velocities
                 * (which might happen in cases of frame drops if I choose to support
                 * dynamic timesteps). One suspicion I have is that if an entity penetrates
                 * deep enough into another, the collision might resolve to put it on the other
                 * side, allowing very fast entities to clip through each other.
                 */

                v2 ddp = entity->ddp;

                f32 epsilon = 1.0e-3f;

                // @TODO: Some kind of relationship with real world units, get away from using pixels.
                f32 gravity = 9.8f;
                ddp.y -= gravity;

                entity->support = 0;

                if (on_ground(entity)) {
                    ddp.x -= entity->friction_of_last_touched_surface*entity->dp.x;
                    entity->flags &= ~EntityFlag_OnGround;
                    // f32 off_ground_time = 0.2f;
                    // if (entity->off_ground_timer < off_ground_time) {
                    //     entity->off_ground_timer += dt;
                    // } else {
                    //     entity->flags &= ~EntityFlag_OnGround;
                    // }
                }

                v2 dp = entity->dp + (entity->support ? entity->support->dp : vec2(0, 0));
                v2 total_delta = 0.5f*ddp*square(dt) + dp*dt;
                entity->dp += ddp*dt;

                entity->friction_of_last_touched_surface = 0.0f;

                f32 t_left = 1.0f;
                v2 delta = t_left*total_delta;

                v2 p = entity->p;
                // if (entity->support) {
                //     p = entity->support->p + entity->local_p + entity->support->dp;
                // }

                if (entity->flags & EntityFlag_Collides) {
                    // @TODO: Optimized spatial indexing of sorts?
                    for (u32 test_entity_index = 0; test_entity_index < game_state->entity_count; test_entity_index++) {
                        Entity* test_entity = game_state->entities + test_entity_index;
                        if (entity != test_entity && (test_entity->flags & EntityFlag_Collides) && !(test_entity->flags & EntityFlag_Physical)) {
                            test_entity->sticking_entity = 0;
                            Transform2D t = transform2d(entity->p, vec2(1.0f, 1.0f), delta);
                            v2 test_sweep = (test_entity == entity->support) ? test_entity->dp : vec2(0, 0);
                            Transform2D test_t = transform2d(test_entity->p, vec2(1.0f, 1.0f), test_sweep);

                            CollisionInfo collision;
                            if (gjk_intersect(t, entity->collision, test_t, test_entity->collision, &collision, &game_state->transient_arena)) {
                                entity->friction_of_last_touched_surface = test_entity->surface_friction;

                                f32 theta_times_length_of_delta = dot(collision.vector, delta);

                                collision.depth += epsilon;

                                if (collision.vector.y < -0.707f) {
                                    if (!on_ground(entity)) {
                                        entity->flags |= EntityFlag_OnGround;
                                    }
                                    entity->off_ground_timer = 0.0f;
                                    entity->support = test_entity;
                                    entity->support_normal = -collision.vector;
                                    entity->local_p = entity->p - test_entity->p;
                                    test_entity->sticking_entity = entity;
                                }

                                dbg_draw_arrow(entity->p, entity->p + collision.vector*collision.depth*100.0f, vec4(1, 0, 0, 1));

                                if (theta_times_length_of_delta > 0.0f) {
                                    f32 penetration_along_delta = collision.depth / theta_times_length_of_delta;
                                    v2 legal_move = delta*(1.0f - penetration_along_delta);
                                    delta *= penetration_along_delta;
                                    delta -= collision.vector*dot(delta, collision.vector);
                                    delta += legal_move;
                                } else {
                                    delta -= collision.vector*collision.depth;
                                }

                                // delta -= collision.vector*dot(collision.vector, delta);
                                entity->dp -= collision.vector*dot(collision.vector, entity->dp);
                            }
                        }
                    }
                }

                entity->p += delta;
            }

            if (on_ground(entity)) {
                entity->color = vec4(0, 0, 1, 1);
            }
        }
    }

    for (u32 entity_index = 1; entity_index < active_entity_count; entity_index++) {
        Entity* entity = active_entities + entity_index;
        assert(entity->type != EntityType_Null);

        if (!(entity->flags & EntityFlag_Invisible) || (game_state->game_mode == GameMode_Editor && editor->shown)) {
            Transform2D transform = transform2d(entity->p);
            switch (entity->type) {
                case EntityType_CameraZone: {
                    f32 aspect_ratio = cast(f32) width / cast(f32) height;
                    transform.rotation_arm = entity->camera_rotation_arm;
                    push_image(render_group, transform, entity->sprite, entity->color);
                    push_shape(render_group, transform, rectangle(entity->camera_zone), entity->color, ShapeRenderMode_Outline);
                    f32 zone_height = get_dim(entity->camera_zone).y;
                    Rect2 visible_zone = rect_center_dim(get_center(entity->camera_zone), vec2(aspect_ratio*zone_height, zone_height));
                    push_shape(render_group, transform, rectangle(visible_zone), entity->color, ShapeRenderMode_Outline);
                } break;

                default: {
                    if (entity->sprite) {
                        push_image(render_group, transform, entity->sprite, entity->color);
                    } else {
                        push_shape(render_group, transform, entity->collision, entity->color);
                    }
                } break;
            }
        }

        if (entity->sprite) {
            entity->color = COLOR_WHITE;
        } else {
            entity->color = game_state->foreground_color;
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
