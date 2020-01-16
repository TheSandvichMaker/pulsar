//
// @TODO: Take a more intelligent approach to dead entities so I don't have to put in tons of error-prone checks for it
//

inline b32 on_ground(Entity* entity) {
    b32 result = entity->flags & EntityFlag_OnGround;
    return result;
}

internal void execute_entity_logic(GameState* game_state, GameInput* input, f32 frame_dt) {
    RenderContext* render_context = &game_state->render_context;
    for (u32 entity_index = 1; entity_index < game_state->entity_count; entity_index++) {
        Entity* entity = game_state->entities + entity_index;
        assert(entity->type != EntityType_Null);

        entity->ddp = vec2(0, 0);

        if (entity->dead) {
            continue;
        }

        switch (entity->type) {
            case EntityType_Player: {
                if (!game_state->camera_target) {
                    game_state->camera_target = entity;
                }

                GameController* controller = &input->controller;
                f32 move_speed = entity->support ? 50.0f : 10.0f;
                if (controller->move_left.is_down) {
                    entity->ddp.x -= move_speed;
                }
                if (controller->move_right.is_down) {
                    entity->ddp.x += move_speed;
                }

                if (entity->support) {
                    // entity->ddp -= entity->support_normal*dot(entity->ddp, entity->support_normal);

                    if (was_pressed(controller->move_up)) {
                        // play_synth(&game_state->audio_mixer, synth_test_impulse);
                        // play_sound(&game_state->audio_mixer, game_state->test_sound);
                        entity->ddp.y += 400.0f;
                        entity->jumped = true;
                    }
                }
            } break;

            case EntityType_Wall: {
                v2 movement = {};
                for (u32 event_index = 0; event_index < game_state->midi_event_buffer_count; event_index++) {
                    ActiveMidiEvent event = game_state->midi_event_buffer[event_index];
                    if (event.note_value == entity->midi_note) {
                        if (event.type == MidiEvent_NoteOn) {
                            entity->midi_test_target.y += 0.5f*(event.note_value - 59);
                        } else if (event.type == MidiEvent_NoteOff) {
                            entity->midi_test_target.y -= 0.5f*(event.note_value - 59);
                        } else {
                            INVALID_CODE_PATH;
                        }
                    }
                }
                movement = 5.0f*(entity->midi_test_target - entity->p);
                // entity->p += entity->sim_dt*movement;
                entity->dp = movement;
                entity->movement_t += frame_dt;
#if 0
                Entity* sticking_entity = entity->sticking_entity;
                if (sticking_entity) {
                    sticking_entity->p += movement;
                    sticking_entity->sticking_dp = rcp_dt*movement;
                }
#endif
            } break;

            case EntityType_SoundtrackPlayer: {
                if (!entity->soundtrack_has_been_played) {
                    Soundtrack* soundtrack = get_soundtrack(&game_state->assets, entity->soundtrack_id);
                    play_soundtrack(game_state, soundtrack, entity->playback_flags);
                    entity->soundtrack_has_been_played = true;
                } else {
                    entity->color = COLOR_GREEN;
                }
            } break;

            case EntityType_CameraZone: {
                Entity* camera_target = game_state->camera_target;
                if (camera_target) {
                    if (is_in_aab(aab_center_dim(entity->p, entity->camera_zone), camera_target->p)) {
                        game_state->active_camera_zone = entity;
                    }
                }
            } break;

            case EntityType_Checkpoint: {
                if (game_state->player) {
                    AxisAlignedBox2 checkpoint_box = aab_center_dim(entity->p, entity->checkpoint_zone);
                    AxisAlignedBox2 player_box = offset(game_state->player->collision, game_state->player->p);
                    if (aab_contained_in_aab(checkpoint_box, player_box)) {
                        game_state->last_activated_checkpoint = entity;
                        entity->most_recent_player_position = game_state->player->p;
                    }
                }
            } break;

            INVALID_DEFAULT_CASE;
        }
    }
}

inline void kill_player(GameState* game_state) {
    assert(game_state->player);
    if (!game_state->player->dead) {
        game_state->player->dead = true;
        game_state->player->support = 0;
        game_state->player_respawn_timer = 2.0f;
    }
}

inline void process_collision_logic(GameState* game_state, Entity* collider, Entity* collidee) {
    if (collider->type == EntityType_Player) {
        if (collidee->flags & EntityFlag_Hazard) {
            kill_player(game_state);
        }
    }
}

struct TraceInfo {
    f32 t;
    v2 hit_normal;
};

