internal CONSOLE_COMMAND(cc_load_level) {
    String level = arguments;
    load_level(game_state, level);
}

internal CONSOLE_COMMAND(cc_save_level) {
    String level_name = arguments;

    Level* level = game_state->active_level;

    b32 write_level = true;
    if (level_name.len > 0) {
        if (level_name.len > 255) {
            log_print(LogLevel_Error, "Level name is above 255 characters");
            write_level = false;
        } else {
            level->name_length = cast(u32) level_name.len;
            copy(level->name_length, level_name.data, level->name);
        }
    }

    if (write_level) {
        write_level_to_disk(game_state, level, wrap_string(level->name_length, level->name));
    }
}

internal CONSOLE_COMMAND(cc_repair_entity_guids) {
    Level* level = game_state->active_level;

    b32 had_to_do_something = false;
    b32 first_available_guid_was_adjusted = false;

    for (u32 outer_entity_index = 1; outer_entity_index < level->entity_count; outer_entity_index++) {
        Entity* outer = level->entities + outer_entity_index;

        if (outer->guid.value >= level->first_available_guid) {
            had_to_do_something = true;
            first_available_guid_was_adjusted = true;
            level->first_available_guid = outer->guid.value + 1;
        }

        for (u32 inner_entity_index = 1; inner_entity_index < level->entity_count; inner_entity_index++) {
            Entity* inner = level->entities + inner_entity_index;

            if (outer_entity_index != inner_entity_index) {
                if (inner->guid.value == outer->guid.value) {
                    had_to_do_something = true;
                    log_print(LogLevel_Warn, "Duplicate Entity ID { %u } repaired (indices %u vs %u)", inner->guid.value, outer_entity_index, inner_entity_index);
                    inner->guid.value = level->first_available_guid++;

                    EntityHash* outer_hash = get_entity_hash_slot(editor, outer->guid);
                    EntityHash* inner_hash = get_entity_hash_slot(editor, inner->guid);

                    outer_hash->guid = outer->guid;
                    outer_hash->index = outer_entity_index;

                    inner_hash->guid = inner->guid;
                    inner_hash->index = inner_entity_index;
                }
            }
        }
    }

    if (first_available_guid_was_adjusted) {
        log_print(LogLevel_Warn, "Repaired the level's first available guid", level->first_available_guid);
    }

    if (!had_to_do_something) {
        log_print(LogLevel_Info, "All guids? All good.");
    }
}

internal CONSOLE_COMMAND(cc_find_entity) {
    s64 target_id;
    if (parse_s64(&arguments, &target_id)) {
        EntityID entity_id = { cast(u32) target_id };
        Entity* entity = get_entity_from_guid(editor, entity_id);
        if (entity) {
            editor->selected_entity_count = 1;
            editor->selected_entities[0] = entity_id;
            game_state->render_context.camera_p = entity->p;
        } else {
            log_print(LogLevel_Error, "Could not find an entity with the guid %u", target_id);
        }
    } else {
        log_print(LogLevel_Error, "Could not interpret argument '%.*s' as a valid number", string_expand(arguments));
    }
}

internal CONSOLE_COMMAND(cc_set_soundtrack) {
    if (editor->selected_entity_count) {
        begin_undo_batch(editor);
        for (u32 selected_index = 0; selected_index < editor->selected_entity_count; selected_index++) {
            Entity* entity = get_entity_from_guid(editor, editor->selected_entities[selected_index]);
            if (entity) {
                SoundtrackID soundtrack_id = get_soundtrack_id_by_name(editor->assets, arguments);
                if (soundtrack_id.value) {
                    switch (entity->type) {
                        case EntityType_SoundtrackPlayer: {
                            add_entity_data_undo_history(editor, wrap_entity_data(entity, &entity->soundtrack_id), "Set Soundtrack");
                            entity->soundtrack_id = soundtrack_id;
                        } break;
                        case EntityType_Wall: {
                            add_entity_data_undo_history(editor, wrap_entity_data(entity, &entity->listening_to), "Set Soundtrack");
                            entity->listening_to = soundtrack_id;
                        } break;
                        default: { log_print(LogLevel_Error, "Selected entity of type %s does not have any set_soundtrack behaviour", enum_name(EntityType, entity->type)); } break;
                    }
                } else {
                    log_print(LogLevel_Error, "Could not find soundtrack '%.*s'", string_expand(arguments));
                }
            }
        }
        end_undo_batch(editor);
    } else {
        log_print(LogLevel_Error, "No entity selected");
    }
}

