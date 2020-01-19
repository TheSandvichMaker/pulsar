//
// @TODO: Take a more intelligent approach to dead entities so I don't have to put in tons of error-prone checks for it
//

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

inline b32 on_ground(Entity* entity) {
    b32 result = entity->flags & EntityFlag_OnGround;
    return result;
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

inline void player_move(GameState* game_state, Entity* entity, f32 dt) {
    f32 t = 1.0f;

    f32 epsilon = 1.0e-3f;

    v2 ddp = entity->ddp;

    v2 gravity = vec2(0.0f, entity->gravity);

    if (!entity->support) {
        ddp += gravity;
    }

    f32 max_x_vel =  10.0f;
    f32 min_y_vel = -40.0f;
    f32 max_y_vel =  40.0f;

    ddp.x = (clamp(ddp.x*dt + entity->dp.x, -max_x_vel, max_x_vel) - entity->dp.x) / dt;
    ddp.y = (clamp(ddp.y*dt + entity->dp.y,  min_y_vel, max_y_vel) - entity->dp.y) / dt;

    entity->dp += ddp*dt;
    // entity->dp.x = clamp(entity->dp.x, -max_x_vel, max_x_vel);
    // entity->dp.y = clamp(entity->dp.y,  min_y_vel, max_y_vel);

    // @TODO: Figure out collision properly instead of this hacky crap that idk what it even is

    if (entity->flags & EntityFlag_Collides) {
        u32 max_iterations = 4;
        u32 iteration_count = 0;
        while (t > epsilon && iteration_count < max_iterations) {
            v2 mod_dp = {};
            if (!entity->support) {
                if (entity->was_supported) {
                    entity->was_supported = false;
                    entity->dp += entity->support_dp;
                    entity->support_dp = vec2(0, 0);
                }
            } else {
                entity->was_supported = true;
                entity->support_dp = entity->support_dp;
                mod_dp += entity->support->dp;
            }

            v2 delta = 0.5f*ddp*square(t*dt) + (entity->dp + mod_dp)*t*dt;

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

                    v2 rel_p = entity->p - test_entity->p;
                    v2 test_region = entity->collision + test_entity->collision;
                    AxisAlignedBox2 test_aab = aab_center_dim(vec2(0, 0), test_region);

                    if (is_in_aab(test_aab, rel_p)) {
                        f32 best_distance = F32_MAX;
                        v2 best_move = vec2(0, 0);

                        f32 test_distance = rel_p.x - test_aab.min.x;
                        if (test_distance < best_distance) {
                            best_distance = test_distance + epsilon; // @TODO: The epsilon is to deal with the whole inclusive/exclusive bounds for the AABB, but is a hack, not good
                            best_move = vec2(-1, 0);
                        }

                        test_distance = test_aab.max.x - rel_p.x;
                        if (test_distance < best_distance) {
                            best_distance = test_distance;
                            best_move = vec2(1, 0);
                        }

                        test_distance = rel_p.y - test_aab.min.y;
                        if (test_distance < best_distance) {
                            best_distance = test_distance + epsilon; // @TODO: The epsilon is to deal with the whole inclusive/exclusive bounds for the AABB, but is a hack, not good
                            best_move = vec2(0, -1);
                        }

                        test_distance = test_aab.max.y - rel_p.y;
                        if (test_distance < best_distance) {
                            best_distance = test_distance;
                            best_move = vec2(0, 1);
                        }

                        log_print(LogLevel_Warn, "Player got stuck in an entity. Best move: { %g, %g }", best_move.x*best_distance, best_move.y*best_distance);

                        entity->p += best_move*best_distance;

                        process_collision_logic(game_state, entity, test_entity);
                    } else {
                        v2 rel_delta = delta - test_delta;

                        TraceInfo info;
                        if (aab_trace(rel_p, rel_p + rel_delta, test_aab, &info)) {
                            // @Note: If you collided with a face that faces away from where you're going, then that's probably an epsilon problem.
                            if (dot(rel_delta, info.hit_normal) < 0.0f) {
                                t -= info.t;

                                did_collide = true;

                                if (t == 0.0f) {
                                    delta += info.hit_normal*epsilon;
                                }

                                entity->p += info.t*delta;

                                ddp = grazing_reflect(ddp, info.hit_normal);
                                entity->dp = grazing_reflect(entity->dp, info.hit_normal);

                                if (dot(info.hit_normal, gravity) < 0) {
                                    entity->support = test_entity;
                                    entity->support_normal = info.hit_normal;
                                    entity->p += test_entity->dp*dt*info.t;
                                }

                                process_collision_logic(game_state, entity, test_entity);
                            }
                        }
                    }
                }
            }

            if (!did_collide) {
                break;
            }

            iteration_count++;
        }

        v2 mod_dp = {};
        if (!entity->support) {
            if (entity->was_supported) {
                entity->was_supported = false;
                entity->dp += entity->support_dp;
                entity->support_dp = vec2(0, 0);
            }
        } else {
            entity->was_supported = true;
            entity->support_dp = entity->support_dp;
            mod_dp += entity->support->dp;
        }

        v2 delta = 0.5f*ddp*square(t*dt) + (entity->dp + mod_dp)*t*dt;
        entity->p += t*delta;

        if (iteration_count >= max_iterations) {
            assert(iteration_count == max_iterations);
            log_print(LogLevel_Warn, "Player move reached %u iterations", max_iterations);
        }
    }
}

