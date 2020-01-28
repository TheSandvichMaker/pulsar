/* This file contains functions related to simulating entities.
 *
 * It is structured around four phases: Midi handling, pre-player simulation, player simulation, post-player simulation
 *
 * To start, all playing midi tracks put midi events that are played this frame into the midi event buffer,
 * so entities can subsequently read from this buffer to make decisions based on midi events.
 *
 * Then, in the pre-player simulation phase every entity (including the player) decides on their course of action
 * for the current frame, so that the player can then be simulated once all those decisions have been made.
 * This is important for things like colliding against platforms that have decided to move this frame.
 *
 * The player simulation phase goes through a fairly messy mostly framerate independent loop that's also
 * got a three phase structure:
 *  Decide the player's delta position for the given timestep based on factors like player velocity,
 *   acceleration, whether the player's supported by an entity, and whether the player has added
 *   velocity from external factors like when they jump off a moving platform.
 *  Then there's a collision loop which goes through and checks the player's move against all the EntityType_Wall
 *   entities in the level, checking whether the player is stuck inside one (and getting them unstuck), or otherwise
 *   checking for the nearest collision.
 *  Finally, if there's a collision the collision is handled and the player's
 *   position, velocity, and acceleration is updated to reflect this, and the frame time left to simulate is updated
 *   based on when the collision happened. If there was a collision or unstuck, the loop continues so long as the
 *   player can still move (so hasn't been killed, for example) and the max iteration count hasn't been reached.
 *
 * In the post-player simulation phase, currently only non-physical (anything that's not the player for now) entities that
 * have velocity will get moved. This is so that all positions are up to date for the next time we enter the simulation
 * again.
 */

inline b32 player_can_move(GameState* game_state) {
    Entity* player = game_state->player;

    if (player->dead || player->killed_this_frame) return false;
    if (game_state->mid_camera_transition)         return false;
    if (game_state->level_intro_timer > 0.0f)      return false;

    return true;
}

struct TraceInfo {
    f32 t_min = 1.0f;
    f32 t_max = 0.0f;
    v2 hit_normal = vec2(0.0f, 0.0f);
};

inline b32 line_trace(f32 wall_x, v2 wall_bounds, v2 wall_normal, v2 start_p, v2 delta_p, TraceInfo* info, f32 t_epsilon) {
    b32 result = false;
    f32 t_result = (wall_x - start_p.x) / delta_p.x;
    if (t_result >= 0.0f) {
        f32 y = start_p.y + t_result*delta_p.y;
        if (y >= wall_bounds.e[0] && y < wall_bounds.e[1]) {
            if (t_result < info->t_min) {
                result = true;
                info->t_min = max(0.0f, t_result - t_epsilon);
                info->hit_normal = wall_normal;
            }
            if (t_result <= 1.0f && t_result > info->t_max) {
                info->t_max = t_result; // @TODO: Think about whether this also needs an epsilon - or whether the epsilons can be nuked
            }
        }
    }
    return result;
}

inline b32 aab_trace(v2 start_p, v2 end_p, AxisAlignedBox2 aab, TraceInfo* info) {
    b32 result = false;

    *info = {};

    f32 t_epsilon = 1.0e-3f;

    v2 delta_p = end_p - start_p;

    result |= line_trace(aab.min.x, vec2(aab.min.y, aab.max.y), vec2(-1,  0), vec2(start_p.x, start_p.y), vec2(delta_p.x, delta_p.y), info, t_epsilon);
    result |= line_trace(aab.max.x, vec2(aab.min.y, aab.max.y), vec2( 1,  0), vec2(start_p.x, start_p.y), vec2(delta_p.x, delta_p.y), info, t_epsilon);
    result |= line_trace(aab.min.y, vec2(aab.min.x, aab.max.x), vec2( 0, -1), vec2(start_p.y, start_p.x), vec2(delta_p.y, delta_p.x), info, t_epsilon);
    result |= line_trace(aab.max.y, vec2(aab.min.x, aab.max.x), vec2( 0,  1), vec2(start_p.y, start_p.x), vec2(delta_p.y, delta_p.x), info, t_epsilon);

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
    Entity* player = game_state->player;
    if (player && !player->dead) {
        player->killed_this_frame = true;
        game_state->player_respawn_timer = 1.0f;
    }
}

inline void process_collision_logic(GameState* game_state, Entity* collider, Entity* collidee) {
    if (collider->type == EntityType_Player) {
        if (collidee->flags & EntityFlag_Hazard) {
            kill_player(game_state);
        }
    }
}

