inline EntityHash* get_entity_hash_slot(EditorState* editor, EntityID guid) {
    EntityHash* result = 0;

    // @TODO: Use external chaining and/or better hash function. Right now I
    // believe this can easily end up so that this lookup can take a long time
    // because the hash values are so densely packed. Any later insertion can
    // end up having to jump many hash slots to find the right entry.
    u32 hash_value = guid.value;

    if (hash_value) {
        for (u32 search = 0; search < ARRAY_COUNT(editor->entity_hash); search++) {
            u32 hash_index = (hash_value + search) % ARRAY_COUNT(editor->entity_hash);
            EntityHash* hash_slot = editor->entity_hash + hash_index;
            if (!hash_slot->guid.value || hash_slot->guid.value == guid.value) {
                if (!hash_slot->guid.value) {
                    hash_slot->guid = guid;
                }
                result = hash_slot;
                break;
            }
        }
    }

    return result;
}

inline Entity* get_entity_from_guid(EditorState* editor, EntityID guid) {
    EntityHash* hash_slot = get_entity_hash_slot(editor, guid);
    assert(hash_slot->index < ARRAY_COUNT(editor->active_level->entities));
    Entity* result = editor->active_level->entities + hash_slot->index;
    return result;
}

inline AddEntityResult add_entity(EditorState* editor, EntityType type, EntityID guid = { 0 }) {
    Level* level = editor->active_level;

    assert(level->entity_count < ARRAY_COUNT(level->entities));

    u32 index = level->entity_count++;
    Entity* entity = level->entities + index;

    if (guid.value) {
        entity->guid = guid;
    } else {
        entity->guid = { level->last_used_guid++ };
    }

    entity->type = type;

    AddEntityResult result;
    result.guid = entity->guid;
    result.ptr = entity;

    EntityHash* hash_slot = get_entity_hash_slot(editor, entity->guid);
    hash_slot->index = index;

    return result;
}

inline AddEntityResult add_player(EditorState* editor, v2 starting_p) {
    AddEntityResult result = add_entity(editor, EntityType_Player);
    Entity* entity = result.ptr;

    entity->p = starting_p;
    entity->collision = circle(0.5f);
    entity->color = vec4(1, 1, 1, 1);

    entity->flags |= EntityFlag_Physical|EntityFlag_Collides;

    return result;
}

inline AddEntityResult add_wall(EditorState* editor, Rect2 rect) {
    AddEntityResult result = add_entity(editor, EntityType_Wall);
    Entity* entity = result.ptr;

    entity->p = get_center(rect);
    entity->color = vec4(1, 1, 1, 1);
    entity->surface_friction = 5.0f;
    entity->midi_test_target = entity->p;

    entity->flags |= EntityFlag_Collides;

    rect = offset(rect, -entity->p);

    v2* verts = push_array(editor->arena, 4, v2, no_clear());
    verts[0] = { rect.min.x, rect.min.y };
    verts[1] = { rect.max.x, rect.min.y };
    verts[2] = { rect.max.x, rect.max.y };
    verts[3] = { rect.min.x, rect.max.y };
    entity->collision = polygon(4, verts);
    // entity->collision = rectangle(rect);

    return result;
}

inline AddEntityResult add_soundtrack_player(EditorState* editor, SoundtrackID soundtrack_id, u32 playback_flags = Playback_Looping) {
    AddEntityResult result = add_entity(editor, EntityType_SoundtrackPlayer);
    Entity* entity = result.ptr;

    entity->soundtrack_id  = soundtrack_id;
    entity->playback_flags = playback_flags;
    entity->sprite = editor->speaker_icon;
    entity->collision = circle(1.0f);
    entity->flags |= EntityFlag_Invisible;

    return result;
}

inline AddEntityResult add_camera_zone(EditorState* editor, Rect2 zone, f32 rotation = 0.0f) {
    AddEntityResult result = add_entity(editor, EntityType_CameraZone);
    Entity* entity = result.ptr;

    entity->p = get_center(zone);
    entity->sprite = editor->camera_icon;

    entity->camera_zone = offset(zone, -entity->p);
    entity->camera_rotation_arm = arm2(rotation);

    entity->collision = circle(1.0f);

    entity->flags |= EntityFlag_Invisible;

    return result;
}