internal CONSOLE_COMMAND(cc_toggle_flag) {
    String flag_name = advance_word(&arguments);

    if (editor->selected_entity_count) {
        u32 flag = 0;

        {
            u32 dummy_flags = 0xFFFFFFFF;
            u32 dummy_flags_compare = 0xFFFFFFFF;
            char* test_name = 0;
            while (enum_flag_name(EntityFlag, &dummy_flags, &test_name)) {
                if (strings_are_equal(flag_name, wrap_cstr(test_name))) {
                    // @Note: enum_flag_name takes out the flag we just got the name for from dummy_flags, so by comparing it with its previous value we can find out
                    // the value of the flag we're talking about.
                    flag = dummy_flags_compare & ~dummy_flags;
                    break;
                }
                dummy_flags_compare = dummy_flags;
            }
        }

        if (flag) {
            begin_undo_batch(editor);
            for (u32 selected_index = 0; selected_index < editor->selected_entity_count; selected_index++) {
                Entity* entity = get_entity_from_guid(editor, editor->selected_entities[selected_index]);
                if (entity) {
                    add_entity_data_undo_history(editor, wrap_entity_data(entity, &entity->flags), "Toggle Flag");
                    if (entity->flags & flag) {
                        entity->flags &= ~flag;
                    } else {
                        entity->flags |= flag;
                    }
                }
            }
            end_undo_batch(editor);
        } else {
            log_print(LogLevel_Error, "Unknown flag %.*s", string_expand(flag_name));
        }
    } else {
        log_print(LogLevel_Error, "No entity selected");
    }
}

internal CONSOLE_COMMAND(cc_switch_game_mode) {
    for (u32 mode_index = 0; mode_index < GameMode_Count; mode_index++) {
        GameMode mode = cast(GameMode) mode_index;
        String mode_name = wrap_cstr(enum_name(GameMode, mode));

        if (mode_name.len &&
            (strings_are_equal(arguments, mode_name, StringMatch_CaseInsenitive) ||
             strings_are_equal(arguments, tail(mode_name, sizeof("GameMode_") - 1), StringMatch_CaseInsenitive))
        ) {
            switch_game_mode(game_state, cast(GameMode) mode_index);
        }
    }
}

internal CONSOLE_COMMAND(cc_set) {
    b32 no_errors = parse_config(game_config, arguments);
    if (no_errors) {
        String key = advance_word(&arguments);
        String value = trim_spaces(advance_to(&arguments, '#'));
        log_print(LogLevel_Info, "Set '%.*s' to '%.*s'", string_expand(key), string_expand(value));
    }
}

