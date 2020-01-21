// @TODO: Ponder the desirableness of EntityData<T>

template <typename T>
inline EntityData<T> wrap_entity_data(Entity* entity, T* member_ptr) {
    assert(cast(void*) member_ptr >= cast(void*) entity && cast(void*) (member_ptr + 1) <= cast(void*) (entity + 1));
    EntityData<T> result;
    result.guid = entity->guid;
    result.offset = cast(u32) (cast(u8*) member_ptr - cast(u8*) entity);
    return result;
}

template <typename T>
inline u32 get_data_size(EntityData<T> data) {
    u32 result = safe_truncate_u64u32(sizeof(T));
    return result;
}

template <typename T>
inline T* get_data(EditorState* editor, EntityData<T> data) {
    T* result = 0;
    Entity* entity = get_entity_from_guid(editor, data.guid);
    if (entity) {
        result = cast(T*) (cast(u8*) entity + data.offset);
    } else {
        log_print(LogLevel_Error, "Could not retrieve entity data from Entity { %u }", data.guid.value);
    }
    return result;
}

inline EntityHash* get_entity_hash_slot(EditorState* editor, EntityID guid) {
    EntityHash* result = 0;

    // @TODO: Use some kind of not totally superbly made-uppy hash? Who knows, maybe this is fine. Let's be honest, you're not gonna notice the difference.
    u32 hash_constant = cast(u32) (1.6180339f*ARRAY_COUNT(editor->entity_hash)); // ...Whatever this is. Wrap by golden ratio, or something.
    u32 hash_value = (guid.value * hash_constant) % ARRAY_COUNT(editor->entity_hash);

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
    Entity* result = 0;
    EntityHash* hash_slot = get_entity_hash_slot(editor, guid);
    if (hash_slot) {
        if (hash_slot->index && hash_slot->index < ARRAY_COUNT(editor->game_state->active_level->entities)) {
            result = editor->game_state->active_level->entities + hash_slot->index;
        }
    }
    return result;
}