inline b32 aab_trace(v2 start_p, v2 end_p, AxisAlignedBox2 aab, TraceInfo* info) {
    b32 result = false;

    f32 t_min = 1.0f;
    f32 t_epsilon = 1.0e-3f;

    v2 delta_p = end_p - start_p;

    {
        f32 t_result = (aab.min.x - start_p.x) / delta_p.x;
        if (t_result >= 0.0f && t_result < t_min) {
            f32 y = start_p.y + t_result*delta_p.y;
            if (y >= aab.min.y && y < aab.max.y) {
                t_min = max(0.0f, t_result - t_epsilon);
                result = true;
                info->hit_normal = vec2(-1, 0);
            }
        }
    }

    {
        f32 t_result = (aab.max.x - start_p.x) / delta_p.x;
        if (t_result >= 0.0f && t_result < t_min) {
            f32 y = start_p.y + t_result*delta_p.y;
            if (y >= aab.min.y && y < aab.max.y) {
                t_min = max(0.0f, t_result - t_epsilon);
                result = true;
                info->hit_normal = vec2(1, 0);
            }
        }
    }

    {
        f32 t_result = (aab.min.y - start_p.y) / delta_p.y;
        if (t_result >= 0.0f && t_result < t_min) {
            f32 x = start_p.x + t_result*delta_p.x;
            if (x >= aab.min.x && x < aab.max.x) {
                t_min = max(0.0f, t_result - t_epsilon);
                result = true;
                info->hit_normal = vec2(0, -1);
            }
        }
    }

    {
        f32 t_result = (aab.max.y - start_p.y) / delta_p.y;
        if (t_result >= 0.0f && t_result < t_min) {
            f32 x = start_p.x + t_result*delta_p.x;
            if (x >= aab.min.x && x < aab.max.x) {
                t_min = max(0.0f, t_result - t_epsilon);
                result = true;
                info->hit_normal = vec2(0, 1);
            }
        }
    }

    info->t = t_min;

    return result;
}

inline v2 grazing_reflect(v2 in, v2 n) {
    v2 result = in - n*dot(in, n);
    return result;
}

inline void player_move(GameState* game_state, Entity* entity, f32 dt) {
    /*
     * @TODO: Entities shouldn't be modified in here at all, that should be deferred,
     * all the changes applied to entities should be stored in PhysicsMoveResult instead.
     */

    f32 t = 1.0f;

    f32 epsilon = 1.0e-3f;

    v2 gravity = vec2(0.0f, -9.8f);

    v2 dp = entity->dp;
    v2 ddp = entity->ddp;

    if (!entity->support) {
        ddp += gravity;
    } else {
        dp += entity->support->dp;
        ddp.x -= entity->support->surface_friction*dp.x;
        gravity = vec2(0, 0);
    }

    v2 delta = 0.5f*ddp*square(dt) + dp*dt;
    entity->dp += ddp*dt;

    if (entity->flags & EntityFlag_Collides) {
        u32 max_iterations = 8;
        u32 iteration_count = 0;
        while (t > epsilon && iteration_count < max_iterations) {
            b32 did_collide = false;

            // @TODO: Optimized spatial indexing of sorts?
            for (u32 test_entity_index = 0; test_entity_index < game_state->entity_count; test_entity_index++) {
                Entity* test_entity = game_state->entities + test_entity_index;
                if (entity != test_entity &&
                    (test_entity->flags & EntityFlag_Collides) &&
                    !(test_entity->flags & EntityFlag_Physical) &&
                    test_entity != entity->support
                ) {
                    // @Note: Right now the policy is that there are no collisions between two physical entities, so test_delta is calculated using only the test_entity's dp.
                    // ddp is for physics.
                    v2 test_delta = test_entity->dp*dt;
                    test_entity->sticking_entity = 0;
#if 1
                    //
                    // AABB path
                    //

                    v2 rel_p = entity->p - test_entity->p;
                    AxisAlignedBox2 test_aab = aab_sum(entity->collision, test_entity->collision);

                    if (is_in_aab(test_aab, rel_p)) {
                        f32 best_distance = F32_MAX;
                        v2 best_move = vec2(0, 0);

                        f32 test_distance = entity->p.x - test_aab.min.x;
                        if (test_distance < best_distance) {
                            best_distance = test_distance;
                            best_move = vec2(-1, 0);
                        }

                        test_distance = test_aab.max.x - entity->p.x;
                        if (test_distance < best_distance) {
                            best_distance = test_distance;
                            best_move = vec2(1, 0);
                        }

                        test_distance = entity->p.y - test_aab.min.y;
                        if (test_distance < best_distance) {
                            best_distance = test_distance;
                            best_move = vec2(0, -1);
                        }

                        test_distance = test_aab.max.y - entity->p.y;
                        if (test_distance < best_distance) {
                            best_distance = test_distance;
                            best_move = vec2(0, 1);
                        }

                        t = 0.0f;

                        delta = best_move*best_distance;

                        entity->dp = vec2(0, 0);

                        process_collision_logic(game_state, entity, test_entity);

                        break;
                    } else {
                        v2 rel_delta = delta - test_delta;

                        TraceInfo info;
                        if (aab_trace(rel_p, rel_p + rel_delta, test_aab, &info)) {
                            // @Note: If you collided with a face that faces away from where you're going, then that's probably an epsilon problem.
                            if (dot(rel_delta, info.hit_normal) < 0.0f) {
                                t -= 1.0f - info.t;

                                did_collide = true;

                                delta *= info.t;

                                if (t == 0.0f) {
                                    delta += info.hit_normal*epsilon;
                                }

                                entity->dp = grazing_reflect(entity->dp, info.hit_normal);

                                if (dot(info.hit_normal, gravity) < 0) {
                                    entity->support = test_entity;
                                    entity->support_normal = info.hit_normal;
                                }

                                process_collision_logic(game_state, entity, test_entity);
                            }
                        }
                    }
#else
                    //
                    // Old GJK path
                    //

                    v2 relative_delta = delta - test_delta;

                    Transform2D t = transform2d(entity->p, vec2(1.0f, 1.0f), relative_delta);
                    Transform2D test_t = transform2d(test_entity->p);

                    CollisionInfo collision;
                    if (gjk_intersect(t, entity->collision, test_t, test_entity->collision, &collision, &game_state->transient_arena)) {
                        did_collide = true;
                        result->colliding_entity = test_entity;
                        entity->friction_of_last_touched_surface = test_entity->surface_friction;

                        f32 theta_times_length_of_delta = dot(collision.vector, relative_delta);

                        collision.depth += epsilon;

                        if (theta_times_length_of_delta > theta_times_length_of_delta) {
                            f32 t_penetration = collision.depth / theta_times_length_of_delta;
                            delta = delta*t_penetration;
                            result->simulated_dt = dt*(1.0f-t_penetration);
                        } else {
                            // @TODO: Think about this case
                            delta -= collision.depth*collision.vector;
                        }

                        if (collision.vector.y < -0.707f) {
                            if (!on_ground(entity)) {
                                entity->flags |= EntityFlag_OnGround;
                            }
                            entity->off_ground_timer = 0.0f;
                            entity->support = test_entity;
                            test_entity->sticking_entity = entity;
                        }

                        result->dp -= collision.vector*dot(result->dp, collision.vector);
                    }
#endif
                }
            }

            if (!did_collide) {
                t = 0.0f;
            }

            entity->p += delta;

            iteration_count++;
        }

        if (iteration_count >= max_iterations) {
            assert(iteration_count == max_iterations);
            log_print(LogLevel_Warn, "Player move exceeded %u iterations", max_iterations);
        }
    }

    if (entity->jumped) {
        entity->jumped = false;
        entity->support = 0;
    }
}

