struct PhysicsMoveResult {
    f32 simulated_dt;
    v2 p;
    v2 dp;
    Entity* colliding_entity;
};

inline b32 physics_move(GameState* game_state, Entity* entity, f32 dt, PhysicsMoveResult* result) {
    /*
     * @TODO: Entities shouldn't be modified in here at all, that should be deferred,
     * all the changes applied to entities should be stored in PhysicsMoveResult instead.
     *
     * @TODO: Figure out some of the cases in which the collision handling will
     * fail, which I'm sure there are some, such as extreme velocities
     * (which might happen in cases of frame drops if I choose to support
     * dynamic timesteps). One suspicion I have is that if an entity penetrates
     * deep enough into another, the collision might resolve to put it on the other
     * side, allowing very fast entities to clip through each other.
     */

    b32 did_collide = false;

    zero_struct(*result);
    result->simulated_dt = dt;

    v2 ddp = entity->ddp;

    f32 epsilon = 0.0f; // 1.0e-3f;

    f32 gravity = 9.8f;
    ddp.y -= gravity;

    entity->support = 0;

    if (on_ground(entity)) {
        ddp.x -= entity->friction_of_last_touched_surface*entity->dp.x;
        entity->flags &= ~EntityFlag_OnGround;
    }

    v2 delta = 0.5f*ddp*square(dt) + entity->dp*dt;
    result->dp = entity->dp + ddp*dt;

    entity->friction_of_last_touched_surface = 0.0f;

    if (entity->flags & EntityFlag_Collides) {
        // @TODO: Optimized spatial indexing of sorts?
        for (u32 test_entity_index = 0; test_entity_index < game_state->entity_count; test_entity_index++) {
            Entity* test_entity = game_state->entities + test_entity_index;
            if (entity != test_entity && (test_entity->flags & EntityFlag_Collides) && !(test_entity->flags & EntityFlag_Physical)) {
                v2 test_delta = 0.5f*test_entity->ddp*square(dt) + test_entity->dp*dt;

                test_entity->sticking_entity = 0;

                // @Note: Relativise the movement of the two objects to just one
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
            }
        }
    }

    result->p = entity->p + delta;

    return did_collide;
}

internal void simulate_entity(GameState* game_state, Entity* entity, PhysicsMoveResult* move, f32 dt) {
    if (entity->flags & EntityFlag_Physical) {
        assert(move);
        entity->p = move->p;
        entity->dp = move->dp;
    } else {
        entity->p += entity->dp*dt;
    }
}

internal f32 simulate_timestep(GameState* game_state, f32 dt) {
    TemporaryMemory temp = begin_temporary_memory(&game_state->transient_arena);

    PhysicsMoveResult* moves = push_array(&game_state->transient_arena, game_state->entity_count, PhysicsMoveResult, no_clear());

    f32 earliest_dt = dt;
    Entity* earliest_colliding_entity = 0;
    PhysicsMoveResult* earliest_move = 0;
    for (u32 entity_index = 1; entity_index < game_state->entity_count; entity_index++) {
        Entity* entity = game_state->entities + entity_index;
        if (entity->flags & EntityFlag_Physical) {
            PhysicsMoveResult* move = moves + entity_index;
            if (physics_move(game_state, entity, dt, move)) {
                if (move->simulated_dt < earliest_dt) {
                    earliest_dt = move->simulated_dt;
                    earliest_move = move;
                    earliest_colliding_entity = entity;
                }
            }
        }
    }

    for (u32 entity_index = 1; entity_index < game_state->entity_count; entity_index++) {
        Entity* entity = game_state->entities + entity_index;
        PhysicsMoveResult* move = moves + entity_index;
        simulate_entity(game_state, entity, move, earliest_dt);
    }

    end_temporary_memory(temp);

    return earliest_dt;
}

internal void run_simulation(GameState* game_state, GameInput* input, f32 frame_dt) {
    game_state->midi_event_buffer_count = 0;
    for (PlayingMidi** playing_midi_ptr = &game_state->first_playing_midi; *playing_midi_ptr;) {
        PlayingMidi* playing_midi = *playing_midi_ptr;

        MidiTrack* track = playing_midi->track;
        if (playing_midi->event_index < track->event_count) {
            if (playing_midi->sync_sound) {
                assert(track->ticks_per_second == 48000);
                playing_midi->tick_timer = playing_midi->sync_sound->samples_played;
            }

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

    Entity* camera_target = game_state->camera_target;
    if (camera_target) {
        game_state->render_group.camera_p = camera_target->p;
    }

    execute_entity_logic(game_state, input, frame_dt);

    f32 frame_dt_left = frame_dt;
    while (frame_dt_left > 0.0f) {
        f32 dt = simulate_timestep(game_state, frame_dt_left);
        frame_dt_left -= dt;
    }
}