internal CONSOLE_COMMAND(cc_dump_config) {
    log_print(LogLevel_Info, "Game Config:");
    for (u32 member_index = 0; member_index < members_count(GameConfig); member_index++) {
        MemberDefinition* member = members_of(GameConfig) + member_index;
        void** member_ptr = member_ptr(game_config, member);

        String member_name = wrap_string(member->name_length, member->name);

        switch (member->type) {
            case meta_type(b32): { log_print(LogLevel_Info, "    %-32.*s %s", string_expand(member_name), *(cast(b32*) member_ptr) ? "true" : "false"); } break;

            case meta_type(u8):  {
                u8 val = *(cast(u8*) member_ptr);
                if (is_printable_ascii(val)) {
                    log_print(LogLevel_Info, "    %-32.*s 0x%X (%c)", string_expand(member_name), val, val);
                } else {
                    log_print(LogLevel_Info, "    %-32.*s 0x%X", string_expand(member_name), val);
                }
            } break;
            case meta_type(u16): { log_print(LogLevel_Info, "    %-32.*s %u",    string_expand(member_name), *(cast(u16*) member_ptr)); } break;
            case meta_type(u32): { log_print(LogLevel_Info, "    %-32.*s %u",    string_expand(member_name), *(cast(u32*) member_ptr)); } break;
            case meta_type(u64): { log_print(LogLevel_Info, "    %-32.*s %I64u", string_expand(member_name), *(cast(u64*) member_ptr)); } break;

            case meta_type(s8):  { log_print(LogLevel_Info, "    %-32.*s %d",    string_expand(member_name), *(cast(s8*) member_ptr)); } break;
            case meta_type(s16): { log_print(LogLevel_Info, "    %-32.*s %d",    string_expand(member_name), *(cast(s16*) member_ptr)); } break;
            case meta_type(s32): { log_print(LogLevel_Info, "    %-32.*s %d",    string_expand(member_name), *(cast(s32*) member_ptr)); } break;
            case meta_type(s64): { log_print(LogLevel_Info, "    %-32.*s %I64d", string_expand(member_name), *(cast(s64*) member_ptr)); } break;

            case meta_type(f32): { log_print(LogLevel_Info, "    %-32.*s %g", string_expand(member_name), *(cast(f32*) member_ptr)); } break;
            case meta_type(f64): { log_print(LogLevel_Info, "    %-32.*s %g", string_expand(member_name), *(cast(f64*) member_ptr)); } break;

            case meta_type(String): {
                String val = *(cast(String*) member_ptr);
                log_print(LogLevel_Info, "    %-32.*s %.*s", string_expand(member_name), string_expand(val));
            } break;
        }
    }
}

internal CONSOLE_COMMAND(cc_kill_player) {
    kill_player(game_state);
}

internal CONSOLE_COMMAND(cc_delete_entity) {
    u64 guid;
    if (parse_u64(&arguments, &guid)) {
        delete_entity(editor, { cast(u32) guid });
    }
}

internal CONSOLE_COMMAND(cc_quit) {
    input->quit_requested = true;
}

#if PULSAR_DEBUG
internal CONSOLE_COMMAND(cc_break_at) {
    String name = advance_word(&arguments);
    b32 found_break = false;
    for (u32 index = 0; index < debug_break_array_count; index++) {
        DebugBreakMarker* marker = debug_break_array + index;
        if (strings_are_equal(name, marker->name)) {
            log_print(LogLevel_Info, "Enabled breakpoint '%.*s'", string_expand(name));
            marker->enabled = true;
            found_break = true;
            break;
        }
    }

    if (!found_break) {
        log_print(LogLevel_Error, "Could not find debug break '%.*s'", string_expand(name));
    }
}
#endif

internal CONSOLE_COMMAND(cc_help) {
    // Dummy...
}

#define console_command(name, help) { cc_##name, string_literal(#name), string_literal(help) }
global ConsoleCommand console_commands[] = {
    console_command(help, "Print out this list of commands."),
    console_command(load_level, "Load a level."),
    console_command(save_level, "Save a level."),
    console_command(repair_entity_guids, "Repair entity guids in the loaded level."),
    console_command(find_entity, "Find an entity with a given GUID."),
    console_command(set_soundtrack, "Set the soundtrack of the selected entity."),
    console_command(toggle_flag, "Toggle a flag on the selected entity."),
    console_command(switch_game_mode, "Switch the gamemode."),
    console_command(set, "Set a config variable."),
    console_command(dump_config, "Dump the current config state."),
    console_command(kill_player, "Kill player."),
    console_command(delete_entity, "Delete an entity with a given GUID."),
    console_command(quit, "Quit the game."),

#if PULSAR_DEBUG
    console_command(break_at, "Enable a particular breakpoint."),
#endif
};