inline v2 accel_towards(v2 min_ddp, v2 max_ddp, v2 cur_dp, v2 tar_dp, f32 dt) {
    v2 dv = tar_dp - cur_dp;
    v2 result = clamp(dv / dt, min_ddp, max_ddp);
    return result;
}

internal void simulate_entities(GameState* game_state, GameInput* input, f32 frame_dt) {
    game_state->midi_event_buffer_count = 0;
    if (!game_state->midi_paused) {
        for (PlayingMidi** playing_midi_ptr = &game_state->first_playing_midi; *playing_midi_ptr;) {
            PlayingMidi* playing_midi = *playing_midi_ptr;

            MidiTrack* track = playing_midi->track;
            if (playing_midi->event_index < track->event_count) {
                f32 ticks_for_frame = cast(f32) track->ticks_per_second*frame_dt;

                // @Note: I moved the sync to output_playing_sounds, which means that if the midi track has a sync sound
                // playing_midi->tick_timer will be synced to it at the end of every frame, meaning this addition here
                // will be overwritten. It's here to account for midi events that would happen during the frame, and to
                // advance the tick timer if the midi track has no sync sound - 16/01/2020
                u32 tick_timer_for_frame = playing_midi->tick_timer + cast(u32) (game_config->simulation_rate*(ticks_for_frame*playing_midi->playback_rate));
                // P.S: This is the kind of comment I don't want to have, because it is extremely vulnerable to going out
                // of date, but I thought it was relevant to note because it's not clear from this code here that this
                // behaviour is happening.

                MidiEvent event = track->events[playing_midi->event_index];
                while (event.absolute_time_in_ticks <= tick_timer_for_frame && playing_midi->event_index < track->event_count) {
                    playing_midi->event_index++;

                    b32 should_use_event = true;
                    if (track->flags & MidiFlag_IgnoreExtremes) {
                        if (playing_midi->event_index == 1 || playing_midi->event_index == track->event_count) {
                            should_use_event = false;
                        }
                    }

                    if (playing_midi->source_soundtrack_player && !playing_midi->source_soundtrack_player->can_be_heard_by_player) {
                        should_use_event = false;
                    }

                    if (should_use_event) {
                        ActiveMidiEvent* active_event = game_state->midi_event_buffer + game_state->midi_event_buffer_count++;
                        active_event->source_soundtrack = playing_midi->source_soundtrack_player->soundtrack_id;
                        active_event->midi_event = event;
                        f32 timing_into_frame = cast(f32) (tick_timer_for_frame - event.absolute_time_in_ticks) / ticks_for_frame;
                        active_event->dt_left = frame_dt*(1.0f - timing_into_frame);

                        assert(game_state->midi_event_buffer_count < ARRAY_COUNT(game_state->midi_event_buffer));

#if 0
                        char* midi_note_name = midi_note_names[event.note_value % 12];
                        s32 midi_note_octave = (cast(s32) event.note_value / 12) - 2;

                        log_print(LogLevel_Info, "Pushed %s %s %d midi event with timing: %u (tick_timer: %u, event_index: %u)", event.type == MidiEvent_NoteOn ? "note on" : "note off", midi_note_name, midi_note_octave, event.absolute_time_in_ticks, playing_midi->tick_timer, playing_midi->event_index - 1);
#endif
                    }

                    event = track->events[playing_midi->event_index];
                }
            }

            b32 midi_done = false;
            if (playing_midi->sync_sound) {
                if (playing_midi->tick_timer >= playing_midi->sync_sound->sound->sample_count) {
                    if (playing_midi->flags & Playback_Looping) {
                        playing_midi->event_index = 0;
                        playing_midi->tick_timer -= playing_midi->sync_sound->sound->sample_count;
                    } else {
                        midi_done = true;
                    }
                }
            } else {
                if (playing_midi->event_index >= track->event_count) {
                    assert(playing_midi->event_index == track->event_count);
                    if (playing_midi->flags & Playback_Looping) {
                        playing_midi->event_index = 0;
                        playing_midi->tick_timer = 0;
                        playing_midi_ptr = &playing_midi->next;
                    } else {
                        midi_done = true;
                    }
                }
            }

            if (midi_done) {
                *playing_midi_ptr = playing_midi->next;
                playing_midi->next = game_state->first_free_playing_midi;
                game_state->first_free_playing_midi = playing_midi;
            } else {
                playing_midi_ptr = &playing_midi->next;
            }
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
                if (!player_can_move(game_state)) {
                    break;
                }

                GameController* controller = &input->controller;

                f32 move_speed = game_config->movement_speed;
                v2 target_dp = entity->dp;

                if (controller->move_left.is_down) {
                    target_dp.x -= move_speed;
                }

                if (controller->move_right.is_down) {
                    target_dp.x += move_speed;
                }

                if (!controller->move_left.is_down && !controller->move_right.is_down) {
                    target_dp.x = 0.0f;
                }

                entity->ddp = accel_towards(vec2(-move_speed, 0.0f), vec2(move_speed, 0.0f), entity->dp, target_dp, frame_dt);

                if (was_pressed(controller->jump)) {
                    entity->early_jump_timer = game_config->early_jump_window;
                }

                b32 do_jump = false;
                if (entity->support) {
                    if (entity->early_jump_timer > 0.0f) {
                        do_jump = true;
                    }
                } else {
                    if (entity->was_supported) {
                        entity->late_jump_timer = game_config->late_jump_window;
                    }

                    if (was_pressed(controller->jump) && entity->late_jump_timer > 0.0f) {
                        do_jump = true;
                    }
                }

                if (do_jump) {
                    entity->early_jump_timer = 0.0f;
                    entity->late_jump_timer = 0.0f;

                    // @Note: I'm dividing the jump force by the frame dt because the jump is instantaneous.
                    // For regular movement, normally if you doubled your fps the amount of acceleration per
                    // frame would be halved, but the number of frames would double, so the net result would
                    // be the same. But because the jump isn't spread out across frames, we need to divide
                    // out the frame dt to make it frame-rate independent.
                    entity->ddp.y += game_config->jump_force / frame_dt;
                    entity->support = 0;
                    entity->moon_time = 1.0f;
                }

                if (entity->early_jump_timer > 0.0f) entity->early_jump_timer -= frame_dt;
                if (entity->late_jump_timer  > 0.0f) entity->late_jump_timer  -= frame_dt;

                entity->gravity = game_config->gravity;

                if (!controller->jump.is_down || entity->dp.y < 0.0f || entity->moon_time <= 0.0f) {
                    entity->gravity *= game_config->downward_gravity_multiplier;
                }

                if (entity->moon_time > 0.0f) {
                    entity->moon_time -= frame_dt / game_config->moon_time_duration;
                    if (entity->moon_time < 0.0f) {
                        entity->moon_time = 0.0f;
                    }
                }

                if (entity->walk_cycle > game_config->player_walk_cycle_length) {
                    entity->walk_cycle -= game_config->player_walk_cycle_length;
                    // play_sound(&game_state->game_audio, game_state->player_footstep_sound);
                }
            } break;

            case EntityType_Wall: {
                if (entity->behaviour == WallBehaviour_Move) {
                    for (u32 event_index = 0; event_index < game_state->midi_event_buffer_count; event_index++) {
                        ActiveMidiEvent event = game_state->midi_event_buffer[event_index];
                        if (!entity->listening_to.value || event.source_soundtrack.value == entity->listening_to.value) {
                            if (event.note_value == entity->midi_note) {
                                if (event.type == MidiEvent_NoteOn) {
                                    entity->moving_to_end = true;
                                } else if (event.type == MidiEvent_NoteOff) {
                                    entity->moving_to_end = false;
                                }
                            }
                        }
                    }

                    f32 t = smootherstep(entity->movement_t);

                    v2 target = entity->start_p + t*entity->end_p;
                    entity->dp = (target - entity->p) / frame_dt;

                    if (entity->moving_to_end) {
                        if (entity->movement_t < 1.0f) {
                            entity->movement_t += frame_dt / (0.001f*entity->movement_speed_ms);
                        }
                        if (entity->movement_t > 1.0f) {
                            entity->movement_t = 1.0f;
                        }
                    } else {
                        if (entity->movement_t > 0.0f) {
                            entity->movement_t -= frame_dt / (0.001f*entity->movement_speed_ms);
                        }
                        if (entity->movement_t < 0.0f) {
                            entity->movement_t = 0.0f;
                        }
                    }
                } else if (entity->behaviour == WallBehaviour_Toggle) {
                    for (u32 event_index = 0; event_index < game_state->midi_event_buffer_count; event_index++) {
                        ActiveMidiEvent event = game_state->midi_event_buffer[event_index];
                        if (!entity->listening_to.value || event.source_soundtrack.value == entity->listening_to.value) {
                            if (event.note_value == entity->midi_note) {
                                if (event.type == MidiEvent_NoteOn) {
                                    entity->flags |= EntityFlag_Collides;
                                } else if (event.type == MidiEvent_NoteOff) {
                                    entity->flags &= ~EntityFlag_Collides;
                                }
                            }
                        }
                    }
                }
            } break;

            case EntityType_SoundtrackPlayer: {
                if (!entity->playing) {
                    if (entity->soundtrack_id.value) {
                        entity->playing = play_soundtrack(game_state, entity, entity->playback_flags);
                        change_volume(entity->playing, 0.0f, vec2(0, 0));
                    }
                } else {
                    entity->color = COLOR_GREEN;

                    v2 rel_listener_p = game_state->camera_target->p - entity->p;

                    v2 volume = {};
                    if (is_in_region(entity->audible_zone, rel_listener_p)) {
                        volume = vec2(0.5f, 0.5f);
                    } else if (is_in_region(entity->audible_zone + vec2(entity->horz_fade_region, entity->vert_fade_region), rel_listener_p)) {
                        // @TODO: Respect vert_fade_region's existence
                        f32 fade = clamp01((abs(rel_listener_p.x) - 0.5f*entity->audible_zone.x) / (0.5f*entity->horz_fade_region));
                        f32 positional_fade = rel_listener_p.x < 0.0f ? fade : -fade;
                        f32 pan_angle = 0.25f*PI_32 + 0.25f*PI_32*positional_fade;
                        volume = (1.0f - fade)*0.5f*square_root(2.0f)*vec2(cos(pan_angle), sin(pan_angle));
                        // log_print(LogLevel_Info, "positional_fade: %f, pan_angle: %f, pan_angle in degrees: %f", positional_fade, pan_angle, rad_to_deg(pan_angle));
                    }

                    for (u32 event_index = 0; event_index < game_state->midi_event_buffer_count; event_index++) {
                        ActiveMidiEvent event = game_state->midi_event_buffer[event_index];
                        if (event.source_soundtrack.value == entity->soundtrack_id.value) {
                            if (event.type == MidiEvent_NoteOn) {
                                game_state->background_pulse_t += (volume.x + volume.y)*game_config->background_pulse_intensity;
                            } else if (event.type == MidiEvent_NoteOff) {
                                game_state->background_pulse_t += 0.25f*(volume.x + volume.y)*game_config->background_pulse_intensity;
                            }
                        }
                    }

                    entity->can_be_heard_by_player = volume.x > 0.0f || volume.y > 0.0f;

                    change_volume(entity->playing, 0.05f, volume);
                }
            } break;

            case EntityType_CameraZone: {
                Entity* camera_target = game_state->camera_target;
                if (camera_target && game_state->active_camera_zone != entity) {
                    if (is_in_region(entity->active_region, camera_target->p - entity->p)) {
                        ADD_DEBUG_BREAK(camera_zone_switch);
                        game_state->previous_camera_zone = game_state->active_camera_zone;
                        game_state->active_camera_zone = entity;
                        // @TODO: camera_transition_t doesn't follow the convention of timers in the rest of the codebase
                        if (!camera_target->dead) {
                            game_state->camera_transition_t = 0.0f;
                        }
                    }
                }
            } break;

            case EntityType_Checkpoint: {
                Entity* player = game_state->player;
                if (player && !player->dead) {
                    if (is_in_region(entity->checkpoint_zone - player->collision, entity->p - player->p)) {
                        game_state->last_activated_checkpoint = entity;
                        // @TODO: I'd rather do a trace here to find the ground but let's do that later
                        entity->respawn_p = vec2(player->p.x, entity->p.y) + vec2(0.0f, -0.5f*entity->checkpoint_zone.y + player->collision.y*0.5f + 1.0e-3f);
                    }
                }
            } break;

            INVALID_DEFAULT_CASE;
        }
    }

    Entity* player = game_state->player;

    if (player && player_can_move(game_state)) {
        f32 t = 1.0f;

        u32 unstuck_count = 0;
        u32 max_iterations = game_config->max_collision_iterations;
        u32 iteration_count = 0;

        f32 epsilon = 1.0e-3f;

        v2 gravity = vec2(0.0f, player->gravity);

        while (player_can_move(game_state) && iteration_count < max_iterations) {
            f32 dt = t*frame_dt;

            v2 ddp = player->ddp;

            if (!player->support) {
                ddp += gravity;
            }

            // @TODO: Figure out where this sits compared to accel_towards
            f32 max_x_vel = game_config->max_x_vel;
            f32 min_y_vel = game_config->min_y_vel;
            f32 max_y_vel = game_config->max_y_vel;

            ddp.x = safe_ratio_0((clamp(ddp.x*dt + player->dp.x, -max_x_vel, max_x_vel) - player->dp.x), dt);
            ddp.y = safe_ratio_0((clamp(ddp.y*dt + player->dp.y,  min_y_vel, max_y_vel) - player->dp.y), dt);

            v2 transient_dp = ddp*dt;

            if (player->support) {
                if (!player->was_supported) {
                    player->dp += grazing_reflect(player->ballistic_dp, player->support_normal);
                    player->ballistic_dp = vec2(0, 0);
                }
                player->was_supported = true;
                player->support_dp = player->support->dp;
                if (length_sq(player->support_dp) > length_sq(player->retained_support_dp)) {
                    player->retained_support_dp = player->support_dp;
                    player->retained_support_dp_timer = game_config->support_dp_retention_time;
                }
                transient_dp += player->support->dp;
            } else {
                if (player->was_supported) {
                    player->was_supported = false;
                    if (length_sq(player->retained_support_dp) > length_sq(player->support_dp)) {
                        player->support_dp = player->retained_support_dp;
                        player->retained_support_dp = vec2(0, 0);
                    }
                    player->ballistic_dp.x = clamp(player->ballistic_dp.x + player->support_dp.x, -game_config->max_ballistic_x_vel, game_config->max_ballistic_x_vel);
                    player->dp.y += clamp(player->support_dp.y, -game_config->max_ballistic_y_vel, game_config->max_ballistic_y_vel);
                    if (length_sq(player->support_dp) > 0.0f) {
                        log_print(LogLevel_Info, "Added { %f, %f } support_dp to player->dp", player->support_dp.x, player->support_dp.y);
                    }
                    player->support_dp = vec2(0, 0);
                }

                transient_dp += player->ballistic_dp;
            }

            if (player->retained_support_dp_timer > 0.0f) {
                player->retained_support_dp_timer -= dt;
                if (player->retained_support_dp_timer <= 0.0f) {
                    player->retained_support_dp = vec2(0, 0);
                }
            }

            transient_dp += player->contact_move;
            player->contact_move = {};

            v2 delta = 0.5f*ddp*square(dt) + (transient_dp + player->dp)*dt;

            if (player->support) {
                f32 player_horizontal_move = abs(0.5f*ddp.x*square(dt) + player->dp.x*dt);
                if (player_horizontal_move < 0.05f) {
                    // @Note: The player's walk cycle resets to being some way through so that when you get started, you don't have to walk a full cycle to trigger the first step
                    player->walk_cycle = 0.75f*game_config->player_walk_cycle_length;
                } else {
                    player->walk_cycle += player_horizontal_move;
                }
            }

            b32 did_an_unstuck = false;
            b32 did_collide = false;
            TraceInfo collision = {};
            Entity* collision_entity = 0;
            v2 collision_rel_delta = {};

            // @TODO: Spatial indexing of sorts?
            for_entity_type (game_state, EntityType_Wall, test_entity) {
                if (test_entity->flags & EntityFlag_Collides) {
                    v2 test_delta = test_entity->dp*dt;
                    v2 spent_test_delta = test_entity->dp*(1.0f - t)*frame_dt;

                    v2 sub_frame_test_entity_p = test_entity->p + spent_test_delta;

                    v2 rel_p = player->p - sub_frame_test_entity_p;
                    v2 test_region = player->collision + test_entity->collision;
                    AxisAlignedBox2 test_aab = aab_center_dim(vec2(0, 0), test_region);

                    if (is_in_aab(test_aab, rel_p)) {
                        f32 best_distance = F32_MAX;
                        v2 best_move = vec2(0, 0);

                        f32 unstuck_epsilon = (1 + unstuck_count)*epsilon; // @Hack: More stupid hacking

                        f32 test_distance = rel_p.x - test_aab.min.x;
                        if (test_distance < best_distance) {
                            best_distance = test_distance + unstuck_epsilon; // @TODO: The epsilon is to deal with the whole inclusive/exclusive bounds for the AABB, but is a hack, not good
                            best_move = vec2(-1, 0);
                        }

                        test_distance = test_aab.max.x - rel_p.x;
                        if (test_distance < best_distance) {
                            best_distance = test_distance;
                            best_move = vec2(1, 0);
                        }

                        test_distance = rel_p.y - test_aab.min.y;
                        if (test_distance < best_distance) {
                            best_distance = test_distance + unstuck_epsilon; // @TODO: The epsilon is to deal with the whole inclusive/exclusive bounds for the AABB, but is a hack, not good
                            best_move = vec2(0, -1);
                        }

                        test_distance = test_aab.max.y - rel_p.y;
                        if (test_distance < best_distance) {
                            best_distance = test_distance;
                            best_move = vec2(0, 1);
                        }

                        log_print(LogLevel_Warn, "Player got stuck in an entity. Best move: { %g, %g }", best_move.x*best_distance, best_move.y*best_distance);

                        delta = best_move*best_distance;
                        process_collision_logic(game_state, player, test_entity);

                        did_an_unstuck = true;
                        unstuck_count++;

                        break; // breaks out of the for, not the while
                    } else if (test_entity != player->support) {
                        v2 rel_delta = delta - test_delta;

                        TraceInfo info = {};
                        if (aab_trace(rel_p, rel_p + rel_delta, test_aab, &info)) {
                            // @Note: If you collided with a face that faces away from the relative delta, then that's probably a precision problem.
                            if (dot(rel_delta, info.hit_normal) <= 0.0f) {
                                if (info.t_min < collision.t_min) {
                                    did_collide = true;
                                    collision = info;
                                    collision_entity = test_entity;
                                    collision_rel_delta = rel_delta;
                                }
                            }
                        }
                    }
                }
            }

            f32 t_spent = 1.0f;

            if (did_collide) {
                // log_print(LogLevel_Info, "collision.t_min = %f, .t_max = %f", collision.t_min, collision.t_max);
                t *= 1.0f - collision.t_min;

                player->dp += ddp*dt*collision.t_min;

                delta *= collision.t_min;

                if (t == 0.0f) {
                    delta += collision.hit_normal*epsilon; // @TODO: More epsilonism. I don't like it.
                }

                player->ddp = grazing_reflect(player->ddp, collision.hit_normal);
                player->dp  = grazing_reflect(player->dp, collision.hit_normal);

                if (dot(collision.hit_normal, gravity) < 0) {
                    // if (dot(delta, gravity) >= 0.0f) {
                        player->support = collision_entity;
                        player->support_normal = collision.hit_normal;
                    // } else {
                    //     // @Hack: This tricks the support velocity ordeal up above into letting you get smacked around from below if you've got upward speed (it doesn't work properly btw)
                    //     player->was_supported = true;
                    //     player->support = 0;
                    //     player->support_normal = collision.hit_normal;
                    //     player->support_dp = collision_entity->dp;
                    // }
                } else {
                    // Shitty @Hack: Not accounting for penetration vector or relative motion or basically anything because my brain is fried
                    // @TODO: Unfry brain, un-shitty hack collision handling
                    v2 bad_not_good_thing = collision.hit_normal*dot(collision_entity->dp, collision.hit_normal);
                    player->contact_move = bad_not_good_thing;
                }

                process_collision_logic(game_state, player, collision_entity);
            } else {
                player->dp += ddp*dt;
                t = 0.0f;
            }

            player->p += delta;

            Entity* support = player->support;
            if (support) {
                if (support->flags & EntityFlag_Collides) {
                    v2 adjusted_support_p = player->support->p + player->support->dp*dt;

                    v2 test_region = player->collision + support->collision;
                    v2 rel_p = player->p - adjusted_support_p;
                    if (rel_p.x < -0.5f*test_region.x || rel_p.x > 0.5f*test_region.x) {
                        player->support = 0;
                    }
                } else {
                    player->support = 0;
                }
            }

            if (!did_collide && !did_an_unstuck) {
                break;
            }

            iteration_count++;
        }

        if (iteration_count >= max_iterations) {
            assert(iteration_count == max_iterations);
            log_print(LogLevel_Warn, "Player move reached %u iterations", max_iterations);
            if (unstuck_count >= max_iterations) {
                kill_player(game_state);
                log_print(LogLevel_Warn, "Player died by crushing");
            }
        }
    }

    for (u32 entity_index = 0; entity_index < game_state->entity_count; entity_index++) {
        Entity* entity = game_state->entities + entity_index;
        if (!(entity->flags & EntityFlag_Physical)) {
            entity->p += entity->dp*frame_dt;
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