#if 0
internal void simulate_entity(GameState* game_state, Entity* entity, PhysicsMoveResult* move, f32 dt) {
    if (!entity->dead) {
        if (entity->flags & EntityFlag_Physical) {
            assert(move);
            entity->p = move->p;
            entity->dp = move->dp;
            entity->ddp = move->ddp;
            entity->support = move->support;
            entity->support_normal = move->support_normal;
            if (move->colliding_entity) {
                move->colliding_entity->color = vec4(0, 0.5f, 0, 1);
            }
        } else {
            entity->p += entity->dp*dt;
        }

        switch (entity->type) {
            case EntityType_Player: {
                if (entity->support) {
                    AxisAlignedBox2 test_aab = aab_sum(entity->collision, entity->support->collision);
                    v2 rel_p = entity->p - entity->support->p;
                    if (rel_p.x < test_aab.min.x || rel_p.x > test_aab.max.x) {
                        entity->support = 0;
                    }
                }

                if (move && move->colliding_entity) {
                    if (move->colliding_entity->flags & EntityFlag_Hazard) {
                        kill_player(game_state);
                    }
                }
            } break;
        }
    }
}
#endif

internal void run_simulation(GameState* game_state, GameInput* input, f32 frame_dt) {
    game_state->midi_event_buffer_count = 0;
    for (PlayingMidi** playing_midi_ptr = &game_state->first_playing_midi; *playing_midi_ptr;) {
        PlayingMidi* playing_midi = *playing_midi_ptr;

        MidiTrack* track = playing_midi->track;
        if (playing_midi->event_index < track->event_count) {
#if 0
            // @Note: I moved the sync to output_playing_sounds
            if (playing_midi->sync_sound) {
                assert(track->ticks_per_second == 48000);
                playing_midi->tick_timer = playing_midi->sync_sound->samples_played;
            }
#endif

            u32 ticks_for_frame = round_f32_to_u32((cast(f32) track->ticks_per_second)*frame_dt);
            playing_midi->tick_timer += ticks_for_frame;

            MidiEvent event = track->events[playing_midi->event_index];
            while (event.absolute_time_in_ticks <= playing_midi->tick_timer && playing_midi->event_index < track->event_count) {
                playing_midi->event_index++;

                ActiveMidiEvent* active_event = game_state->midi_event_buffer + game_state->midi_event_buffer_count++;
                active_event->midi_event = event;
                f32 timing_into_frame = cast(f32) (playing_midi->tick_timer - event.absolute_time_in_ticks) / cast(f32) ticks_for_frame;
                active_event->dt_left = frame_dt*(1.0f - timing_into_frame);

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

    execute_entity_logic(game_state, input, frame_dt);
    player_move(game_state, game_state->player, frame_dt);

    for (u32 entity_index = 0; entity_index < game_state->entity_count; entity_index++) {
        Entity* entity = game_state->entities + entity_index;
        if (!(entity->flags & EntityFlag_Physical)) {
            entity->p += entity->dp*frame_dt;
        }
    }
}