inline void add_undo_history(EditorState* editor, UndoType type, u32 data_size, void* data, char* description = 0);
inline void delete_entity(EditorState* editor, EntityID guid, b32 with_undo_history = true) {
    EntityHash* hash_slot = get_entity_hash_slot(editor, guid);

    if (hash_slot) {
        Level* level = editor->active_level;

        assert(hash_slot->index < ARRAY_COUNT(level->entities));
        Entity* entity = level->entities + hash_slot->index;

        if (editor->selected_entity && editor->selected_entity->guid.value == guid.value) {
            editor->selected_entity = 0;
        }

        if (with_undo_history) {
            add_undo_history(editor, Undo_DeleteEntity, sizeof(*entity), entity);
        }

        hash_slot->guid = { 0 }; // Free deleted entity's hash slot

        level->entities[hash_slot->index] = level->entities[--level->entity_count];
        EntityHash* moved_entity_hash = get_entity_hash_slot(editor, level->entities[hash_slot->index].guid);
        moved_entity_hash->index = hash_slot->index;
    }
}

// @Incomplete: Right now, the undo buffer is simply linear and once you run out of
// space, that's it. Needs to be made into a circular buffer or some other equivalent system.

inline void add_undo_history(EditorState* editor, UndoType type, u32 data_size, void* data, char* description) {
    assert(editor->undo_buffer_last_header + sizeof(UndoHeader) + data_size < ARRAY_COUNT(editor->undo_buffer));

    UndoHeader* last_header = cast(UndoHeader*) (editor->undo_buffer + editor->undo_buffer_last_header);
    u32 next_header_index = editor->undo_buffer_last_header + sizeof(*last_header) + last_header->data_size;

    UndoHeader* header = cast(UndoHeader*) (editor->undo_buffer + next_header_index);

    header->type = type;
    header->description = description;
    header->data_size = data_size;
    header->data_ptr  = data;

    copy(header->data_size, header->data_ptr, header + 1);

    header->prev = editor->undo_buffer_last_header;
    editor->undo_buffer_last_header = next_header_index;

    editor->undo_watermark = editor->undo_buffer_last_header;
}

inline void undo(EditorState* editor) {
    UndoHeader* header = cast(UndoHeader*) (editor->undo_buffer + editor->undo_buffer_last_header);
    if (header->type != Undo_Null) {
        void* data = header + 1;

        TemporaryMemory temp = begin_temporary_memory(editor->arena);
        switch (header->type) {
            case Undo_SetData: {
                void* temp_redo_buffer = push_size(editor->arena, header->data_size, no_clear());
                copy(header->data_size, header->data_ptr, temp_redo_buffer);
                copy(header->data_size, data, header->data_ptr);
                copy(header->data_size, temp_redo_buffer, data);
            } break;

            case Undo_CreateEntity: {
                EntityID guid = (cast(Entity*) data)->guid;
                Entity* entity = get_entity_from_guid(editor, guid);
                if (entity) {
                    assert(header->data_size == sizeof(*entity));
                    copy(header->data_size, entity, data);
                    delete_entity(editor, guid, false);
                }
            } break;

            case Undo_DeleteEntity: {
                Entity* entity_data = cast(Entity*) data;
                AddEntityResult added_entity = add_entity(editor, EntityType_Null, entity_data->guid);
                copy(header->data_size, data, added_entity.ptr);
            } break;
        }
        end_temporary_memory(temp);

        editor->undo_buffer_last_header = header->prev;
    }
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

            case Undo_CreateEntity: {
                Entity* entity_data = cast(Entity*) data;
                AddEntityResult added_entity = add_entity(editor, EntityType_Null, entity_data->guid);
                copy(header->data_size, data, added_entity.ptr);
            } break;

            case Undo_DeleteEntity: {
                EntityID guid = (cast(Entity*) data)->guid;
                Entity* entity = get_entity_from_guid(editor, guid);
                if (entity) {
                    assert(header->data_size == sizeof(*entity));
                    copy(header->data_size, entity, data);
                    delete_entity(editor, guid, false);
                }
            } break;
        }
        end_temporary_memory(temp);

        editor->undo_buffer_last_header = next_header_index;
    }
}