inline void execute_console_command(GameState* game_state, GameInput* input, String in_buffer) {
    if (in_buffer.data[0] != '/') {
        String command = advance_word(&in_buffer);
        if (command.len > 0) {
            b32 print_help = false;

            b32 found_command = false;
            for (u32 command_index = 0; command_index < ARRAY_COUNT(console_commands); command_index++) {
                ConsoleCommand candidate = console_commands[command_index];
                if (strings_are_equal(command, candidate.name)) {
                    found_command = true;
                    if (candidate.f == cc_help) {
                        print_help = true;
                    } else {
                        candidate.f(game_state, game_state->editor_state, input, trim_spaces_right(in_buffer));
                    }
                    break;
                }
            }

            if (!found_command) {
                log_print(LogLevel_Error, "Unknown command: %.*s", string_expand(command));
            }

            if (print_help) {
                log_print(LogLevel_Info, "Available commands:");
                log_print(LogLevel_Info, "-------------------");
                for (u32 command_index = 0; command_index < ARRAY_COUNT(console_commands); command_index++) {
                    ConsoleCommand candidate = console_commands[command_index];
                    log_print(LogLevel_Info, "%.*s: %.*s", string_expand(candidate.name), string_expand(candidate.help));
                }
            }
        }
    }
}

internal String find_closest_matching_console_command(String query) {
    String result = {};
    for (u32 candidate_index = 0; candidate_index < ARRAY_COUNT(console_commands); candidate_index++) {
        String candidate = console_commands[candidate_index].name;
        if (starts_with(candidate, query, StringMatch_CaseInsenitive)) {
            result = candidate;
        }
    }
    return result;
}

