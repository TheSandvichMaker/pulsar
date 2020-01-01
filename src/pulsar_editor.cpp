inline EditorState* allocate_editor_state(GameRenderCommands* render_commands, Assets* assets, MemoryArena* permanent_arena, MemoryArena* transient_arena) {
    EditorState* editor = push_struct(permanent_arena, EditorState);
    editor->render_commands = render_commands;
    editor->assets = assets;
    editor->arena = transient_arena;
    return editor;
}

internal void editor_print(EditorLayout* layout, v4 color, char* format_string, ...) {
    EditorState* editor = layout->editor;

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
            layout->at_p.x += font->whitespace_width;
        } else if (at[0] == '\n') {
            layout->at_p.x  = layout->origin.x;
            layout->at_p.y -= get_line_spacing(font) + layout->spacing;
        } else {
            ImageID glyph_id = get_glyph_id_for_codepoint(font, at[0]);
            if (glyph_id.value) {
                Image* glyph = get_image(editor->assets, glyph_id);
                layout->at_p = vec2(roundf(layout->at_p.x), roundf(layout->at_p.y));
                push_image(editor->render_commands, glyph, layout->at_p + vec2(1.0f, -1.0f), vec4(0, 0, 0, 0.75f));
                push_image(editor->render_commands, glyph, layout->at_p, color);

                Rect2 glyph_rect = offset(get_aligned_image_rect(glyph), layout->at_p);
                if (layout->print_initialized) {
                    layout->last_print_bounds = rect_union(layout->last_print_bounds, glyph_rect);
                } else {
                    layout->last_print_bounds = glyph_rect;
                    layout->print_initialized = true;
                }

                if (at[1] && at[1] != ' ') {
                    layout->at_p.x += get_advance_for_codepoint_pair(font, at[0], at[1]);
                } else {
                    // @TODO: Is this what you want to do?
                    layout->at_p.x += get_advance_for_codepoint_pair(font, at[0], at[0]);
                }
            }
        }
    }

    end_temporary_memory(temp);
}

internal Rect2 editor_finish_print(EditorLayout* layout) {
    assert(layout->print_initialized);

    Rect2 result = layout->last_print_bounds;

    layout->print_initialized = false;
    layout->at_p.x  = layout->origin.x;
    layout->at_p.y -= get_line_spacing(layout->editor->font) + layout->spacing;

    return result;
}

#define editor_print_line(layout, color, format_string, ...)       \
    (                                                              \
        assert(!(layout)->print_initialized),                      \
        editor_print(layout, color, format_string, ##__VA_ARGS__), \
        editor_finish_print(layout)                                \
    )

#define DECLARE_EDITABLE_TYPE_INFERENCER(Type)            \
    inline EditableType infer_editable_type(Type* type) { \
        EditableType result = Editable_##Type;            \
        return result;                                    \
    }

DECLARE_EDITABLE_TYPE_INFERENCER(u32)
DECLARE_EDITABLE_TYPE_INFERENCER(v2)
DECLARE_EDITABLE_TYPE_INFERENCER(EntityID)

#define add_editable(editables, struct_type, member) add_editable_(editables, infer_editable_type(&(cast(struct_type*) 0)->member), #member, offset_of(struct_type, member), sizeof(cast(struct_type*) 0)->member)
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
        add_editable(editables, Entity, id);
        add_editable(editables, Entity, p);
    }
    end_editables(editor, editables);

    editables = begin_editables(editor, EntityType_Wall);
    {
        add_editable(editables, Entity, id);
        add_editable(editables, Entity, p);

        editable = add_editable(editables, Entity, midi_note);
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

    set_up_editable_parameters(editor);

    editor->initialized = true;
}

inline void print_editable(EditorLayout* layout, EditableParameter* editable, void** editable_ptr, v4 color, EditorWidget* widget = 0) {
    switch (editable->type) {
        case Editable_v2: {
            v2 value = *(cast(v2*) editable_ptr);
            editor_print(layout, color, "{ %f, %f }", value.x, value.y);
        } break;

        case Editable_u32: {
            u32 value = *(cast(u32*) editable_ptr);
            if (editable->flags & Editable_IsMidiNote) {
                char* midi_note_name = midi_note_names[value % 12];
                s32 midi_note_octave = (cast(s32) value / 12) - 2;
                editor_print(layout, color, "%s %d", midi_note_name, midi_note_octave);
            } else {
                editor_print(layout, color, "%d", value);
            }
        } break;

        case Editable_EntityID: {
            EntityID id = *(cast(EntityID*) editable_ptr);
            editor_print(layout, color, "EntityID { %d }", id.value);
        } break;
    }
}