inline Level* allocate_level(MemoryArena* arena, char* name) {
    Level* result = push_struct(arena, Level);

    // @Note: The 0th entity is reserved as the null entity
    result->last_used_guid = 1;
    result->entity_count = 1;

    size_t name_length = cstr_length(name);
    result->name.len = name_length;
    result->name.data = push_string_and_null_terminate(arena, name_length, name); // @Note: Null termination just in case we want to interact with some C APIs.

    return result;
}

inline void load_level(EditorState* editor, Level* level) {
    editor->active_level = level;
    for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
        Entity* entity = editor->active_level->entities + entity_index;
        EntityHash* hash = get_entity_hash_slot(editor, entity->guid);
        hash->guid = entity->guid;
        hash->index = { entity_index };
    }
}

inline void create_debug_level(EditorState* editor) {
    add_player(editor, vec2(2.0f, 1.5f));

    SoundtrackID soundtrack_id = get_soundtrack_id_by_name(editor->assets, "test_soundtrack");
    add_soundtrack_player(editor, soundtrack_id);

    add_wall(editor, rect_min_max(vec2(-35.0f, -0.75f), vec2(1.25f, 0.75f)));

    for (s32 i = 0; i < 12; i++) {
        Entity* wall = add_wall(editor, rect_center_dim(vec2(2.0f + 1.5f*i, 0.0f), vec2(1.5f, 1.5f))).ptr;
        wall->midi_note = 60 + i;
    }

    u32 width = editor->render_group.commands->width;
    u32 height = editor->render_group.commands->height;

    f32 aspect_ratio = cast(f32) width / cast(f32) height;
    add_camera_zone(editor, rect_center_dim(vec2(6.0f, 2.0f), vec2(aspect_ratio*20.0f, 20.0f)));
}

internal void editor_print_va(EditorLayout* layout, v4 color, char* format_string, va_list va_args) {
    EditorState* editor = layout->editor;

    if (!editor->shown) {
        return;
    }

    TemporaryMemory temp = begin_temporary_memory(editor->arena);

    u32 text_size = stbsp_vsnprintf(0, 0, format_string, va_args) + 1; // @Note: stbsp_vsprintf doesn't include the null terminator in the returned size, so I add it in.

    char* text = cast(char*) push_size(editor->arena, text_size, align_no_clear(1));
    stbsp_vsnprintf(text, text_size, format_string, va_args);

    Font* font = layout->active_font;

    if (!layout->print_initialized) {
        layout->last_print_bounds = inverted_infinity_rectangle2();
        layout->print_initialized = true;
    } else {
        if (layout->last_codepoint && text[0]) {
            layout->at_p.x += get_advance_for_codepoint_pair(font, layout->last_codepoint, text[0]);
        }
    }

    for (char* at = text; at[0]; at++) {
        layout->last_codepoint = at[0];
        if (at[0] == ' ') {
            layout->at_p.x += font->whitespace_width;
        } else if (at[0] == '\n') {
            layout->at_p.x  = layout->origin.x;
            layout->at_p.y += layout->vertical_advance;
        } else {
            ImageID glyph_id = get_glyph_id_for_codepoint(font, at[0]);
            if (glyph_id.value) {
                Image* glyph = get_image(editor->assets, glyph_id);
                layout->at_p = vec2(roundf(layout->at_p.x), roundf(layout->at_p.y));
                v2 p = layout->at_p + vec2(layout->depth*font->whitespace_width*4.0f, 0.0f);
                push_image(&editor->render_group, transform2d(p + vec2(1.0f, -1.0f), vec2(glyph->w, glyph->h)), glyph, vec4(0, 0, 0, 0.75f));
                push_image(&editor->render_group, transform2d(p, vec2(glyph->w, glyph->h)), glyph, color);

                Rect2 glyph_rect = offset(get_aligned_image_rect(glyph), p);
                layout->last_print_bounds = rect_union(layout->last_print_bounds, glyph_rect);

                if (at[1] && in_font_range(font, at[1])) {
                    layout->at_p.x += get_advance_for_codepoint_pair(font, at[0], at[1]);
                }
            }
        }
    }

    layout->total_bounds = rect_union(layout->total_bounds, layout->last_print_bounds);

    end_temporary_memory(temp);
}

