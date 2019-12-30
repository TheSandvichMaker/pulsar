inline EditorState* allocate_editor_state(GameRenderCommands* render_commands, Assets* assets, MemoryArena* permanent_arena, MemoryArena* transient_arena) {
    EditorState* editor = push_struct(permanent_arena, EditorState);
    editor->render_commands = render_commands;
    editor->assets = assets;
    editor->arena = transient_arena;
    return editor;
}

internal void editor_print(EditorState* editor, v4 color, char* format_string, ...) {
    TemporaryMemory temp = begin_temporary_memory(editor->arena);

    va_list va_args;
    va_start(va_args, format_string);

    u32 text_size = stbsp_vsnprintf(0, 0, format_string, va_args) + 1; // @Note: stbsp_vsprintf doesn't include the null terminator in the returned size, so I add it in.

    char* text = (char*)push_size(editor->arena, text_size, align_no_clear(1));
    stbsp_vsnprintf(text, text_size, format_string, va_args);

    va_end(va_args);

    Font* font = editor->font;
    for (char* at = text; at[0]; at++) {
        if (at[0] == ' ') {
            editor->at_p.x += font->whitespace_width;
        } else if (at[0] == '\n') {
            editor->at_p.x  = editor->origin.x;
            editor->at_p.y -= get_line_spacing(editor->font) + editor->spacing;
        } else {
            ImageID glyph_id = get_glyph_id_for_codepoint(font, at[0]);
            if (glyph_id.value) {
                Image* glyph = get_image(editor->assets, glyph_id);
                push_image(editor->render_commands, glyph, editor->at_p + vec2(2.0f, -2.0f), vec4(0, 0, 0, 0.75f));
                push_image(editor->render_commands, glyph, editor->at_p, color);

                Rect2 glyph_rect = rect_min_dim(editor->at_p - glyph->align*vec2(glyph->w, glyph->h), vec2(glyph->w, glyph->h));
                if (editor->bounds_initialized) {
                    editor->last_print_bounds = rect_union(editor->last_print_bounds, glyph_rect);
                } else {
                    editor->last_print_bounds = glyph_rect;
                    editor->bounds_initialized = true;
                }

                if (at[1] && at[1] != ' ') {
                    editor->at_p.x += get_advance_for_codepoint_pair(font, at[0], at[1]);
                } else {
                    // @TODO: Is this what you want to do?
                    editor->at_p.x += get_advance_for_codepoint_pair(font, at[0], at[0]);
                }
            }
        }
    }

    end_temporary_memory(temp);
}

internal Rect2 editor_finish_print(EditorState* editor) {
    Rect2 result = editor->last_print_bounds;

    editor->bounds_initialized = false;
    editor->at_p.x  = editor->origin.x;
    editor->at_p.y -= get_line_spacing(editor->font) + editor->spacing;

    return result;
}

#define editor_print_line(editor, color, format_string, ...) (editor_print(editor, color, format_string, ##__VA_ARGS__), editor_finish_print(editor))

#define add_editable(editables, struct_type, member, member_type) add_editable_(editables, Editable_##member_type, #member, offset_of(struct_type, member), sizeof(member_type))
inline EditableParameter* add_editable_(LinearBuffer(EditableParameter) editables, EditableType type, char* name, u32 offset, u32 size) {
    EditableParameter* parameter = lb_push(editables);
    parameter->type = type;
    parameter->name = name;
    parameter->offset = offset;
    parameter->size = size;
    return parameter;
}

inline LinearBuffer(EditableParameter) begin_editables(EditorState* editor, EntityType type) {
    assert(editor->current_editable_type == EntityType_Null);

    editor->current_editable_type = type;
    assert(editor->editable_parameter_count[editor->current_editable_type] == 0);

    editor->editable_parameter_info[editor->current_editable_type] = begin_linear_buffer(editor->arena, EditableParameter);
    return editor->editable_parameter_info[editor->current_editable_type];
}

inline void end_editables(EditorState* editor, LinearBuffer(EditableParameter) editables) {
    assert(editor->current_editable_type != EntityType_Null);

    end_linear_buffer(editables);
    editor->editable_parameter_count[editor->current_editable_type] = cast(u32) lb_count(editables);

    editor->current_editable_type = EntityType_Null;
}