inline AddEntityResult add_entity(EditorState* editor, EntityType type, EntityID guid = { 0 }) {
    Level* level = editor->game_state->active_level;

    assert(level->entity_count < ARRAY_COUNT(level->entities));

    u32 index = level->entity_count++;
    Entity* entity = level->entities + index;
    zero_struct(*entity);

    if (guid.value) {
        entity->guid = guid;
    } else {
        entity->guid = { level->first_available_guid++ };
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
    entity->collision = vec2(0.4f, 1.0f);
    entity->color = vec4(1, 1, 1, 1);

    entity->flags |= EntityFlag_Physical|EntityFlag_Collides;

    return result;
}

inline AddEntityResult add_wall(EditorState* editor, AxisAlignedBox2 aab, SoundtrackID listening_to = { 0 }) {
    AddEntityResult result = add_entity(editor, EntityType_Wall);
    Entity* entity = result.ptr;

    entity->p = get_center(aab);
    entity->color = vec4(1, 1, 1, 1);
    entity->start_p = entity->p;
    entity->end_p = entity->p + vec2(0, 5);
    entity->movement_speed_ms = 200.0f;
    entity->movement_t = 1.0f;
    entity->listening_to = listening_to;

    entity->flags |= EntityFlag_Collides;

    entity->collision = get_dim(aab);

    return result;
}

inline AddEntityResult add_soundtrack_player(EditorState* editor, AxisAlignedBox2 inner_zone, SoundtrackID soundtrack_id, u32 playback_flags = Playback_Looping) {
    AddEntityResult result = add_entity(editor, EntityType_SoundtrackPlayer);
    Entity* entity = result.ptr;

    entity->p = get_center(inner_zone);
    entity->audible_zone = get_dim(inner_zone);
    entity->soundtrack_id  = soundtrack_id;
    entity->playback_flags = playback_flags;
    entity->sprite = editor->speaker_icon;

    entity->collision = editor->default_collision;

    entity->flags |= EntityFlag_Invisible;

    return result;
}

inline AddEntityResult add_camera_zone(EditorState* editor, AxisAlignedBox2 active_region, f32 view_region_height, f32 rotation = 0.0f) {
    AddEntityResult result = add_entity(editor, EntityType_CameraZone);
    Entity* entity = result.ptr;

    entity->p = get_center(active_region);
    entity->active_region = get_dim(active_region);
    entity->view_region_height = view_region_height;

    entity->sprite = editor->camera_icon;

    entity->camera_rotation_arm = arm2(rotation);

    entity->collision = editor->default_collision;

    entity->flags |= EntityFlag_Invisible;

    return result;
}

inline AddEntityResult add_checkpoint(EditorState* editor, AxisAlignedBox2 zone) {
    AddEntityResult result = add_entity(editor, EntityType_Checkpoint);
    Entity* entity = result.ptr;

    entity->p = get_center(zone);
    entity->checkpoint_zone = get_dim(zone);

    entity->sprite = editor->checkpoint_icon;

    entity->collision = editor->default_collision;

    entity->flags |= EntityFlag_Invisible;

    return result;
}

inline void add_undo_history(EditorState* editor, UndoType type, u32 data_size, void* data, char* description = 0);
inline void delete_entity(EditorState* editor, EntityID guid, b32 with_undo_history = true) {
    EntityHash* hash_slot = get_entity_hash_slot(editor, guid);

    if (hash_slot) {
        Level* level = editor->game_state->active_level;

        assert(hash_slot->index < ARRAY_COUNT(level->entities));
        Entity* entity = level->entities + hash_slot->index;

        if (editor->selected_entity.value && editor->selected_entity.value == guid.value) {
            editor->selected_entity = { 0 };
        }

        if (with_undo_history) {
            add_undo_history(editor, Undo_DeleteEntity, sizeof(*entity), entity);
        }

        hash_slot->guid = { 0 }; // Free the deleted entity's hash slot

        if (level->entity_count > 1) {
            level->entities[hash_slot->index] = level->entities[--level->entity_count];
            EntityHash* moved_entity_hash = get_entity_hash_slot(editor, level->entities[hash_slot->index].guid);
            moved_entity_hash->index = hash_slot->index;
        } else {
            level->entity_count--;
        }
    }
}

inline void* get_undo_data(UndoFooter* footer) {
    void* result = cast(u8*) footer - footer->data_size;
    return result;
}

inline UndoFooter* get_undo_footer(EditorState* editor, u32 offset) {
    UndoFooter* result = offset ? cast(UndoFooter*) (editor->undo_buffer + offset) : 0;
    return result;
}

// @Note: This is more complicated and hard to understand than I would like
inline UndoFooter* add_undo_footer(EditorState* editor, u32 data_size) {
    UndoFooter* prev_footer = 0;
    u32 data_index = 0;
    if (editor->undo_most_recent) {
        prev_footer = get_undo_footer(editor, editor->undo_most_recent);
        data_index = editor->undo_most_recent + sizeof(UndoFooter);
    } else {
        editor->undo_oldest = 0;
    }

    u32 footer_index = data_index + data_size;
    u32 watermark = footer_index + sizeof(UndoFooter);

    b32 wrapped = false;
    if (watermark > ARRAY_COUNT(editor->undo_buffer)) {
        wrapped = true;
        watermark = ARRAY_COUNT(editor->undo_buffer) + data_size + sizeof(UndoFooter);
    }

    UndoFooter* oldest = get_undo_footer(editor, editor->undo_oldest);

    if (oldest) {
        u32 oldest_index = editor->undo_oldest;
        if (oldest_index <= editor->undo_most_recent) {
            oldest_index += ARRAY_COUNT(editor->undo_buffer);
        }
        while (watermark > oldest_index - oldest->data_size) {
            editor->undo_oldest = oldest->next;

            oldest = get_undo_footer(editor, editor->undo_oldest);
            if (oldest) {
                oldest->prev = 0;

                oldest_index = editor->undo_oldest;
                if (oldest_index < editor->undo_most_recent) {
                    oldest_index += ARRAY_COUNT(editor->undo_buffer);
                }
            } else {
                break;
            }
        }
    } else {
        editor->undo_oldest = footer_index;
    }

    if (wrapped) {
        data_index = 0;
        footer_index = data_index + data_size;
    }

    UndoFooter* footer = get_undo_footer(editor, footer_index);
    zero_struct(*footer);

    if (prev_footer) {
        prev_footer->next = footer_index;
    }
    footer->prev = editor->undo_most_recent;

    editor->undo_most_recent = footer_index;

    return footer;
}

inline void add_undo_history(EditorState* editor, UndoType type, u32 data_size, void* data, char* description) {
    assert(data_size + sizeof(UndoFooter) <= sizeof(editor->undo_buffer));

    UndoFooter* footer = add_undo_footer(editor, data_size);

    footer->type = type;
    footer->description = description;
    footer->data_size = data_size;
    footer->data_ptr = data;

    copy(footer->data_size, footer->data_ptr, get_undo_data(footer));
}

inline void add_entity_data_undo_history_(EditorState* editor, EntityID guid, u32 data_size, u32 data_offset, char* description = 0) {
    assert(data_size + sizeof(UndoFooter) <= sizeof(editor->undo_buffer));

    UndoFooter* footer = add_undo_footer(editor, data_size);

    Entity* entity = get_entity_from_guid(editor, guid);
    if (entity) {
        footer->type = Undo_SetEntityData;
        footer->description = description;
        footer->data_size = data_size;

        footer->entity_guid = guid;
        footer->entity_data_offset = data_offset;

        copy(footer->data_size, cast(u8*) entity + footer->entity_data_offset, get_undo_data(footer));
    }
}

template <typename T>
inline void add_entity_data_undo_history(EditorState* editor, EntityData<T> entity_data, char* description = 0) {
    add_entity_data_undo_history_(editor, entity_data.guid, get_data_size(entity_data), entity_data.offset);
}

// @TODO: Think about deduplicating the symmetric undo/redo cases
inline void undo(EditorState* editor) {
    UndoFooter* footer = get_undo_footer(editor, editor->undo_most_recent);
    if (footer) {
        void* data = get_undo_data(footer);

        TemporaryMemory temp = begin_temporary_memory(editor->arena);
        switch (footer->type) {
            case Undo_SetEntityData: {
                void* temp_redo_buffer = push_size(editor->arena, footer->data_size, no_clear());
                Entity* entity = get_entity_from_guid(editor, footer->entity_guid);
                void* entity_data = cast(u8*) entity + footer->entity_data_offset;
                copy(footer->data_size, entity_data, temp_redo_buffer);
                copy(footer->data_size, data, entity_data);
                copy(footer->data_size, temp_redo_buffer, data);
            } break;

            case Undo_SetData: {
                void* temp_redo_buffer = push_size(editor->arena, footer->data_size, no_clear());
                copy(footer->data_size, footer->data_ptr, temp_redo_buffer);
                copy(footer->data_size, data, footer->data_ptr);
                copy(footer->data_size, temp_redo_buffer, data);
            } break;

            case Undo_CreateEntity: {
                EntityID guid = (cast(Entity*) data)->guid;
                Entity* entity = get_entity_from_guid(editor, guid);
                if (entity) {
                    assert(footer->data_size == sizeof(*entity));
                    copy(footer->data_size, entity, data);
                    delete_entity(editor, guid, false);
                }
            } break;

            case Undo_DeleteEntity: {
                Entity* entity_data = cast(Entity*) data;
                AddEntityResult added_entity = add_entity(editor, EntityType_Null, entity_data->guid);
                copy(footer->data_size, data, added_entity.ptr);
            } break;
        }
        end_temporary_memory(temp);

        editor->undo_most_recent = footer->prev;
    }
}

inline void redo(EditorState* editor) {
    u32 footer_index = 0;
    if (!editor->undo_most_recent) {
        footer_index = editor->undo_oldest;
    } else {
        UndoFooter* prev_footer = get_undo_footer(editor, editor->undo_most_recent);
        assert(prev_footer);
        footer_index = prev_footer->next;
    }

    if (footer_index) {
        UndoFooter* footer = get_undo_footer(editor, footer_index);
        void* data = get_undo_data(footer);

        TemporaryMemory temp = begin_temporary_memory(editor->arena);
        switch (footer->type) {
            case Undo_SetEntityData: {
                void* temp_undo_buffer = push_size(editor->arena, footer->data_size, no_clear());
                Entity* entity = get_entity_from_guid(editor, footer->entity_guid);
                void* entity_data = cast(u8*) entity + footer->entity_data_offset;
                copy(footer->data_size, entity_data, temp_undo_buffer);
                copy(footer->data_size, data, entity_data);
                copy(footer->data_size, temp_undo_buffer, data);
            } break;

            case Undo_SetData: {
                void* temp_undo_buffer = push_size(editor->arena, footer->data_size, no_clear());
                copy(footer->data_size, footer->data_ptr, temp_undo_buffer);
                copy(footer->data_size, data, footer->data_ptr);
                copy(footer->data_size, temp_undo_buffer, data);
            } break;

            case Undo_CreateEntity: {
                Entity* entity_data = cast(Entity*) data;
                AddEntityResult added_entity = add_entity(editor, EntityType_Null, entity_data->guid);
                copy(footer->data_size, data, added_entity.ptr);
            } break;

            case Undo_DeleteEntity: {
                EntityID guid = (cast(Entity*) data)->guid;
                Entity* entity = get_entity_from_guid(editor, guid);
                if (entity) {
                    assert(footer->data_size == sizeof(*entity));
                    copy(footer->data_size, entity, data);
                    delete_entity(editor, guid, false);
                }
            } break;
        }
        end_temporary_memory(temp);

        editor->undo_most_recent = footer_index;
    }
}

inline void load_level_into_editor(EditorState* editor, Level* level) {
    // @Note: Weird paranoid assert in case I ever change the hash from a fixed size array and forget to change this...
    assert(sizeof(editor->entity_hash) == sizeof(EntityHash)*MAX_ENTITY_COUNT);

    zero_size(sizeof(editor->entity_hash), editor->entity_hash);

    for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
        Entity* entity = level->entities + entity_index;
        EntityHash* hash = get_entity_hash_slot(editor, entity->guid);
        hash->guid = entity->guid;
        hash->index = { entity_index };
    }
}