inline void editor_print(EditorLayout* layout, v4 color, char* format_string, ...) {
    va_list va_args;
    va_start(va_args, format_string);
    editor_print_va(layout, color, format_string, va_args);
    va_end(va_args);
}

internal Rect2 editor_finish_print(EditorLayout* layout) {
    assert(layout->print_initialized);

    Rect2 result = layout->last_print_bounds;
    layout->print_initialized = false;
    layout->last_codepoint = 0;

    layout->at_p.x  = layout->origin.x;
    layout->at_p.y += layout->vertical_advance;

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
DECLARE_EDITABLE_TYPE_INFERENCER(s32)
DECLARE_EDITABLE_TYPE_INFERENCER(v2)
DECLARE_EDITABLE_TYPE_INFERENCER(Rect2)
DECLARE_EDITABLE_TYPE_INFERENCER(EntityID)
DECLARE_EDITABLE_TYPE_INFERENCER(SoundtrackID)
typedef Entity* EntityPtr;
DECLARE_EDITABLE_TYPE_INFERENCER(EntityPtr)

#define add_editable(editables, struct_type, member, ...) \
    add_editable_(editables, infer_editable_type(&(cast(struct_type*) 0)->member), #member, offset_of(struct_type, member), sizeof(cast(struct_type*) 0)->member, ##__VA_ARGS__)
inline EditableParameter* add_editable_(LinearBuffer(EditableParameter) editables, EditableType type, char* name, u32 offset, u32 size, u32 flags = 0) {
    EditableParameter* parameter = lb_push(editables);
    parameter->type = type;
    parameter->name = name;
    parameter->offset = offset;
    parameter->size = size;
    parameter->flags = flags;
    return parameter;
}

#define add_viewable(editables, struct_type, member, ...) \
    add_viewable_(editables, infer_editable_type(&(cast(struct_type*) 0)->member), #member, offset_of(struct_type, member), sizeof(cast(struct_type*) 0)->member, ##__VA_ARGS__)
inline EditableParameter* add_viewable_(LinearBuffer(EditableParameter) editables, EditableType type, char* name, u32 offset, u32 size, u32 flags = 0) {
    return add_editable_(editables, type, name, offset, size, Editable_Static|flags);
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
        add_viewable(editables, Entity, guid);
        add_viewable(editables, Entity, p);
        add_viewable(editables, Entity, support);
        editable = add_viewable(editables, Entity, flags);
        editable->type = Editable_EntityFlag;
    }
    end_editables(editor, editables);

    editables = begin_editables(editor, EntityType_SoundtrackPlayer);
    {
        add_viewable(editables, Entity, guid);
        add_viewable(editables, Entity, soundtrack_id);
        add_viewable(editables, Entity, soundtrack_has_been_played, Editable_IsBool);
    }
    end_editables(editor, editables);

    editables = begin_editables(editor, EntityType_CameraZone);
    {
        add_viewable(editables, Entity, guid);
        add_viewable(editables, Entity, p);
        add_editable(editables, Entity, camera_zone);
    }
    end_editables(editor, editables);

    editables = begin_editables(editor, EntityType_Wall);
    {
        add_viewable(editables, Entity, guid);
        add_viewable(editables, Entity, p);

        editable = add_editable(editables, Entity, midi_note, Editable_IsMidiNote|Editable_RangeLimited);
        editable->e_u32.min_value = 0;
        editable->e_u32.max_value = 127;
    }
    end_editables(editor, editables);
}

char* midi_note_names[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

inline void print_editable(EditorLayout* layout, EditableParameter* editable, void** editable_ptr, v4 color, EditorWidget* widget = 0) {
    switch (editable->type) {
        case Editable_u32: {
            u32 value = *(cast(u32*) editable_ptr);
            if (editable->flags & Editable_IsMidiNote) {
                char* midi_note_name = midi_note_names[value % 12];
                s32 midi_note_octave = (cast(s32) value / 12) - 2;
                editor_print(layout, color, "%s %d", midi_note_name, midi_note_octave);
            } else {
                editor_print(layout, color, "%u", value);
            }
        } break;

        case Editable_s32: {
            s32 value = *(cast(s32*) editable_ptr);
            if (editable->flags & Editable_IsBool) {
                editor_print(layout, color, "%s", value ? "true" : "false");
            } else {
                editor_print(layout, color, "%d", value);
            }
        } break;

        case Editable_v2: {
            v2 value = *(cast(v2*) editable_ptr);
            editor_print(layout, color, "{ %f, %f }", value.x, value.y);
        } break;

        case Editable_Rect2: {
            Rect2 value = *(cast(Rect2*) editable_ptr);
            editor_print(layout, color, "{ min: { %f, %f }, max{ %f, %f } }", value.min.x, value.min.y, value.max.x, value.max.y);
        } break;

        case Editable_EntityID: {
            EntityID id = *(cast(EntityID*) editable_ptr);
            editor_print(layout, color, "EntityID { %u }", id.value);
        } break;

        case Editable_SoundtrackID: {
            SoundtrackID id = *(cast(SoundtrackID*) editable_ptr);
            editor_print(layout, color, "SoundtrackID { %u }", id.value);
        } break;

        case Editable_EntityFlag: {
            u32 flags = *(cast(u32*) editable_ptr);
            char* prev_name = 0;
            char* name;
            while (enum_flag_name(EntityFlag, &flags, &name)) {
                editor_print(layout, color, "%s%s", prev_name ? " | " : "", name);
                prev_name = name;
            }
        } break;

        case Editable_EntityPtr: {
            Entity* entity = *(cast(Entity**) editable_ptr);
            if (entity) {
                editor_print(layout, color, "Entity* -> EntityID { %u }", entity->guid.value);
            } else {
                editor_print(layout, color, "Entity* -> null");
            }
        } break;
    }

    editor_print(layout, color, " ");
}

inline char* widget_name(EditorWidget widget) {
    return enum_name(EditorWidgetType, widget.type);
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
    editor->world_mouse_p_on_active = editor->world_mouse_p;
}

inline void clear_active(EditorState* editor) {
    editor->active_widget = {};
}

inline v2 snap_to_grid(EditorState* editor, v2 p) {
    v2 result = p;
    if (editor->grid_snapping_enabled) {
        result = vec2(roundf(p.x / editor->grid_size.x), roundf(p.y / editor->grid_size.y)) * editor->grid_size;
    }
    return result;
}

inline EditorState* allocate_editor(GameState* game_state, GameRenderCommands* render_commands, Level* active_level) {
    EditorState* editor = push_struct(&game_state->permanent_arena, EditorState);

    initialize_render_group(&editor->render_group, render_commands, 0.0f);

    editor->assets = &game_state->assets;
    editor->arena = &game_state->transient_arena;

    editor->shown = true;

    editor->camera_icon = get_image_by_name(editor->assets, "camera_icon");
    editor->speaker_icon = get_image_by_name(editor->assets, "speaker_icon");

    editor->big_font = get_font_by_name(editor->assets, "editor_font_big");
    editor->font = get_font_by_name(editor->assets, "editor_font");

    editor->top_margin  = cast(f32) render_commands->height - get_baseline_from_top(editor->font);
    editor->left_margin = 4.0f;

    editor->grid_snapping_enabled = true;
    editor->grid_size = vec2(0.5f, 0.5f);

    editor->zoom = 1.0f;

    set_up_editable_parameters(editor);
    load_level(editor, active_level);
    create_debug_level(editor);

    return editor;
}

internal void execute_editor(GameState* game_state, EditorState* editor, GameInput* input) {
    u32 width = editor->render_group.commands->width;
    u32 height = editor->render_group.commands->height;

    render_screenspace(&editor->render_group);
    editor->top_margin  = cast(f32) editor->render_group.commands->height - get_baseline_from_top(editor->font);
    editor->left_margin = 4.0f;

    if (game_state->game_mode == GameMode_Editor) {
        if (input->shift.is_down) {
            if (was_pressed(get_key(input, 'W'))) {
                editor->zoom *= 2.0f;
            }
            if (was_pressed(get_key(input, 'D'))) {
                editor->zoom *= 0.5f;
            }
        }

        game_state->render_group.vertical_fov /= editor->zoom;
    }

    v2 mouse_p = vec2(input->mouse_x, input->mouse_y);
    v2 world_mouse_p = screen_to_world(&game_state->render_group, transform2d(mouse_p)).offset;

    editor->mouse_p = mouse_p;
    editor->world_mouse_p = mouse_p;

    if (input->space.is_down && input->mouse_buttons[PlatformMouseButton_Left].is_down) {
        if (!editor->panning) {
            editor->panning = true;
            editor->world_mouse_p_on_pan = world_mouse_p;
            editor->camera_p_on_pan = game_state->render_group.camera_p;
        } else {
            game_state->render_group.camera_p = editor->camera_p_on_pan;
            v2 pan_world_mouse_p = screen_to_world(&game_state->render_group, transform2d(mouse_p)).offset;
            game_state->render_group.camera_p += editor->world_mouse_p_on_pan - pan_world_mouse_p;
        }
    } else {
        editor->panning = false;
    }

    if (!editor->shown) {
        return;
    }

    EditorLayout layout = make_layout(editor, vec2(4.0f, editor->top_margin));

    Level* level = editor->active_level;
    editor_print_line(&layout, COLOR_WHITE, "Editing level '%s'", level->name.data);
    editor_print_line(&layout, COLOR_WHITE, "Hot Widget: %s", widget_name(editor->hot_widget));
    editor_print_line(&layout, COLOR_WHITE, "Active Widget: %s", widget_name(editor->active_widget));

    if (was_pressed(get_key(input, 'S'))) {
        editor->grid_snapping_enabled = !editor->grid_snapping_enabled;
    }

    EditorLayout status_bar = make_layout(editor, vec2(4.0f, get_line_spacing(editor->font) + get_baseline(editor->font)), true);
    editor_print_line(&status_bar, COLOR_WHITE, "Entity Count: %u/%u | Grid Snapping: %s, Grid Size: { %f, %f } | Zoom: %gx",
        level->entity_count, ARRAY_COUNT(level->entities),
        editor->grid_snapping_enabled ? "true" : "false", editor->grid_size.x, editor->grid_size.y,
        editor->zoom
    );

    EditorLayout undo_log = make_layout(editor, vec2(500.0f, editor->top_margin));

    u32 undo_buffer_used = editor->undo_buffer_last_header + (cast(UndoHeader*) (editor->undo_buffer + editor->undo_buffer_last_header))->data_size;
    u32 undo_buffer_size = ARRAY_COUNT(editor->undo_buffer);
    editor_print_line(&undo_log, COLOR_WHITE, "Undo Buffer Usage: %dKB/%dKB (%.02f%%)",
        undo_buffer_used / 1024,
        undo_buffer_size / 1024,
        100.0f*(cast(f32) undo_buffer_used / cast(f32) undo_buffer_size)
    );

    undo_log.depth++;

    u32 undo_log_count = 0;
    for (u32 header_index = editor->undo_watermark; header_index > 0 && undo_log_count < 5;) {
        UndoHeader* header = cast(UndoHeader*) (editor->undo_buffer + header_index);
        v4 color = vec4(0.85f, 0.85f, 0.85f, 1.0f);
        if (header_index > editor->undo_buffer_last_header) {
            color = vec4(1.0f, 1.0f, 0.5f, 1.0f);
        }
        editor_print(&undo_log, color, "[%04u] %s: ", header_index, enum_name(UndoType, header->type));
        switch (header->type) {
            case Undo_SetData: {
                if (header->description) {
                    editor_print(&undo_log, color, "%s", header->description);
                } else {
                    editor_print(&undo_log, color, "0x%I64x", cast(u64) header->data_ptr);
                }
            } break;
            case Undo_CreateEntity:
            case Undo_DeleteEntity: {
                EntityID id = (cast(Entity*) (header + 1))->guid;
                editor_print(&undo_log, color, "EntityID { %d }", id.value);
            } break;
        }
        editor_finish_print(&undo_log);
        header_index = header->prev;
        undo_log_count++;
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

        if (!(entity->flags & EntityFlag_Invisible) || (game_state->game_mode == GameMode_Editor && editor->shown)) {
            if (gjk_intersect_point(transform2d(entity->p), entity->collision, world_mouse_p)) {
                // if (game_state->game_mode == GameMode_Ingame) {
                //     moused_over = get_entity(level, entity->id);
                // } else {
                    moused_over = entity;
                // }
                moused_over->color = vec4(1, 0, 1, 1);
            }
        }
    }

    if (input->ctrl.is_down) {
        if (was_pressed(get_key(input, 'Z'))) {
            undo(editor);
        } else if (was_pressed(get_key(input, 'R'))) {
            redo(editor);
        }
    }

    EditorWidget manipulate_entity = {};
    manipulate_entity.guid = &manipulate_entity;
    manipulate_entity.type = Widget_ManipulateEntity;
    manipulate_entity.manipulate.entity = moused_over;

    if (moused_over) {
        editor->next_hot_widget = manipulate_entity;
    } else if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
        if (editor->active_widget.type == Widget_None && !editor->panning) {
            editor->selected_entity = 0;
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
                    add_undo_history(editor, Undo_SetData, sizeof(widget->manipulate.entity->p), &widget->manipulate.entity->p, "Moved Entity");
                } else if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
                    editor->selected_entity = moused_over;
                    clear_active(editor);
                }
            } break;
            case Manipulate_DragEntity: {
                widget->manipulate.entity->p = snap_to_grid(editor, world_mouse_p - widget->manipulate.drag_offset);
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
                delete_entity(editor, widget->manipulate.entity->guid);
                clear_active(editor);
            } break;
        }
    } else if (is_hot(editor, manipulate_entity)) {
        EditorWidget widget = editor->hot_widget;
        if (input->del.is_down && input->mouse_buttons[PlatformMouseButton_Left].is_down) {
            widget.manipulate.type = Manipulate_DeleteEntity;
            set_active(editor, widget);
        } else if (was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
            widget.manipulate.drag_offset = world_mouse_p - widget.manipulate.entity->p;
            set_active(editor, widget);
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
            editor_print_line(&spawn_menu, color, enum_name(EntityType, entity_type));

            if (is_in_rect(spawn_menu.last_print_bounds, mouse_p)) {
                highlighted_type = entity_type;
            }
        }
        editor->type_to_spawn = highlighted_type;
    } else if (was_released(input->shift)) {
        Entity* created_entity = 0;
        switch (editor->type_to_spawn) {
            case EntityType_Player: {
                created_entity = add_player(editor, snap_to_grid(editor, world_mouse_p)).ptr;
            } break;
            case EntityType_CameraZone: {
                f32 aspect_ratio = cast(f32) width / cast(f32) height;
                created_entity = add_camera_zone(editor, rect_center_dim(snap_to_grid(editor, world_mouse_p), vec2(aspect_ratio*20.0f, 20.0f))).ptr;
            } break;
        }

        if (created_entity) {
            add_undo_history(editor, Undo_CreateEntity, sizeof(*created_entity), created_entity);
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
        Entity* respective_entity = 0;
        if (game_state->game_mode == GameMode_Ingame) {
            respective_entity = get_entity_from_guid(editor, selected->guid);
        }

        if (selected->type == EntityType_CameraZone) {
            EditorWidget drag_camera_zone;
            drag_camera_zone.guid = selected;
            drag_camera_zone.type = Widget_DragRect;

            Rect2 zone = offset(selected->camera_zone, selected->p);

            {
                v2 corner = zone.min;
                Rect2 corner_rect = rect_center_dim(corner, vec2(0.5f, 0.5f));
                push_shape(&game_state->render_group, transform2d(corner), rectangle(corner_rect), COLOR_YELLOW, ShapeRenderMode_Outline);
            }

            {
                v2 corner = vec2(zone.max.x, zone.min.y);
                Rect2 corner_rect = rect_center_dim(corner, vec2(0.5f, 0.5f));
                push_shape(&game_state->render_group, transform2d(corner), rectangle(corner_rect), COLOR_YELLOW, ShapeRenderMode_Outline);
            }

            {
                v2 corner = zone.max;
                Rect2 corner_rect = rect_center_dim(corner, vec2(0.5f, 0.5f));
                push_shape(&game_state->render_group, transform2d(corner), rectangle(corner_rect), COLOR_YELLOW, ShapeRenderMode_Outline);
            }

            {
                v2 corner = vec2(zone.min.x, zone.max.y);
                Rect2 corner_rect = rect_center_dim(corner, vec2(0.5f, 0.5f));
                push_shape(&game_state->render_group, transform2d(corner), rectangle(corner_rect), COLOR_YELLOW, ShapeRenderMode_Outline);
            }
        }

        editor_print_line(&layout, COLOR_WHITE, "");
        editor_print_line(&layout, COLOR_WHITE, "Entity (%d, %s)", selected->guid.value, enum_name(EntityType, selected->type));

        layout.depth++;

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

                v4 color = vec4(0.95f, 0.95f, 0.95f, 1);
                if (!(editable->flags & Editable_Static)) {
                    if (is_hot(editor, widget) || is_active(editor, widget)) {
                        color = vec4(1, 1, 0.5f, 1);
                    }
                }

                editor_print(&layout, color, "%s: ", editable->name);
                print_editable(&layout, editable, editable_ptr, color, &widget);

                if (is_active(editor, widget)) {
                    v2 start_mouse_p = editor->mouse_p_on_active;

                    switch (editable->type) {
                        case Editable_u32: {
                            s32 delta = cast(s32) (length(mouse_p - start_mouse_p) / 8.0f);
                            u32 new_value = cast(u32) editor->active_widget.start_value;
                            if ((mouse_p - start_mouse_p).y < 0.0f) {
                                delta = -delta;
                                if (ABS(delta) > cast(s32) new_value) {
                                    delta = -cast(s32) new_value;
                                }
                            }
                            if (editable->flags & Editable_RangeLimited) {
                                new_value = CLAMP(new_value + delta, editable->e_u32.min_value, editable->e_u32.max_value);
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
                        if (!(editable->flags & Editable_Static)) {
                            add_undo_history(editor, Undo_SetData, editable->size, editable_ptr);
                        }
                    }
                }

                if (is_in_rect(layout.last_print_bounds, mouse_p)) {
                    editor->next_hot_widget = widget;
                }

                if (respective_entity) {
                    editable_ptr = cast(void**) (cast(u8*) respective_entity + editable->offset);
                    print_editable(&layout, editable, editable_ptr, vec4(0.7f, 0.7f, 0.7f, 1.0f));
                }

                editor_finish_print(&layout);
            }
        }

        if (was_pressed(input->del)) {
            delete_entity(editor, selected->guid);
        }
    }

    editor->hot_widget = editor->next_hot_widget;
    editor->next_hot_widget = {};
}