inline b32 widgets_equal(EditorWidget a, EditorWidget b) {
    b32 result = (a.guid && b.guid && a.type == b.type && a.guid == b.guid);
    return result;
}

inline b32 is_hot(EditorState* editor, EditorWidget widget) {
    b32 result = widgets_equal(editor->hot_widget, widget);
    return result;
}

inline b32 is_active(EditorState* editor, EditorWidget widget) {
    b32 result = widgets_equal(editor->active_widget, widget);
    return result;
}

inline void set_active(EditorState* editor, EditorWidget widget) {
    editor->active_widget = widget;
    editor->mouse_p_on_active = editor->mouse_p;
}

inline void clear_active(EditorState* editor) {
    editor->active_widget = {};
}

inline void add_undo_history(EditorState* editor, UndoType type, u32 data_size, void* data) {
    assert(editor->undo_buffer_last_header + sizeof(UndoHeader) + data_size < ARRAY_COUNT(editor->undo_buffer));

    UndoHeader* last_header = cast(UndoHeader*) (editor->undo_buffer + editor->undo_buffer_last_header);

    u32 next_header_index = 0;
    if (last_header->type != Undo_Null) {
        next_header_index = editor->undo_buffer_last_header + sizeof(*last_header) + last_header->data_size;
    } else {
        next_header_index = 0;
    }

    UndoHeader* header = cast(UndoHeader*) (editor->undo_buffer + next_header_index);

    header->type = type;
    header->data_size = data_size;
    header->data_ptr  = data;

    copy(header->data_size, header->data_ptr, header + 1);

    header->prev = editor->undo_buffer_last_header;
    editor->undo_buffer_last_header = next_header_index;

    editor->undo_watermark = editor->undo_buffer_last_header;
}

inline void undo(EditorState* editor) {
    UndoHeader* header = cast(UndoHeader*) (editor->undo_buffer + editor->undo_buffer_last_header);
    void* data = header + 1;

    TemporaryMemory temp = begin_temporary_memory(editor->arena);
    switch (header->type) {
        case Undo_SetData: {
            void* temp_redo_buffer = push_size(editor->arena, header->data_size, no_clear());
            copy(header->data_size, header->data_ptr, temp_redo_buffer);
            copy(header->data_size, data, header->data_ptr);
            copy(header->data_size, temp_redo_buffer, data);
        } break;
        case Undo_DeleteEntity: {
            AddEntityResult added_entity = add_entity(editor->active_level, EntityType_Null);
            (cast(Entity*) data)->id = added_entity.id;
            copy(header->data_size, data, added_entity.ptr);
        } break;
    }
    end_temporary_memory(temp);

    editor->undo_buffer_last_header = header->prev;
}

inline void redo(EditorState* editor) {
    if (editor->undo_buffer_last_header < editor->undo_watermark) {
        UndoHeader* last_header = cast(UndoHeader*) (editor->undo_buffer + editor->undo_buffer_last_header);
        u32 next_header_index = editor->undo_buffer_last_header + sizeof(*last_header) + last_header->data_size;

        UndoHeader* header = cast(UndoHeader*) (editor->undo_buffer + next_header_index);
        void* data = header + 1;

        TemporaryMemory temp = begin_temporary_memory(editor->arena);
        switch (header->type) {
            case Undo_SetData: {
                void* temp_undo_buffer = push_size(editor->arena, header->data_size, no_clear());
                copy(header->data_size, header->data_ptr, temp_undo_buffer);
                copy(header->data_size, data, header->data_ptr);
                copy(header->data_size, temp_undo_buffer, data);
            } break;
            case Undo_DeleteEntity: {
                EntityID id = (cast(Entity*) data)->id;
                Entity* entity = get_entity(editor->active_level, id);
                if (entity) {
                    assert(header->data_size == sizeof(*entity));
                    copy(header->data_size, entity, data);
                    delete_entity(editor->active_level, id);
                }
            } break;
        }
        end_temporary_memory(temp);

        editor->undo_buffer_last_header = next_header_index;
    }
}

inline void delete_entity(EditorState* editor, EntityID id) {
    Entity* entity = get_entity(editor->active_level, id);

    if (entity) {
        if (editor->selected_entity && editor->selected_entity->id.value == id.value) {
            editor->selected_entity = 0;
        }

        add_undo_history(editor, Undo_DeleteEntity, sizeof(*entity), entity);
        delete_entity(editor->active_level, id);
    }
}