inline void create_debug_level(EditorState* editor) {
    editor->game_state->active_level->entity_count = 1;
    editor->game_state->active_level->first_available_guid = 1;

    add_player(editor, vec2(-8.0f, 1.5f));

    SoundtrackID soundtrack_id = get_soundtrack_id_by_name(editor->assets, string_literal("test_soundtrack"));
    add_soundtrack_player(editor, aab_center_dim(vec2(-2.0f, 4.0f), vec2(20.0f, 10.0f)), soundtrack_id);

    add_wall(editor, aab_min_max(vec2(-35.0f, -1.0f), vec2(2.0f, 1.0f)));

    for (s32 i = 0; i < 13; i++) {
        Entity* wall = add_wall(editor, aab_center_dim(vec2(3.0f + 2.0f*i, 0.0f), vec2(2.0f, 2.0f))).ptr;
        wall->behaviour = WallBehaviour_Move;
        wall->midi_note = 60 + i;
        wall->end_p = wall->start_p + vec2(0, 1 + i);
        if (i % 2 == 1) {
            wall->flags |= EntityFlag_Hazard;
        }
    }

    add_checkpoint(editor, aab_min_dim(vec2(-6.0f, 1.0f), vec2(4.0f, 8.0f)));

    u32 width = editor->render_context.commands->width;
    u32 height = editor->render_context.commands->height;

    add_camera_zone(editor, aab_center_dim(vec2(0.0f, 4.0f), vec2(35.0f, 15.0f)), 15.0f);
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

internal void layout_print_va(UILayout* layout, v4 color, char* format_string, va_list va_args) {
    TemporaryMemory temp = begin_temporary_memory(layout->context.temp_arena);

    u32 text_size = stbsp_vsnprintf(0, 0, format_string, va_args) + 1; // @Note: stbsp_vsprintf doesn't include the null terminator in the returned size, so I add it in.

    char* text = cast(char*) push_size(layout->context.temp_arena, text_size, align_no_clear(1));
    stbsp_vsnprintf(text, text_size, format_string, va_args);

    Font* font = layout->context.font;

    if (!layout->print_initialized) {
        layout->last_print_bounds = inverted_infinity_aab2();
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
                Image* glyph = get_image(layout->context.assets, glyph_id);
                layout->at_p = vec2(roundf(layout->at_p.x), roundf(layout->at_p.y));
                v2 p = layout->at_p + vec2(layout->depth*font->whitespace_width*4.0f, 0.0f);
                push_image(layout->context.rc, transform2d(p + vec2(1.0f, -1.0f), vec2(glyph->w, glyph->h)), glyph, vec4(0, 0, 0, color.a));
                push_image(layout->context.rc, transform2d(p, vec2(glyph->w, glyph->h)), glyph, color);

                AxisAlignedBox2 glyph_aab = offset(get_aligned_image_aab(glyph), p);
                layout->last_print_bounds = aab_union(layout->last_print_bounds, glyph_aab);

                if (at[1] && in_font_range(font, at[1])) {
                    layout->at_p.x += get_advance_for_codepoint_pair(font, at[0], at[1]);
                }
            }
        }
    }

    layout->total_bounds = aab_union(layout->total_bounds, layout->last_print_bounds);

    end_temporary_memory(temp);
}

inline void layout_print(UILayout* layout, v4 color, char* format_string, ...) {
    va_list va_args;
    va_start(va_args, format_string);
    layout_print_va(layout, color, format_string, va_args);
    va_end(va_args);
}

internal AxisAlignedBox2 layout_finish_print(UILayout* layout) {
    assert(layout->print_initialized);

    AxisAlignedBox2 result = layout->last_print_bounds;
    layout->print_initialized = false;
    layout->last_codepoint = 0;

    layout->at_p.x  = layout->origin.x;
    layout->at_p.y += layout->vertical_advance;

    return result;
}

