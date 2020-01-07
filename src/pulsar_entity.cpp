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

        entity->was_on_ground = on_ground(entity);

        switch (entity->type) {
            case EntityType_Player: {
                if (!game_state->camera_target) {
                    game_state->camera_target = entity;
                }

                GameController* controller = &input->controller;
                f32 move_speed = entity->was_on_ground ? 50.0f : 10.0f;
                if (controller->move_left.is_down) {
                    entity->ddp.x -= move_speed;
                }
                if (controller->move_right.is_down) {
                    entity->ddp.x += move_speed;
                }

                if (entity->support) {
                    entity->ddp += entity->support_normal*dot(entity->ddp, entity->support_normal);
                }

                if (entity->was_on_ground && was_pressed(controller->move_up)) {
                    play_sound(&game_state->audio_mixer, game_state->test_sound);
                    entity->ddp.y += 400.0f;
                    entity->flags &= ~EntityFlag_OnGround;
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
                    if (is_in_aab(offset(entity->camera_zone, entity->p), camera_target->p)) {
                        render_context->camera_p = get_center(offset(entity->camera_zone, entity->p));
                        render_context->camera_rotation_arm = entity->camera_rotation_arm;
                        render_worldspace(render_context, get_dim(entity->camera_zone).y);
                    }
                }
            } break;

            case EntityType_Checkpoint: {
                AxisAlignedBox2 checkpoint_box = offset(entity->checkpoint_zone, entity->p);
                AxisAlignedBox2 player_box = offset(game_state->player->collision.bounding_box, game_state->player->p);
                if (aab_contained_in_aab(checkpoint_box, player_box)) {
                    game_state->last_activated_checkpoint = entity;
                    entity->most_recent_player_position = game_state->player->p;
                }
            } break;

            INVALID_DEFAULT_CASE;
        }
    }
}