internal void execute_console(GameState* game_state, ConsoleState* console, GameInput* input) {
    RenderContext* rc = &console->rc;

    v2 mouse_p = vec2(input->mouse_x, input->mouse_y);

    render_screenspace(rc);

    UILayoutContext layout_context;
    layout_context.rc         = rc;
    layout_context.assets     = &game_state->assets;
    layout_context.temp_arena = &game_state->transient_arena;

    v2 dim = get_screen_dim(rc);
    f32 width = dim.x;
    f32 height = dim.y;
    f32 advance = get_advance_for_codepoint_pair(console->font, 'M', 'M');

    if (was_pressed(input->tilde)) {
        if (console->open && !console->in_focus) {
            console->in_focus = true;
        } else {
            if (console->open && !console->wide_open) {
                console->wide_open = true;
            } else {
                console->wide_open = false;
                console->open = !console->open;
                console->in_focus = console->open;
                input->event_mode = console->open;
            }
        }
    }

    input->event_mode = console->open && console->in_focus;

    // @Note: I don't get consolas's sizing
    f32 console_input_box_height = console->font->size + get_line_spacing(console->font);
    f32 console_open_speed = game_config->console_open_speed;
    f32 console_close_speed = game_config->console_close_speed;
    f32 console_height = 0.66f*dim.y;

    f32 openness_target = console->wide_open ? 1.0f : 0.33f;

    if (console->open) {
        if (console->openness_t < openness_target) {
            console->openness_t += input->frame_dt / console_open_speed;
            if (console->openness_t >= openness_target) {
                console->openness_t = openness_target;
            }
        }
    } else {
        if (console->openness_t > 0.0f) {
            console->openness_t -= input->frame_dt / console_close_speed;
            if (console->openness_t <= 0.0f) {
                console->openness_t = 0.0f;
            }
        }
    }

    f32 openness_t = console->openness_t;

    u32 warnings, errors;
    platform.get_unread_log_messages(0, &warnings, &errors);
    UILayout message_counter = make_layout(layout_context, console->font, vec2(width - 50.0f, height - console_height*openness_t - 2.0f*console_input_box_height*openness_t - get_baseline_from_top(console->font)));
    if (warnings) {
        layout_print(&message_counter, vec4(1.0f, 0.5f, 0.0f, 1.0f), "%u ", warnings);
    }
    if (errors) {
        layout_print(&message_counter, COLOR_RED, "%u", errors);
    }
    if (warnings || errors) {
        layout_finish_print(&message_counter);
    }

    if (openness_t > 0.0f) {
        if (console->in_focus) {
            for (u32 event_index = 0; event_index < input->event_count; event_index++) {
                char ascii;
                PlatformKeyCode code = decode_input_event(input->event_buffer[event_index], &ascii);
                switch (code) {
                    case PKC_Escape: {
                        console->in_focus = false;
                    } break;

                    case PKC_Left: {
                        if (console->caret_pos > 0) {
                            --console->caret_pos;
                        }
                    } break;

                    case PKC_Right: {
                        if (console->caret_pos < console->input_buffer_count) {
                            ++console->caret_pos;
                        }
                    } break;

                    case PKC_Tab: {
                        String candidate = find_closest_matching_console_command(input_buffer_as_string(console));
                        if (candidate.len) {
                            console->input_buffer_count = cast(u32) candidate.len;
                            copy(console->input_buffer_count, candidate.data, console->input_buffer);
                            console->input_buffer[console->input_buffer_count++] = ' ';
                            console->caret_pos = console->input_buffer_count;
                        }
                    } break;

                    case PKC_Return: {
                        if (console->input_buffer_count > 0) {
                            String command = input_buffer_as_string(console);
                            console->input_buffer_count = 0;
                            console->caret_pos = 0;
                            execute_console_command(game_state, input, command);
                        }
                    } break;

                    case PKC_Back: {
                        if (console->input_buffer_count > 0) {
                            if (input->ctrl.is_down) {
                                console->input_buffer_count = 0;
                                console->caret_pos = 0;
                            } else {
                                if (console->caret_pos == console->input_buffer_count) {
                                    --console->input_buffer_count;
                                    --console->caret_pos;
                                } else {
                                    if (console->caret_pos > 0) {
                                        --console->caret_pos;
                                    }
                                    --console->input_buffer_count;
                                    for (u32 i = console->caret_pos; i < console->input_buffer_count; ++i) {
                                        console->input_buffer[i] = console->input_buffer[i + 1];
                                    }
                                }
                            }
                        }
                    } break;

                    case PKC_Oem3: {
                        /* Just eat this one silently to avoid leaving a ` or ~ in the console when closing */
                    } break;

                    case PKC_Delete: {
                        if (console->input_buffer_count > console->caret_pos && console->input_buffer_count > 0) {
                            --console->input_buffer_count;
                            for (u32 i = console->caret_pos; i < console->input_buffer_count; ++i) {
                                console->input_buffer[i] = console->input_buffer[i + 1];
                            }
                        }
                    } break;

                    default: {
                        if (ascii) {
                            if (console->input_buffer_count < ARRAY_COUNT(console->input_buffer)) {
                                if (console->caret_pos == console->input_buffer_count) {
                                    console->input_buffer[console->input_buffer_count++] = ascii;
                                    ++console->caret_pos;
                                } else {
                                    for (u32 i = console->input_buffer_count; i > console->caret_pos; --i) {
                                        console->input_buffer[i] = console->input_buffer[i - 1];
                                    }
                                    console->input_buffer[console->caret_pos++] = ascii;
                                    ++console->input_buffer_count;
                                }
                            }
                        }
                    } break;
                }
            }
        }

        // @TODO: Should have some kind of nicer way of handling sorting
        f32 old_sort_bias = rc->sort_key_bias;
        rc->sort_key_bias += 50000.0f;

        f32 console_alpha = console->in_focus ? 1.0f : 0.5f;
        v4 log_box_color = vec4(0.15f, 0.0f, 0.1f, 0.85f);
        v4 input_box_color = vec4(0.1, 0.0f, 0.05f, 0.95f);

        f32 console_log_box_height = cast(f32) height - (console_height + console_input_box_height)*openness_t;
        AxisAlignedBox2 console_log_box = aab_min_max(vec2(0.0f, console_log_box_height), vec2(width, height));
        push_shape(rc, default_transform2d(), rectangle(console_log_box), vec4(log_box_color.rgb, log_box_color.a*console_alpha));

        PlatformLogMessage* selected_message = 0;

        UILayout log = make_layout(layout_context, console->font, vec2(4.0f, console_log_box_height + get_line_spacing(console->font) + get_baseline(console->font)), true);
        for (PlatformLogMessage* message = platform.get_most_recent_log_message(); message; message = message->next) {
            if (log.at_p.y < height) {
                b32 filter_match = false;
                if (console->input_buffer_count > 1 && console->input_buffer[0] == '/') {
                    String filter_string = wrap_string(console->input_buffer_count - 1, console->input_buffer + 1);
                    String match = find_partial_match(message->text, filter_string, StringMatch_CaseInsenitive);
                    if (!match.len) match = find_partial_match(message->file, filter_string, StringMatch_CaseInsenitive);
                    if (!match.len) match = find_partial_match(message->function, filter_string, StringMatch_CaseInsenitive);

                    if (match.len > 0) {
                        filter_match = true;
                    } else {
                        continue;
                    }
                }

                v4 color = COLOR_WHITE;
                if (message->level == LogLevel_Error) {
                    color = COLOR_RED;
                } else if (message->level == LogLevel_Warn) {
                    color = COLOR_YELLOW;
                }

                if (filter_match) {
                    color.rgb = square_root(lerp(color.rgb*color.rgb, COLOR_GREEN.rgb, 0.2f));
                }

                layout_print_line(&log, color, "%.*s", string_expand(message->text));

                if (is_in_aab(log.last_print_bounds, mouse_p)) {
                    selected_message = message;
                }
            } else {
                break;
            }
        }

        v2 input_box_min = vec2(0.0f, console_log_box_height - console_input_box_height);
        AxisAlignedBox2 console_input_box = aab_min_max(input_box_min, vec2(cast(f32) width, console_log_box_height));
        push_shape(rc, default_transform2d(), rectangle(console_input_box), vec4(input_box_color.rgb, input_box_color.a*console_alpha));

        f32 caret_x = advance*(console->caret_pos);
        AxisAlignedBox2 caret_box = aab_min_dim(vec2(caret_x + 4.0f, input_box_min.y - get_baseline(console->font)), vec2(2.0f, (f32)console->font->size)); 
        push_shape(rc, default_transform2d(), rectangle(caret_box), COLOR_WHITE);

        {
            UILayout input_box = make_layout(layout_context, console->font, input_box_min + vec2(4.0f, get_line_spacing(console->font) + get_baseline(console->font)));
            layout_print(&input_box, COLOR_WHITE, "%.*s", console->input_buffer_count, console->input_buffer);

            String closest_command = find_closest_matching_console_command(input_buffer_as_string(console));
            advance_by(&closest_command, console->input_buffer_count);
            if (closest_command.len) {
                layout_print(&input_box, vec4(1.0f, 1.0f, 1.0f, 0.5f + 0.15f*sin(console->caret_breathing)), "%.*s", string_expand(closest_command));
            }
            // if (console->input_buffer_count >= closest_command.len) {
            //     layout_print(&input_box, vec4(1.0f, 1.0f, 1.0f, 0.75f + 0.25f*sin(console->caret_breathing)), " |");
            // }
            console->caret_breathing += 2.0f*input->frame_dt;
            if (console->caret_breathing > TAU_32) {
                console->caret_breathing -= TAU_32;
            }
            layout_finish_print(&input_box);
        }

        if (selected_message) {
            UILayout log_tooltip = make_layout(layout_context, console->font, mouse_p + vec2(0.0f, 12.0f), true);
            rc->sort_key_bias += 100.0f;
            layout_print_line(&log_tooltip, COLOR_WHITE, "%s:%s:%u", selected_message->file, selected_message->function, selected_message->line);
            push_shape(rc, default_transform2d(), rectangle(log_tooltip.last_print_bounds), vec4(0.0f, 0.0f, 0.0f, console_alpha*0.85f), ShapeRenderMode_Fill, -10.0f);
        }

        AxisAlignedBox2 total_console_area = aab_union(console_log_box, console_input_box);
        if (was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
            if (is_in_aab(total_console_area, mouse_p)) {
                console->in_focus = true;
            } else {
                console->in_focus = false;
            }
        }

        rc->sort_key_bias = old_sort_bias;
    }
}