#define layout_print_line(layout, color, format_string, ...)       \
    (                                                              \
        assert(!(layout)->print_initialized),                      \
        layout_print(layout, color, format_string, ##__VA_ARGS__), \
        layout_finish_print(layout)                                \
    )

#define DECLARE_EDITABLE_TYPE_INFERENCER(Type)            \
    inline EditableType infer_editable_type(Type* type) { \
        EditableType result = Editable_##Type;            \
        return result;                                    \
    }

DECLARE_EDITABLE_TYPE_INFERENCER(u32)
DECLARE_EDITABLE_TYPE_INFERENCER(s32)
DECLARE_EDITABLE_TYPE_INFERENCER(f32)
DECLARE_EDITABLE_TYPE_INFERENCER(v2)
DECLARE_EDITABLE_TYPE_INFERENCER(AxisAlignedBox2)
DECLARE_EDITABLE_TYPE_INFERENCER(EntityID)
DECLARE_EDITABLE_TYPE_INFERENCER(SoundtrackID)
typedef Entity* EntityPtr;
DECLARE_EDITABLE_TYPE_INFERENCER(EntityPtr)
DECLARE_EDITABLE_TYPE_INFERENCER(WallBehaviour)

#define add_editable(editables, struct_type, member, ...) \
    add_editable_(editables, infer_editable_type(&(cast(struct_type*) 0)->member), #member, offsetof(struct_type, member), sizeof_member(struct_type, member), ##__VA_ARGS__)
inline EditableParameter* add_editable_(LinearBuffer<EditableParameter>* editables, EditableType type, char* name, u32 offset, u32 size, u32 flags = 0) {
    EditableParameter* parameter = lb_push(editables);
    parameter->type = type;
    parameter->name = name;
    parameter->offset = offset;
    parameter->size = size;
    parameter->flags = flags;
    return parameter;
}

#define add_viewable(editables, struct_type, member, ...) \
    add_viewable_(editables, infer_editable_type(&(cast(struct_type*) 0)->member), #member, offsetof(struct_type, member), sizeof_member(struct_type, member), ##__VA_ARGS__)
inline EditableParameter* add_viewable_(LinearBuffer<EditableParameter>* editables, EditableType type, char* name, u32 offset, u32 size, u32 flags = 0) {
    return add_editable_(editables, type, name, offset, size, Editable_Static|flags);
}

inline LinearBuffer<EditableParameter>* begin_editables(EditorState* editor, EntityType type) {
    assert(editor->current_editable_type == EntityType_Null);

    editor->current_editable_type = type;

    editor->editable_parameter_info[editor->current_editable_type] = begin_linear_buffer<EditableParameter>(editor->arena);
    return editor->editable_parameter_info[editor->current_editable_type];
}

inline void end_editables(EditorState* editor, LinearBuffer<EditableParameter>* editables) {
    assert(editor->current_editable_type != EntityType_Null);

    end_linear_buffer(editables);

    editor->current_editable_type = EntityType_Null;
}

internal void set_up_editable_parameters(EditorState* editor) {
    EditableParameter* editable = 0;
    LinearBuffer<EditableParameter>* editables = 0;

    editables = begin_editables(editor, EntityType_Player);
    {
        add_viewable(editables, Entity, guid);
        editable = add_viewable(editables, Entity, flags);
        editable->type = Editable_EntityFlag;
        add_viewable(editables, Entity, p);
        add_viewable(editables, Entity, dp);
        add_viewable(editables, Entity, ddp);
        add_viewable(editables, Entity, gravity);
        add_viewable(editables, Entity, early_jump_timer);
        add_viewable(editables, Entity, late_jump_timer);
        add_viewable(editables, Entity, dead, Editable_IsBool);
        add_viewable(editables, Entity, support);
    }
    end_editables(editor, editables);

    editables = begin_editables(editor, EntityType_Wall);
    {
        add_viewable(editables, Entity, guid);
        editable = add_viewable(editables, Entity, flags);
        editable->type = Editable_EntityFlag;
        add_viewable(editables, Entity, p);
        add_editable(editables, Entity, behaviour);
        add_viewable(editables, Entity, listening_to);
        editable = add_editable(editables, Entity, movement_speed_ms, Editable_RangeLimited);
        editable->e_f32.min_value = 10.0f;
        editable->e_f32.max_value = FLT_MAX;
        editable = add_editable(editables, Entity, midi_note, Editable_IsMidiNote|Editable_RangeLimited);
        editable->e_u32.min_value = 0;
        editable->e_u32.max_value = 127;
    }
    end_editables(editor, editables);

    editables = begin_editables(editor, EntityType_SoundtrackPlayer);
    {
        add_viewable(editables, Entity, guid);
        editable = add_viewable(editables, Entity, flags);
        editable->type = Editable_EntityFlag;
        add_viewable(editables, Entity, soundtrack_id);
        add_viewable(editables, Entity, audible_zone);
        editable = add_editable(editables, Entity, horz_fade_region, Editable_RangeLimited);
        editable->e_f32.min_value = 0.0f;
        editable->e_f32.max_value = FLT_MAX;
        editable = add_editable(editables, Entity, vert_fade_region, Editable_RangeLimited);
        editable->e_f32.min_value = 0.0f;
        editable->e_f32.max_value = FLT_MAX;
    }
    end_editables(editor, editables);

    editables = begin_editables(editor, EntityType_CameraZone);
    {
        add_viewable(editables, Entity, guid);
        editable = add_viewable(editables, Entity, flags);
        editable->type = Editable_EntityFlag;
        add_viewable(editables, Entity, p);
        add_viewable(editables, Entity, active_region);
        add_editable(editables, Entity, view_region_height);
    }
    end_editables(editor, editables);

    editables = begin_editables(editor, EntityType_Checkpoint);
    {
        add_viewable(editables, Entity, guid);
        editable = add_viewable(editables, Entity, flags);
        editable->type = Editable_EntityFlag;
        add_viewable(editables, Entity, p);
        add_viewable(editables, Entity, checkpoint_zone);
        add_viewable(editables, Entity, most_recent_player_position);
    }
    end_editables(editor, editables);

    //
    // Prefabs (@TODO: Clean this up)
    //

    editor->entity_prefabs[EntityType_Wall] = begin_linear_buffer<EntityPrefab>(editor->arena);
    LinearBuffer<EntityPrefab>* prefabs = editor->entity_prefabs[EntityType_Wall];
    lb_add(prefabs, EntityPrefab_Hazard);
    lb_add(prefabs, EntityPrefab_InvisibleHazard);
    end_linear_buffer(prefabs);
}

char* midi_note_names[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

inline void print_editable(UILayout* layout, EditableParameter* editable, void** editable_ptr, v4 color, EditorWidget* widget = 0) {
    switch (editable->type) {
        case Editable_u32: {
            u32 value = *(cast(u32*) editable_ptr);
            if (editable->flags & Editable_IsMidiNote) {
                char* midi_note_name = midi_note_names[value % 12];
                s32 midi_note_octave = (cast(s32) value / 12) - 2;
                layout_print(layout, color, "%s %d", midi_note_name, midi_note_octave);
            } else {
                layout_print(layout, color, "%u", value);
            }
        } break;

        case Editable_s32: {
            s32 value = *(cast(s32*) editable_ptr);
            if (editable->flags & Editable_IsBool) {
                layout_print(layout, color, "%s", value ? "true" : "false");
            } else {
                layout_print(layout, color, "%d", value);
            }
        } break;

        case Editable_f32: {
            f32 value = *(cast(f32*) editable_ptr);
            layout_print(layout, color, "%f", value);
        } break;

        case Editable_v2: {
            v2 value = *(cast(v2*) editable_ptr);
            layout_print(layout, color, "{ %f, %f }", value.x, value.y);
        } break;

        case Editable_AxisAlignedBox2: {
            AxisAlignedBox2 value = *(cast(AxisAlignedBox2*) editable_ptr);
            layout_print(layout, color, "{ min: { %f, %f }, max{ %f, %f } }", value.min.x, value.min.y, value.max.x, value.max.y);
        } break;

        case Editable_EntityID: {
            EntityID id = *(cast(EntityID*) editable_ptr);
            layout_print(layout, color, "EntityID { %u }", id.value);
        } break;

        case Editable_SoundtrackID: {
            SoundtrackID id = *(cast(SoundtrackID*) editable_ptr);
            layout_print(layout, color, "SoundtrackID { %u }", id.value);
        } break;

        case Editable_EntityFlag: {
            u32 flags = *(cast(u32*) editable_ptr);
            char* prev_name = 0;
            char* name;
            while (enum_flag_name(EntityFlag, &flags, &name)) {
                layout_print(layout, color, "%s%s", prev_name ? " | " : "", name);
                prev_name = name;
            }
        } break;

        case Editable_EntityPtr: {
            Entity* entity = *(cast(Entity**) editable_ptr);
            if (entity) {
                layout_print(layout, color, "Entity* -> EntityID { %u }", entity->guid.value);
            } else {
                layout_print(layout, color, "Entity* -> null");
            }
        } break;

        case Editable_WallBehaviour: {
            WallBehaviour value = *(cast(WallBehaviour*) editable_ptr);
            layout_print(layout, color, enum_name(WallBehaviour, value));
        } break;
    }

    layout_print(layout, color, " ");
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

inline f32 snap_to(f32 x, f32 step) {
    f32 result = x;
    if (step != 0.0f) {
        result = roundf(x / step) * step;
    }
    return result;
}

inline f32 snap_to_grid_x(EditorState* editor, f32 x) {
    f32 result = snap_to(x, editor->grid_snapping_enabled ? editor->grid_size.x : 0.0f);
    return result;
}

inline f32 snap_to_grid_y(EditorState* editor, f32 y) {
    f32 result = snap_to(y, editor->grid_snapping_enabled ? editor->grid_size.y : 0.0f);
    return result;
}

inline v2 snap_to_grid(EditorState* editor, v2 p) {
    v2 result = p;
    if (editor->grid_snapping_enabled) {
        result = vec2(roundf(p.x / editor->grid_size.x), roundf(p.y / editor->grid_size.y)) * editor->grid_size;
    }
    return result;
}

#define CONSOLE_COMMAND(name) void name(GameState* game_state, EditorState* editor, GameInput* input, String arguments)
typedef CONSOLE_COMMAND(ConsoleCommandFunction);

struct ConsoleCommand {
    ConsoleCommandFunction* f;
    String name;
};

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

internal CONSOLE_COMMAND(cc_set_soundtrack) {
    Entity* entity = get_entity_from_guid(editor, editor->selected_entity);
    if (entity) {
        SoundtrackID soundtrack_id = get_soundtrack_id_by_name(editor->assets, arguments);
        if (soundtrack_id.value) {
            switch (entity->type) {
                case EntityType_SoundtrackPlayer: { entity->soundtrack_id = soundtrack_id; } break;
                case EntityType_Wall: { entity->listening_to = soundtrack_id; } break;
                default: { log_print(LogLevel_Error, "Selected entity of type %s does not have any set_soundtrack behaviour", enum_name(EntityType, entity->type)); } break;
            }
        } else {
            log_print(LogLevel_Error, "Could not find soundtrack '%.*s'", PRINTF_STRING(arguments));
        }
    } else {
        log_print(LogLevel_Error, "No entity selected");
    }
}

internal CONSOLE_COMMAND(cc_switch_gamemode) {
    for (u32 mode_index = 0; mode_index < GameMode_Count; mode_index++) {
        GameMode mode = cast(GameMode) mode_index;
        char* mode_name = enum_name(GameMode, mode);

        if (mode_name &&
            (strings_are_equal(arguments, mode_name, StringMatch_CaseInsenitive) ||
             strings_are_equal(arguments, mode_name + sizeof("GameMode_") - 1, StringMatch_CaseInsenitive))
        ) {
            switch_gamemode(game_state, cast(GameMode) mode_index);
        }
    }
}

internal CONSOLE_COMMAND(cc_quit) {
    input->quit_requested = true;
}

#define console_command(name) { cc_##name, string_literal(#name) }
global ConsoleCommand console_commands[] = {
    console_command(load_level),
    console_command(save_level),
    console_command(set_soundtrack),
    console_command(switch_gamemode),
    console_command(quit),
};

inline void execute_console_command(GameState* game_state, GameInput* input, String in_buffer) {
    if (in_buffer.data[0] != '/') {
        String command = advance_word(&in_buffer);
        if (command.len > 0) {
            b32 found_command = false;
            for (u32 command_index = 0; command_index < ARRAY_COUNT(console_commands); command_index++) {
                ConsoleCommand candidate = console_commands[command_index];
                if (strings_are_equal(command, candidate.name)) {
                    found_command = true;
                    candidate.f(game_state, game_state->editor_state, input, trim_spaces_right(in_buffer));
                    break;
                }
            }

            if (!found_command) {
                log_print(LogLevel_Error, "Unknown command: %.*s", PRINTF_STRING(command));
            }
        }
    }
}

inline EditorWidget drag_region_widget(GameState* game_state, EditorState* editor, v2 p, EntityData<v2> target) {
    EditorWidget widget;
    widget.type = Widget_DragRegion;
    widget.guid = &widget;

    EditorWidgetDragRegion* drag_region = &widget.drag_region;

    Entity* entity = get_entity_from_guid(editor, target.guid);

    v2 zone = *get_data(editor, target);
    drag_region->original = zone;
    drag_region->original_p = entity->p;

    drag_region->target = target;

    v4 corner_color = is_hot(editor, widget) || is_active(editor, widget) ? COLOR_RED : COLOR_YELLOW;

    Transform2D t = transform2d(p);

    {
        v2 corner = 0.5f*vec2(-zone.x, -zone.y);
        AxisAlignedBox2 corner_box = aab_center_dim(corner, vec2(0.25f, 0.25f));
        push_shape(&game_state->render_context, t, rectangle(corner_box), corner_color, ShapeRenderMode_Fill, 1000.0f);

        if (is_in_aab(offset(corner_box, t.offset), editor->world_mouse_p)) {
            drag_region->scaling = vec2(-1.0f, -1.0f);
            editor->next_hot_widget = widget;
        }
    }

    {
        v2 corner = 0.5f*vec2(zone.x, -zone.y);
        AxisAlignedBox2 corner_box = aab_center_dim(corner, vec2(0.25f, 0.25f));
        push_shape(&game_state->render_context, t, rectangle(corner_box), corner_color, ShapeRenderMode_Fill, 1000.0f);

        if (is_in_aab(offset(corner_box, t.offset), editor->world_mouse_p)) {
            drag_region->scaling = vec2(1.0f, -1.0f);
            editor->next_hot_widget = widget;
        }
    }

    {
        v2 corner = 0.5f*vec2(zone.x, zone.y);
        AxisAlignedBox2 corner_box = aab_center_dim(corner, vec2(0.25f, 0.25f));
        push_shape(&game_state->render_context, t, rectangle(corner_box), corner_color, ShapeRenderMode_Fill, 1000.0f);

        if (is_in_aab(offset(corner_box, t.offset), editor->world_mouse_p)) {
            drag_region->scaling = vec2(1.0f, 1.0f);
            editor->next_hot_widget = widget;
        }
    }

    {
        v2 corner = 0.5f*vec2(-zone.x, zone.y);
        AxisAlignedBox2 corner_box = aab_center_dim(corner, vec2(0.25f, 0.25f));
        push_shape(&game_state->render_context, t, rectangle(corner_box), corner_color, ShapeRenderMode_Fill, 1000.0f);

        if (is_in_aab(offset(corner_box, t.offset), editor->world_mouse_p)) {
            drag_region->scaling = vec2(-1.0f, 1.0f);
            editor->next_hot_widget = widget;
        }
    }

    return widget;
}

internal void execute_console(GameState* game_state, ConsoleState* console, GameInput* input) {
    RenderContext* rc = &console->rc;

    v2 mouse_p = vec2(input->mouse_x, input->mouse_y);

    if (was_pressed(input->tilde)) {
        console->open = !console->open;
        input->event_mode = console->open;
    }

    render_screenspace(rc);

    UILayoutContext layout_context;
    layout_context.rc         = rc;
    layout_context.assets     = &game_state->assets;
    layout_context.temp_arena = &game_state->transient_arena;
    layout_context.font       = game_state->console_font;

    v2 dim = get_screen_dim(rc);
    f32 width = dim.x;
    f32 height = dim.y;

    f32 console_input_box_height = cast(f32) game_state->console_font->size;
    f32 console_open_speed = 0.1f;
    f32 console_close_speed = 0.04f;
    f32 console_height = dim.y / 4.0f;

    if (console->open) {
        if (console->openness_t < 1.0f) {
            console->openness_t += input->frame_dt / console_open_speed;
        } else {
            console->openness_t = 1.0f;
        }
    } else {
        if (console->openness_t > 0.0f) {
            console->openness_t -= input->frame_dt / console_close_speed;
        } else {
            console->openness_t = 0.0f;
        }
    }

    u32 warnings, errors;
    platform.get_unread_log_messages(0, &warnings, &errors);
    UILayout message_counter = make_layout(layout_context, vec2(width - 50.0f, height - console_height*console->openness_t - 2.0f*console_input_box_height*console->openness_t - get_baseline_from_top(game_state->console_font)));
    if (warnings) {
        layout_print(&message_counter, vec4(1.0f, 0.5f, 0.0f, 1.0f), "%u ", warnings);
    }
    if (errors) {
        layout_print(&message_counter, COLOR_RED, "%u", errors);
    }
    if (warnings || errors) {
        layout_finish_print(&message_counter);
    }

    if (console->openness_t > 0.0f) {
        for (u32 event_index = 0; event_index < input->event_count; event_index++) {
            char ascii;
            PlatformKeyCode code = decode_input_event(input->event_buffer[event_index], &ascii);
            switch (code) {
                case PKC_Escape: {
                    if (console->input_buffer_count > 0) {
                        console->input_buffer_count = 0;
                    } else {
                        console->open = false;
                        input->event_mode = false;
                    }
                } break;

                case PKC_Tab: {
                    for (u32 candidate_index = 0; candidate_index < ARRAY_COUNT(console_commands); candidate_index++) {
                        String candidate = console_commands[candidate_index].name;
                        if (find_match(candidate, input_buffer_as_string(console), StringMatch_CaseInsenitive).len) {
                            console->input_buffer_count = cast(u32) candidate.len;
                            copy(console->input_buffer_count, candidate.data, console->input_buffer);
                            console->input_buffer[console->input_buffer_count++] = ' ';
                            break;
                        }
                    }
                } break;

                case PKC_Return: {
                    if (console->input_buffer_count > 0) {
                        execute_console_command(game_state, input, input_buffer_as_string(console));
                        console->input_buffer_count = 0;
                    }
                } break;

                case PKC_Back: {
                    if (console->input_buffer_count > 0) {
                        console->input_buffer_count--;
                    }
                } break;

                case PKC_Oem3: {
                    /* Just eat this one silently to avoid leaving a ` or ~ in the console when closing */
                } break;

                case PKC_Delete: {
                    console->input_buffer_count = 0;
                } break;

                default: {
                    if (ascii) {
                        if (console->input_buffer_count + 1 < ARRAY_COUNT(console->input_buffer)) {
                            console->input_buffer[console->input_buffer_count++] = ascii;
                        }
                    }
                } break;
            }
        }

        // @TODO: Should have some kind of nicer way of handling sorting
        f32 old_sort_bias = rc->sort_key_bias;
        rc->sort_key_bias += 50000.0f;

        f32 console_log_box_height = cast(f32) height - (console_height + console_input_box_height)*console->openness_t;
        AxisAlignedBox2 console_log_box = aab_min_max(vec2(0.0f, console_log_box_height), vec2(width, height));
        push_shape(rc, default_transform2d(), rectangle(console_log_box), vec4(0.0f, 0.0f, 0.0f, 0.65f));

        PlatformLogMessage* selected_message = 0;

        UILayout log = make_layout(layout_context, vec2(4.0f, console_log_box_height + get_line_spacing(game_state->console_font) + get_baseline(game_state->console_font)), true);
        for (PlatformLogMessage* message = platform.get_most_recent_log_message(); message; message = message->next) {
            if (log.at_p.y < height) {
                b32 filter_match = false;
                if (console->input_buffer_count > 1 && console->input_buffer[0] == '/') {
                    String filter_string = wrap_string(console->input_buffer_count - 1, console->input_buffer + 1);
                    String match = find_match(wrap_string(message->text_length, message->text), filter_string, StringMatch_CaseInsenitive);
                    if (!match.len) match = find_match(wrap_cstr(message->file), filter_string, StringMatch_CaseInsenitive);
                    if (!match.len) match = find_match(wrap_cstr(message->function), filter_string, StringMatch_CaseInsenitive);

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
                layout_print_line(&log, color, message->text);

                if (is_in_aab(log.last_print_bounds, mouse_p)) {
                    selected_message = message;
                }
            } else {
                break;
            }
        }

        v2 input_box_min = vec2(0.0f, console_log_box_height - console_input_box_height);
        AxisAlignedBox2 console_input_box = aab_min_max(input_box_min, vec2(cast(f32) width, console_log_box_height));
        push_shape(rc, default_transform2d(), rectangle(console_input_box), vec4(0.0f, 0.0f, 0.0f, 0.8f));

        UILayout input_box = make_layout(layout_context, input_box_min + vec2(4.0f, get_line_spacing(game_state->console_font) + get_baseline(game_state->console_font)));
        layout_print_line(&input_box, COLOR_WHITE, "%.*s", console->input_buffer_count, console->input_buffer);

        if (selected_message) {
            UILayout log_tooltip = make_layout(layout_context, mouse_p + vec2(0.0f, 12.0f), true);
            rc->sort_key_bias += 100.0f;
            layout_print_line(&log_tooltip, COLOR_WHITE, "%s:%s:%u", selected_message->file, selected_message->function, selected_message->line);
            push_shape(rc, default_transform2d(), rectangle(log_tooltip.last_print_bounds), vec4(0.0f, 0.0f, 0.0f, 0.8f), ShapeRenderMode_Fill, -10.0f);
        }

        rc->sort_key_bias = old_sort_bias;
    }
}


inline EditorState* allocate_editor(GameState* game_state, GameRenderCommands* render_commands) {
    EditorState* editor = push_struct(&game_state->permanent_arena, EditorState);
    editor->game_state = game_state;

    initialize_render_context(&editor->render_context, render_commands, 0.0f);
    editor->render_context.sort_key_bias = 32000.0f;

    editor->assets = &game_state->assets;
    editor->arena = &game_state->transient_arena;

    editor->shown = false;
    editor->show_statistics = false;

    editor->camera_icon     = get_image_id_by_name(editor->assets, string_literal("camera_icon"));
    editor->speaker_icon    = get_image_id_by_name(editor->assets, string_literal("speaker_icon"));
    editor->checkpoint_icon = get_image_id_by_name(editor->assets, string_literal("checkpoint_icon"));

    editor->big_font = get_font_by_name(editor->assets, string_literal("editor_font_big"));
    editor->font = get_font_by_name(editor->assets, string_literal("editor_font"));

    editor->default_collision = vec2(1, 1);

    editor->top_margin  = cast(f32) render_commands->height - get_baseline_from_top(editor->font);
    editor->left_margin = 4.0f;

    editor->grid_snapping_enabled = true;
    editor->grid_size = vec2(0.5f, 0.5f);

    editor->zoom = 1.0f;

    set_up_editable_parameters(editor);

    if (game_state->active_level->entity_count <= 1) {
        create_debug_level(editor);
    }

    return editor;
}

internal void execute_editor(GameState* game_state, EditorState* editor, GameInput* input, DebugFrameTimeHistory* frame_history) {
    u32 width = editor->render_context.commands->width;
    u32 height = editor->render_context.commands->height;

    render_screenspace(&editor->render_context);
    editor->top_margin  = cast(f32) height - get_baseline_from_top(editor->font);
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

        game_state->render_context.vertical_fov /= editor->zoom;
    }

    v2 mouse_p = vec2(input->mouse_x, input->mouse_y);
    v2 world_mouse_p = screen_to_world(&game_state->render_context, transform2d(mouse_p)).offset;

    editor->mouse_p = mouse_p;
    editor->world_mouse_p = world_mouse_p;

    if (!editor->panning) {
        if (input->space.is_down && was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
            editor->panning = true;
            editor->world_mouse_p_on_pan = world_mouse_p;
            editor->camera_p_on_pan = game_state->render_context.camera_p;
        }
    } else {
        game_state->render_context.camera_p = editor->camera_p_on_pan;
        v2 pan_world_mouse_p = screen_to_world(&game_state->render_context, transform2d(mouse_p)).offset;
        game_state->render_context.camera_p += editor->world_mouse_p_on_pan - pan_world_mouse_p;
        // @TODO: The hackyness of this here suggests these kinds of actions should use widgets too.
        if (!input->mouse_buttons[PlatformMouseButton_Left].is_down && !was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
            editor->panning = false;
        }
    }

    UILayout layout = make_layout(editor, vec2(4.0f, editor->top_margin));

    if (editor->shown || editor->show_statistics) {
        f32 average_frame_time = 0.0f;
        for (u32 frame_index = 0; frame_index < frame_history->valid_entry_count; frame_index++) {
            average_frame_time += frame_history->history[(frame_history->first_valid_entry + frame_index) % ARRAY_COUNT(frame_history->history)];
        }
        average_frame_time /= frame_history->valid_entry_count;

        f32 average_frame_time_in_ms = 1000.0f*average_frame_time;
        f32 frame_target_miss_amount_in_ms = clamp01(1000.0f*(average_frame_time - input->frame_dt));
        v4 timer_color = vec4(1.0f, 1.0f-frame_target_miss_amount_in_ms, 1.0f-frame_target_miss_amount_in_ms, 1.0f);
        layout_print_line(&layout, timer_color, "Average Frame Time: %fms (target update rate: %ghz)\n", average_frame_time_in_ms, input->update_rate);
    }

    if (!editor->shown) {
        return;
    }

    Level* level = editor->game_state->active_level;

    layout_print_line(&layout, COLOR_WHITE, "Camera Position: { %f, %f }", game_state->render_context.camera_p.x, game_state->render_context.camera_p.y);

    layout_print(&layout, COLOR_WHITE, "Editing level '%.*s'", cast(int) level->name_length, level->name);
    if (editor->level_saved_timer > 0.0f) {
        layout_print(&layout, vec4(1.0f, 1.0f, 1.0f, clamp01(editor->level_saved_timer)), "  (Saved...)");
    }
    layout_finish_print(&layout);

    layout_print_line(&layout, COLOR_WHITE, "Hot Widget: %s", widget_name(editor->hot_widget));
    layout_print_line(&layout, COLOR_WHITE, "Active Widget: %s", widget_name(editor->active_widget));
    layout_print_line(&layout, COLOR_WHITE, "Last Activated Checkpoint: EntityID { %d }", game_state->last_activated_checkpoint ? game_state->last_activated_checkpoint->guid.value : 0);

    if (editor->level_saved_timer > 0.0f) {
        editor->level_saved_timer -= input->frame_dt;
    }

    if (was_pressed(get_key(input, 'S'))) {
        if (input->ctrl.is_down) {
            write_level_to_disk(game_state, editor->game_state->active_level, wrap_string(editor->game_state->active_level->name_length, editor->game_state->active_level->name));
            editor->level_saved_timer = 2.0f;
        } else {
            editor->grid_snapping_enabled = !editor->grid_snapping_enabled;
        }
    }

    UILayout status_bar = make_layout(editor, vec2(4.0f, get_line_spacing(editor->font) + get_baseline(editor->font)), true);
    layout_print_line(&status_bar, COLOR_WHITE, "Entity Count: %u/%u | Grid Snapping: %s, Grid Size: { %f, %f } | Zoom: %gx",
        level->entity_count, ARRAY_COUNT(level->entities),
        editor->grid_snapping_enabled ? "true" : "false", editor->grid_size.x, editor->grid_size.y,
        editor->zoom
    );

    UILayout undo_log = make_layout(editor, vec2(500.0f, editor->top_margin));

    u32 undo_buffer_location = editor->undo_most_recent ? editor->undo_most_recent + sizeof(UndoFooter) : 0;
    u32 undo_buffer_size = ARRAY_COUNT(editor->undo_buffer);
    layout_print_line(&undo_log, COLOR_WHITE, "Undo Buffer Location: %dKB/%dKB", undo_buffer_location / 1024, undo_buffer_size / 1024);

    undo_log.depth++;

    u32 undo_log_count = 0;
    for (u32 footer_index = editor->undo_most_recent; undo_log_count < 5;) {
        UndoFooter* footer = get_undo_footer(editor, footer_index);

        if (footer) {
            void* data = get_undo_data(footer);

            v4 color = vec4(0.85f, 0.85f, 0.85f, 1.0f - (cast(f32) undo_log_count / 4.5f));

            layout_print(&undo_log, color, "[%04u] %s: ", footer_index, enum_name_safe(UndoType, footer->type));
            switch (footer->type) {
                case Undo_SetEntityData: {
                    layout_print(&undo_log, color, "EntityID { %u } ", footer->entity_guid.value);
                    if (footer->description) {
                        layout_print(&undo_log, color, "-> %s", footer->description);
                    }
                } break;
                case Undo_SetData: {
                    if (footer->description) {
                        layout_print(&undo_log, color, "%s", footer->description);
                    } else {
                        layout_print(&undo_log, color, "0x%I64x", cast(u64) footer->data_ptr);
                    }
                } break;
                case Undo_CreateEntity:
                case Undo_DeleteEntity: {
                    EntityID id = (cast(Entity*) data)->guid;
                    layout_print(&undo_log, color, "EntityID { %d }", id.value);
                } break;
            }
            layout_finish_print(&undo_log);

            undo_log_count++;

            footer_index = footer->prev;
        } else {
            break;
        }
    }

    Entity* moused_over = 0;
    if (game_state->game_mode == GameMode_Ingame) {
        for (u32 entity_index = 1; entity_index < game_state->entity_count; entity_index++) {
            Entity* entity = game_state->entities + entity_index;

            if (!(entity->flags & EntityFlag_Invisible) || editor->shown) {
                if (is_in_region(entity->collision, world_mouse_p - entity->p)) {
                    moused_over = entity;
                }
            }
        }
    }

    if (!moused_over) {
        for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
            Entity* entity = level->entities + entity_index;

            if (is_in_region(entity->collision, world_mouse_p - entity->p)) {
                moused_over = entity;
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

    if (moused_over) {
        manipulate_entity.manipulate.guid = moused_over->guid;
        editor->next_hot_widget = manipulate_entity;
    } else if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
        if (editor->active_widget.type == Widget_None && !editor->panning) {
            editor->selected_entity = { 0 };
        }
    }

    if (was_pressed(input->shift)) {
        editor->spawn_menu_p = mouse_p;
    }

    if (input->shift.is_down) {
        UILayout spawn_menu = make_layout(editor, editor->spawn_menu_p);

        layout_print_line(&spawn_menu, COLOR_WHITE, "Spawn Entity:");
        spawn_menu.depth++;

        EntityType highlighted_type = EntityType_Null;
        EntityPrefab highlighted_prefab = EntityPrefab_Null;
        for (u32 entity_type_id = cast(u32) EntityType_Null + 1; entity_type_id < EntityType_Count; entity_type_id++) {
            EntityType entity_type = cast(EntityType) entity_type_id;

            v4 color = (editor->type_to_spawn == entity_type) ? vec4(1, 0, 1, 1) : vec4(1, 1, 1, 1);
            char* entity_name = enum_name(EntityType, entity_type);
            if (entity_name) {
                entity_name += sizeof("EntityType_") - 1;

                layout_print_line(&spawn_menu, color, entity_name);

                if (is_in_aab(spawn_menu.last_print_bounds, mouse_p)) {
                    highlighted_type = entity_type;
                }

                if (editor->entity_prefabs[entity_type] && editor->entity_prefabs[entity_type]->count) {
                    spawn_menu.depth++;
                    for (u32 prefab_index = 0; prefab_index < editor->entity_prefabs[entity_type]->count; prefab_index++) {
                        EntityPrefab prefab = editor->entity_prefabs[entity_type]->data[prefab_index];
                        char* prefab_name = enum_name(EntityPrefab, prefab) + sizeof("EntityPrefab_") - 1;

                        color = (editor->prefab_to_spawn == prefab) ? vec4(1, 0, 1, 1) : vec4(1, 1, 1, 1);
                        layout_print_line(&spawn_menu, color, prefab_name);

                        if (is_in_aab(spawn_menu.last_print_bounds, mouse_p)) {
                            highlighted_type = entity_type;
                            highlighted_prefab = prefab;
                        }
                    }
                    spawn_menu.depth--;
                }
            }
        }
        editor->type_to_spawn = highlighted_type;
        editor->prefab_to_spawn = highlighted_prefab;
    } else if (was_released(input->shift)) {
        Entity* created_entity = 0;
        switch (editor->type_to_spawn) {
            case EntityType_Player: {
                created_entity = add_player(editor, snap_to_grid(editor, world_mouse_p)).ptr;
            } break;

            case EntityType_Wall: {
                created_entity = add_wall(editor, aab_center_dim(snap_to_grid(editor, world_mouse_p), vec2(2, 2))).ptr;
                if (editor->prefab_to_spawn == EntityPrefab_Hazard || editor->prefab_to_spawn == EntityPrefab_InvisibleHazard) {
                    created_entity->flags |= EntityFlag_Hazard;
                    created_entity->color = COLOR_RED;
                }
                if (editor->prefab_to_spawn == EntityPrefab_InvisibleHazard) {
                    created_entity->flags |= EntityFlag_Invisible;
                    created_entity->color = vec4(COLOR_RED.rgb, 0.25f);
                }
            } break;

            case EntityType_CameraZone: {
                created_entity = add_camera_zone(editor, aab_center_dim(snap_to_grid(editor, world_mouse_p), vec2(35.0f, 15.0f)), 15.0f).ptr;
            } break;

            case EntityType_Checkpoint: {
                created_entity = add_checkpoint(editor, aab_center_dim(snap_to_grid(editor, world_mouse_p), vec2(4.0f, 6.0f))).ptr;
            } break;

            case EntityType_SoundtrackPlayer: {
                created_entity = add_soundtrack_player(editor, aab_center_dim(snap_to_grid(editor, world_mouse_p), vec2(20.0f, 10.0f)), SoundtrackID { 0 }).ptr;
            } break;
        }

        if (created_entity) {
            add_undo_history(editor, Undo_CreateEntity, sizeof(*created_entity), created_entity);
        }

        editor->type_to_spawn = EntityType_Null;
    }

    Entity* selected = 0;
    if (editor->selected_entity.value) {
        selected = get_entity_from_guid(editor, editor->selected_entity);
    } else {
        selected = moused_over;
    }

    if (selected) {
#if 0
        Entity* full_entity = 0;
        if (game_state->game_mode == GameMode_Ingame) {
        }
#endif

        if (game_state->game_mode == GameMode_Editor) {
            if (selected->type == EntityType_CameraZone) {
                drag_region_widget(game_state, editor, selected->p, wrap_entity_data(selected, &selected->active_region));
            } else if (selected->type == EntityType_Checkpoint) {
                drag_region_widget(game_state, editor, selected->p, wrap_entity_data(selected, &selected->checkpoint_zone));
            } else if (selected->type == EntityType_Wall) {
                drag_region_widget(game_state, editor, selected->p, wrap_entity_data(selected, &selected->collision));

                if (selected->behaviour == WallBehaviour_Move) {
                    EditorWidget widget;
                    widget.guid = &widget;
                    widget.type = Widget_DragP;
                    widget.drag_p.original_p = selected->end_p;
                    widget.drag_p.drag_offset = world_mouse_p - selected->end_p;
                    widget.drag_p.target = wrap_entity_data(selected, &selected->end_p);

                    if (is_active(editor, widget)) {
                        v2* target = get_data(editor, editor->active_widget.drag_p.target);
                        v2 original_p = editor->active_widget.drag_p.original_p;
                        v2 drag_offset = editor->active_widget.drag_p.drag_offset;
                        *target = original_p + snap_to_grid(editor, world_mouse_p - editor->world_mouse_p_on_active);
                        if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
                            clear_active(editor);
                        }
                    } else if (is_hot(editor, widget)) {
                        if (was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
                            add_entity_data_undo_history(editor, editor->hot_widget.drag_p.target);
                            set_active(editor, editor->hot_widget);
                        }
                    }

                    v3 fill_color = is_active(editor, widget) ? COLOR_RED.rgb : selected->color.rgb;
                    v3 outline_color = is_active(editor, widget) || is_hot(editor, widget) ? COLOR_RED.rgb : selected->color.rgb;

                    push_shape(&game_state->render_context, transform2d(selected->end_p), rectangle(aab_center_dim(vec2(0, 0), selected->collision)), vec4(fill_color, 0.25f));
                    push_shape(&game_state->render_context, transform2d(selected->end_p), rectangle(aab_center_dim(vec2(0, 0), selected->collision)), vec4(outline_color, 0.85f), ShapeRenderMode_Outline);

                    if (selected->guid.value == editor->selected_entity.value) {
                        if (is_in_region(selected->collision, world_mouse_p - selected->end_p)) {
                            editor->next_hot_widget = widget;
                        }
                    }
                }
            } else if (selected->type == EntityType_SoundtrackPlayer) {
                drag_region_widget(game_state, editor, selected->p, wrap_entity_data(selected, &selected->audible_zone));
            }
        }

        layout_print_line(&layout, COLOR_WHITE, "");
        layout_print_line(&layout, COLOR_WHITE, "Entity (%d, %s)", selected->guid.value, enum_name_safe(EntityType, selected->type));

        layout.depth++;

        LinearBuffer<EditableParameter>* editables = editor->editable_parameter_info[selected->type];
        if (editables) {
            EditableParameter* highlighted_editable = 0;

            for (u32 editable_index = 0; editable_index < editables->count; editable_index++) {
                EditableParameter* editable = editables->data + editable_index;
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

                layout_print(&layout, color, "%s: ", editable->name);
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
                            new_value += delta;
                            if (editable->flags & Editable_RangeLimited) {
                                new_value = CLAMP(new_value, editable->e_u32.min_value, editable->e_u32.max_value);
                            }
                            *(cast(u32*) editable_ptr) = new_value;
                        } break;

                        case Editable_f32: {
                            f32 delta = snap_to_grid_y(editor, dot(mouse_p - start_mouse_p, vec2(0, 1)) / 8.0f);
                            f32 new_value = *(cast(f32*) &editor->active_widget.start_value);
                            new_value += delta;
                            if (editable->flags & Editable_RangeLimited) {
                                new_value = clamp(new_value, editable->e_f32.min_value, editable->e_f32.max_value);
                            }
                            *(cast(f32*) editable_ptr) = new_value;
                        } break;
                    }

                    if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
                        if (editable->type == Editable_WallBehaviour) {
                            WallBehaviour value = *(cast(WallBehaviour*) editable_ptr);
                            value = cast(WallBehaviour) (cast(u32) value + 1);
                            if (value >= WallBehaviour_Count) {
                                value = WallBehaviour_None;
                            }
                            *(cast(WallBehaviour*) editable_ptr) = value;
                        }
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
                            add_entity_data_undo_history_(editor, selected->guid, editable->offset, editable->size, editable->name);
                        }
                    }
                }

                if (is_in_aab(layout.last_print_bounds, mouse_p)) {
                    editor->next_hot_widget = widget;
                }

#if 0
                if (respective_entity) {
                    editable_ptr = cast(void**) (cast(u8*) respective_entity + editable->offset);
                    print_editable(&layout, editable, editable_ptr, vec4(0.7f, 0.7f, 0.7f, 1.0f));
                }
#endif

                layout_finish_print(&layout);
            }
        }

        if (was_pressed(input->del)) {
            delete_entity(editor, selected->guid);
            selected = 0;
        }
    }

    if (game_state->game_mode == GameMode_Editor) {
        if (moused_over) {
            push_shape(&game_state->render_context, transform2d(moused_over->p), rectangle(moused_over->collision), COLOR_PINK, ShapeRenderMode_Outline, 100.0f);
        }

        if (selected && selected != moused_over) {
            push_shape(&game_state->render_context, transform2d(selected->p), rectangle(selected->collision), COLOR_GREEN, ShapeRenderMode_Outline, 100.0f);
        }
    }

    if (editor->active_widget.type) {
        switch (editor->active_widget.type) {
            case Widget_ManipulateEntity: {
                EditorWidget* widget = &editor->active_widget;
                switch (widget->manipulate.type) {
                    case Manipulate_Default: {
                        f32 drag_distance = length(mouse_p - editor->mouse_p_on_active);
                        if (input->mouse_buttons[PlatformMouseButton_Left].is_down && drag_distance > 5.0f) {
                            editor->selected_entity = widget->manipulate.guid;
                            widget->manipulate.type = Manipulate_DragEntity;
                            Entity* entity = get_entity_from_guid(editor, widget->manipulate.guid); // @WidgetEntityData
                            widget->manipulate.original_p = entity->p;
                            add_entity_data_undo_history(editor, wrap_entity_data(entity, &entity->p));
                        } else if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
                            if (moused_over) {
                                editor->selected_entity = moused_over->guid;
                            }
                            clear_active(editor);
                        }
                    } break;

                    case Manipulate_DragEntity: {
                        Entity* entity = get_entity_from_guid(editor, widget->manipulate.guid); // @WidgetEntityData
                        entity->p = widget->manipulate.original_p + snap_to_grid(editor, world_mouse_p - editor->world_mouse_p_on_active);
                        if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
                            editor->selected_entity = { 0 };
                            clear_active(editor);
                        } else if (was_released(input->mouse_buttons[PlatformMouseButton_Right])) {
                            editor->selected_entity = { 0 };
                            clear_active(editor);
                            undo(editor);
                        }
                    } break;

                    case Manipulate_DeleteEntity: {
                        delete_entity(editor, widget->manipulate.guid);
                        clear_active(editor);
                    } break;
                }
            } break;

            case Widget_DragRegion: {
                EditorWidgetDragRegion* drag_region = &editor->active_widget.drag_region;
                Entity* entity = get_entity_from_guid(editor, drag_region->target.guid);
                v2* target = get_data(editor, drag_region->target);

                v2 mouse_delta = world_mouse_p - editor->world_mouse_p_on_active;
                *target = drag_region->original + snap_to_grid(editor, drag_region->scaling*mouse_delta);
                if (input->ctrl.is_down) {
                    f32 aspect_ratio = drag_region->original.x / drag_region->original.y;
                    target->x = aspect_ratio*target->y;
                }
                entity->p = drag_region->original_p + 0.5f*snap_to_grid(editor, mouse_delta);
                if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
                    clear_active(editor);
                }
            } break;
        }
    } else if (editor->hot_widget.type) {
        switch (editor->hot_widget.type) {
            case Widget_ManipulateEntity: {
                EditorWidget widget = editor->hot_widget;
                if (input->del.is_down && input->mouse_buttons[PlatformMouseButton_Left].is_down) {
                    widget.manipulate.type = Manipulate_DeleteEntity;
                    set_active(editor, widget);
                } else if (was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
                    Entity* entity = get_entity_from_guid(editor, widget.manipulate.guid); // @WidgetEntityData
                    widget.manipulate.drag_offset = world_mouse_p - entity->p;
                    set_active(editor, widget);
                }
            } break;

            case Widget_DragRegion: {
                EditorWidgetDragRegion* drag_region = &editor->hot_widget.drag_region;
                if (was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
                    add_entity_data_undo_history(editor, drag_region->target);
                    set_active(editor, editor->hot_widget);
                }
            } break;
        }
    }

    editor->hot_widget = editor->next_hot_widget;
    editor->next_hot_widget = {};
}