internal void execute_editor(GameState* game_state, EditorState* editor, GameInput* input) {
    v2 mouse_p = vec2(input->mouse_x, input->mouse_y);
    editor->mouse_p = mouse_p;

    u32 width = editor->render_commands->width;
    u32 height = editor->render_commands->height;

    EditorLayout layout = make_layout(editor, vec2(4.0f, height - get_baseline_from_top(editor->font)));

    Level* level = editor->active_level;
    editor_print_line(&layout, vec4(1, 1, 1, 1), "Editing level '%s'", level->name.data);
    editor_print_line(&layout, vec4(1, 1, 1, 1), "Undo Buffer Usage: %dB/%dB", editor->undo_buffer_last_header + (cast(UndoHeader*) (editor->undo_buffer + editor->undo_buffer_last_header))->data_size, ARRAY_COUNT(editor->undo_buffer));
    editor_print_line(&layout, vec4(1, 1, 1, 1), "Hot Widget: %s", widget_name(editor->hot_widget));
    editor_print_line(&layout, vec4(1, 1, 1, 1), "Active Widget: %s", widget_name(editor->active_widget));

    for (u32 header_index = 0; header_index <= editor->undo_watermark;) {
        UndoHeader* header = cast(UndoHeader*) (editor->undo_buffer + header_index);
        v4 color = vec4(0.85f, 0.85f, 0.85f, 1.0f);
        if (header_index > editor->undo_buffer_last_header) {
            color = vec4(1.0f, 1.0f, 0.5f, 1.0f);
        }
        editor_print(&layout, color, "%s (%dB): ", undo_type_name(header->type), header->data_size);
        switch (header->type) {
            case Undo_SetData: {
                editor_print(&layout, color, "%I64x", cast(u64) header->data_ptr);
            } break;
            case Undo_DeleteEntity: {
                EntityID id = (cast(Entity*) (header + 1))->id;
                editor_print(&layout, color, "EntityID { %d }", id.value);
            } break;
        }
        editor_finish_print(&layout);
        header_index += sizeof(*header) + header->data_size;
    }

    u32 entity_count = 0;
    Entity* entities = 0;
    if (game_state->game_mode == GameMode_Ingame) {
        entity_count = game_state->entity_count;
        entities = game_state->entities;
    } else {
        entity_count = level->entity_count;
        entities = level->entities;
    }

    Entity* moused_over = 0;
    for (u32 entity_index = 1; entity_index < entity_count; entity_index++) {
        Entity* entity = entities + entity_index;

        if (gjk_intersect_point(transform2d(entity->p), entity->collision, mouse_p)) {
            if (game_state->game_mode == GameMode_Ingame) {
                moused_over = get_entity(level, entity->id);
            } else {
                moused_over = entity;
            }
            moused_over->color = vec4(1, 0, 1, 1);
        }
    }

    if (input->ctrl.is_down && was_pressed(input->z)) {
        if (input->shift.is_down) {
            redo(editor);
        } else {
            undo(editor);
        }
    }

    EditorWidget manipulate_entity = {};
    manipulate_entity.guid = &manipulate_entity;
    manipulate_entity.type = Widget_ManipulateEntity;
    manipulate_entity.manipulate.entity = moused_over;

    if (moused_over) {
        editor->next_hot_widget = manipulate_entity;
    } else if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
        editor->selected_entity = 0;
    }

    if (is_hot(editor, manipulate_entity)) {
        if (input->del.is_down && input->mouse_buttons[PlatformMouseButton_Left].is_down) {
            editor->hot_widget.manipulate.type = Manipulate_DeleteEntity;
            set_active(editor, editor->hot_widget);
        } else if (was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
            manipulate_entity.manipulate.drag_offset = mouse_p - editor->hot_widget.manipulate.entity->p;
            set_active(editor, editor->hot_widget);
        }
    }

    if (is_active(editor, manipulate_entity)) {
        EditorWidget* widget = &editor->active_widget;
        switch (widget->manipulate.type) {
            case Manipulate_Default: {
                f32 drag_distance = length(mouse_p - editor->mouse_p_on_active);
                if (input->mouse_buttons[PlatformMouseButton_Left].is_down && drag_distance > 5.0f) {
                    editor->selected_entity = widget->manipulate.entity;
                    widget->manipulate.type = Manipulate_DragEntity;
                    add_undo_history(editor, Undo_SetData, sizeof(widget->manipulate.entity->p), &widget->manipulate.entity->p);
                } else if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
                    editor->selected_entity = moused_over;
                    clear_active(editor);
                }
            } break;
            case Manipulate_DragEntity: {
                widget->manipulate.entity->p = mouse_p - widget->manipulate.drag_offset;
                if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
                    editor->selected_entity = 0;
                    clear_active(editor);
                } else if (was_released(input->mouse_buttons[PlatformMouseButton_Right])) {
                    editor->selected_entity = 0;
                    clear_active(editor);
                    undo(editor);
                }
            } break;
            case Manipulate_DeleteEntity: {
                delete_entity(editor, widget->manipulate.entity->id);
                clear_active(editor);
            } break;
        }
    }

    if (was_pressed(input->shift)) {
        editor->spawn_menu_p = mouse_p;
    }

    if (input->shift.is_down) {
        EditorLayout spawn_menu = make_layout(editor, editor->spawn_menu_p);

        EntityType highlighted_type = EntityType_Null;
        for (u32 entity_type_id = cast(u32) EntityType_Null + 1; entity_type_id < EntityType_Count; entity_type_id++) {
            EntityType entity_type = cast(EntityType) entity_type_id;

            v4 color = (editor->type_to_spawn == entity_type) ? vec4(1, 0, 1, 1) : vec4(1, 1, 1, 1);
            editor_print_line(&spawn_menu, color, entity_type_name(entity_type));

            if (is_in_rect(spawn_menu.last_print_bounds, mouse_p)) {
                highlighted_type = entity_type;
            }
        }
        editor->type_to_spawn = highlighted_type;
    } else if (was_released(input->shift)) {
        switch (editor->type_to_spawn) {
            case EntityType_Player: {
                editor->selected_entity = add_player(game_state, level, mouse_p).ptr;
            } break;
        }

        editor->type_to_spawn = EntityType_Null;
    }

    Entity* selected = 0;
    if (editor->selected_entity) {
        selected = editor->selected_entity;
        selected->color = vec4(0, 1, 0, 1);
    } else {
        selected = moused_over;
    }

    if (selected) {
        Entity* sim_entity = 0;
        if (game_state->game_mode == GameMode_Ingame) {
            sim_entity = game_state->entities + selected->id.value;
        }

        EditableParameter* editables = editor->editable_parameter_info[selected->type];
        if (editables) {
            EditableParameter* highlighted_editable = 0;

            u32 editables_count = editor->editable_parameter_count[selected->type];
            for (u32 editable_index = 0; editable_index < editables_count; editable_index++) {
                EditableParameter* editable = editables + editable_index;
                void** editable_ptr = cast(void**) (cast(u8*) selected + editable->offset);

                EditorWidget widget;
                widget.guid = editable_ptr;
                widget.type = Widget_DragEditable;
                widget.start_value = cast(void*) *(cast(u64*) editable_ptr);

                v4 color = is_hot(editor, widget) ? vec4(1, 0, 1, 1) : vec4(1, 1, 1, 1);

                editor_print(&layout, color, "%s: ", editable->name);
                print_editable(&layout, editable, editable_ptr, color, &widget);

                if (is_active(editor, widget)) {
                    v2 start_mouse_p = editor->mouse_p_on_active;

                    switch (editable->type) {
                        case Editable_u32: {
                            u32 delta = cast(u32) (length(mouse_p - start_mouse_p) / 8.0f);
                            u32 new_value = cast(u32) editor->active_widget.start_value + delta;
                            if (editable->flags & Editable_RangeLimited) {
                                new_value = CLAMP(new_value, editable->e_u32.min_value, editable->e_u32.max_value);
                            }
                            *(cast(u32*) editable_ptr) = new_value;
                        } break;
                    }

                    if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
                        clear_active(editor);
                    }
                    if (was_released(input->mouse_buttons[PlatformMouseButton_Right])) {
                        undo(editor);
                        clear_active(editor);
                    }
                } else if (is_hot(editor, widget)) {
                    if (was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
                        set_active(editor, editor->hot_widget);
                        add_undo_history(editor, Undo_SetData, editable->size, editable_ptr);
                    }
                }

                if (is_in_rect(layout.last_print_bounds, mouse_p)) {
                    editor->next_hot_widget = widget;
                }

                if (sim_entity) {
                    editable_ptr = cast(void**) (cast(u8*) sim_entity + editable->offset);
                    print_editable(&layout, editable, editable_ptr, vec4(0.5f, 0.5f, 0.5f, 1.0f));
                }

                editor_finish_print(&layout);
            }
        }

        if (was_pressed(input->del)) {
            delete_entity(editor, selected->id);
        }
    }

    editor->hot_widget = editor->next_hot_widget;
    editor->next_hot_widget = {};
}