internal void run_simulation(GameState* game_state, GameInput* input, f32 frame_dt) {
    game_state->midi_event_buffer_count = 0;
    for (PlayingMidi** playing_midi_ptr = &game_state->first_playing_midi; *playing_midi_ptr;) {
        PlayingMidi* playing_midi = *playing_midi_ptr;

        MidiTrack* track = playing_midi->track;
        if (playing_midi->event_index < track->event_count) {
            u32 ticks_for_frame = round_f32_to_u32((cast(f32) track->ticks_per_second)*frame_dt);

            // @Note: I moved the sync to output_playing_sounds, which means that if the midi track has a sync sound
            // playing_midi->tick_timer will be synced to it at the end of every frame, meaning this addition here
            // will be overwritten. It's here to account for midi events that would happen during the frame, and to
            // advance the tick timer if the midi track has no sync sound - 16/01/2020
            playing_midi->tick_timer += ticks_for_frame;
            // P.S: This is the kind of comment I don't want to have, because it is extremely vulnerable to going out
            // of date, but I thought it was relevant to note because it's not clear from this code here that this
            // behaviour is happening.

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
                if (game_state->mid_camera_transition) {
                    break;
                }

                if (!game_state->camera_target) {
                    game_state->camera_target = entity;
                }

                GameController* controller = &input->controller;

                f32 move_speed = game_config.movement_speed;

                if (controller->move_left.is_down) {
                    entity->ddp.x -= move_speed;
                }

                if (controller->move_right.is_down) {
                    entity->ddp.x += move_speed;
                }

                if (!controller->move_left.is_down && !controller->move_right.is_down) {
                    entity->ddp.x = entity->dp.x > 0.0f ? -move_speed : move_speed;
                }

                if (was_pressed(controller->jump)) {
                    entity->early_jump_timer = game_config.early_jump_window;
                }

                b32 do_jump = false;
                if (entity->support) {
                    if (entity->early_jump_timer > 0.0f) {
                        do_jump = true;
                    }

                    entity->was_supported = true;
                    entity->support_dp = entity->support_dp;
                } else {
                    if (entity->was_supported) {
                        entity->late_jump_timer = game_config.late_jump_window;

                        entity->dp += entity->support_dp;
                        entity->support_dp = vec2(0, 0);

                        entity->was_supported = false;
                    }

                    if (entity->late_jump_timer > 0.0f) {
                        do_jump = true;
                    }
                }

                if (do_jump) {
                    entity->early_jump_timer = 0.0f;
                    entity->late_jump_timer = 0.0f;

                    entity->ddp.y += game_config.jump_force;
                    entity->support = 0;
                }

                if (entity->early_jump_timer > 0.0f) entity->early_jump_timer -= frame_dt;
                if (entity->late_jump_timer  > 0.0f) entity->late_jump_timer  -= frame_dt;

                entity->gravity = game_config.gravity;

                if (!controller->jump.is_down || entity->dp.y < 0.0f) {
                    entity->gravity *= game_config.downward_gravity_multiplier;
                }

                if (entity->support) {
                    entity->gravity = 0.0f;
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
                if (!entity->playing) {
                    if (entity->soundtrack_id.value) {
                        Soundtrack* soundtrack = get_soundtrack(&game_state->assets, entity->soundtrack_id);
                        entity->playing = play_soundtrack(game_state, soundtrack, entity->playback_flags);
                        change_volume(entity->playing, 0.0f, vec2(0, 0));
                    }
                } else {
                    entity->color = COLOR_GREEN;

                    v2 rel_target_p = game_state->render_context.camera_p - entity->p;

                    v2 volume = {};
                    if (is_in_region(entity->audible_zone, rel_target_p)) {
                        volume = vec2(1, 1);
                    } else if (is_in_region(entity->audible_zone + vec2(entity->horz_fade_region, entity->vert_fade_region), rel_target_p)) {
                        // f32 right_fade = clamp01((rel_target_p.x - 0.5f*entity->audible_zone.x) / (0.5f*entity->horz_fade_region));
                        // volume = vec2(1.0f, 1.0f - right_fade);
                        f32 fade = clamp01((abs(rel_target_p.x) - 0.5f*entity->audible_zone.x) / (0.5f*entity->horz_fade_region));
                        volume = vec2(1.0f - fade, 1.0f - fade);
                    }

                    change_volume(entity->playing, 0.1f, volume);
                }
            } break;

            case EntityType_CameraZone: {
                Entity* camera_target = game_state->camera_target;
                if (camera_target && game_state->active_camera_zone != entity) {
                    if (is_in_region(entity->active_region, camera_target->p - entity->p)) {
                        game_state->previous_camera_zone = game_state->active_camera_zone;
                        game_state->active_camera_zone = entity;
                        game_state->camera_transition_t = 0.0f;
                    }
                }
            } break;

            case EntityType_Checkpoint: {
                Entity* player = game_state->player;
                if (player) {
                    if (is_in_region(entity->checkpoint_zone - player->collision, entity->p - player->p)) {
                        game_state->last_activated_checkpoint = entity;
                        entity->most_recent_player_position = player->p;
                    }
                }
            } break;

            INVALID_DEFAULT_CASE;
        }
    }

    if (!game_state->mid_camera_transition) {
        player_move(game_state, game_state->player, frame_dt);
    }

    for (u32 entity_index = 0; entity_index < game_state->entity_count; entity_index++) {
        Entity* entity = game_state->entities + entity_index;
        if (!(entity->flags & EntityFlag_Physical)) {
            entity->p += entity->dp*frame_dt;
        }

        switch (entity->type) {
            case EntityType_Player: {
                if (entity->support) {
                    Entity* support = entity->support;
                    v2 test_region = entity->collision + support->collision;
                    v2 rel_p = entity->p - support->p;
                    if (rel_p.x < -test_region.x || rel_p.x > test_region.x) {
                        entity->support = 0;
                    }
                }
            } break;
        }
    }
}

//
//
//

#if 0
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