internal void set_up_editable_parameters(EditorState* editor) {
    EditableParameter* editable = 0;
    LinearBuffer(EditableParameter) editables = 0;

    editables = begin_editables(editor, EntityType_Player);
    {
        add_editable(editables, Entity, p, v2);
    }
    end_editables(editor, editables);

    editables = begin_editables(editor, EntityType_Wall);
    {
        add_editable(editables, Entity, p, v2);

        editable = add_editable(editables, Entity, midi_note, u32);
        editable->flags |= Editable_IsMidiNote|Editable_RangeLimited;
        editable->e_u32.min_value = 0;
        editable->e_u32.max_value = 127;
    }
    end_editables(editor, editables);
}

char* midi_note_names[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

internal void initialize_editor(EditorState* editor, Level* active_level) {
    u32 width = editor->render_commands->width;
    u32 height = editor->render_commands->height;

    editor->active_level = active_level;
    editor->font = get_font_by_name(editor->assets, "editor_font");
    editor->origin = vec2(4.0f, height - get_baseline_from_top(editor->font));
    editor->at_p = editor->origin;
    editor->spacing = 8.0f;

    set_up_editable_parameters(editor);

    editor->initialized = true;
}

internal void execute_editor(EditorState* editor, GameInput* input) {
    v2 mouse_p = vec2(input->mouse_x, input->mouse_y);

    u32 width = editor->render_commands->width;
    u32 height = editor->render_commands->height;

    editor->at_p = editor->origin;

    Level* level = editor->active_level;
    editor_print_line(editor, vec4(1, 1, 1, 1), "Editing level '%s'", level->name.data);

    Entity* moused_over = 0;
    for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
        Entity* entity = level->entities + entity_index;

        if (gjk_intersect_point(transform2d(entity->p), entity->collision, mouse_p)) {
            moused_over = entity;
            moused_over->color = vec4(1, 0, 1, 1);
        }
    }

    if (was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
        // if (moused_over == editor->selected_entity) {
        //     editor->selected_entity = 0;
        // } else {
            editor->selected_entity = moused_over;
        // }
    }

    Entity* selected = 0;
    if (editor->selected_entity) {
        selected = editor->selected_entity;
        selected->color = vec4(0, 1, 0, 1);
    } else {
        selected = moused_over;
    }

    if (selected) {
        EditableParameter* editables = editor->editable_parameter_info[selected->type];
        if (editables) {
            EditableParameter* highlighted_editable = 0;

            u32 editables_count = editor->editable_parameter_count[selected->type];
            for (u32 editable_index = 0; editable_index < editables_count; editable_index++) {
                EditableParameter* editable = editables + editable_index;
                void* editable_ptr = cast(void*) (cast(u8*) selected + editable->offset);

                b32 is_hot = (editable == editor->selected_editable);
                v4 color = is_hot ? vec4(1, 0, 1, 1) : vec4(1, 1, 1, 1);

                editor_print(editor, color, "%s: ", editable->name);

                switch (editable->type) {
                    case Editable_v2: {
                        v2* value = cast(v2*) editable_ptr;
                        editor_print(editor, color, "{ %f, %f }", value->x, value->y);
                    } break;

                    case Editable_u32: {
                        u32 value = *(cast(u32*) editable_ptr);
                        if (editable->flags & Editable_IsMidiNote) {
                            char* midi_note_name = midi_note_names[value % 12];
                            s32 midi_note_octave = (cast(s32) value / 12) - 2;
                            editor_print(editor, color, "%s %d", midi_note_name, midi_note_octave);
                        } else {
                            editor_print(editor, color, "%d", value);
                        }
                    } break;
                }

                editor_finish_print(editor);

                if (is_in_rect(editor->last_print_bounds, mouse_p)) {
                    highlighted_editable = editable;
                }
            }

            editor->selected_editable = highlighted_editable;
        }

        if (input->shift_down && input->mouse_buttons[PlatformMouseButton_Left].is_down) {
            selected->p = mouse_p;
        }
    }
}
