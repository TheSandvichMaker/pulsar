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
    zero_struct(*entity);

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
    entity->collision = aab_min_max(vec2(-0.2f, -0.5f), vec2(0.2f, 0.5f));
    entity->color = vec4(1, 1, 1, 1);

    entity->flags |= EntityFlag_Physical|EntityFlag_Collides;

    return result;
}

inline AddEntityResult add_wall(EditorState* editor, AxisAlignedBox2 aab, b32 deadly_wall = false) {
    AddEntityResult result = add_entity(editor, EntityType_Wall);
    Entity* entity = result.ptr;

    entity->p = get_center(aab);
    entity->color = vec4(1, 1, 1, 1);
    entity->surface_friction = 5.0f;
    entity->midi_test_target = entity->p;

    entity->flags |= EntityFlag_Collides;
    if (deadly_wall) {
        entity->flags |= EntityFlag_Hazard;
    }

    aab = offset(aab, -entity->p);

    entity->collision = aab;

    return result;
}

inline AddEntityResult add_soundtrack_player(EditorState* editor, SoundtrackID soundtrack_id, u32 playback_flags = Playback_Looping) {
    AddEntityResult result = add_entity(editor, EntityType_SoundtrackPlayer);
    Entity* entity = result.ptr;

    entity->soundtrack_id  = soundtrack_id;
    entity->playback_flags = playback_flags;
    entity->sprite = editor->speaker_icon;

    entity->collision = editor->default_collision;

    entity->flags |= EntityFlag_Invisible;

    return result;
}

inline AddEntityResult add_camera_zone(EditorState* editor, AxisAlignedBox2 zone, f32 rotation = 0.0f) {
    AddEntityResult result = add_entity(editor, EntityType_CameraZone);
    Entity* entity = result.ptr;

    entity->p = get_center(zone);
    entity->camera_zone = get_dim(zone);

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
        Level* level = editor->active_level;

        assert(hash_slot->index < ARRAY_COUNT(level->entities));
        Entity* entity = level->entities + hash_slot->index;

        if (editor->selected_entity && editor->selected_entity->guid.value == guid.value) {
            editor->selected_entity = 0;
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

// @TODO: Is add_undo_history overly complex?
// Here's how I would simplify it:
// Have two ring buffers instead of one, one for undo and one for redo
// RingBuffer undo_buffer[UNDO_BUFFER_SIZE];
// RingBuffer redo_buffer[UNDO_BUFFER_SIZE];
// Every time you read from the undo buffer, you write the respective redo data to the redo buffer,
//  and every time you read from the redo buffer, you write the respective undo data to the undo buffer.
// This would get you the same functionality for twice the memory profile, but it would be tremendously
//  more simple and would be able to use a generic ring buffer implementation for both buffers.
// The ring buffer would look like this:
// struct RingBuffer {
//     size_t size;
//     u8* buffer;
//
//     size_t write_cursor;
//     size_t read_cursor;
// };
inline void add_undo_history(EditorState* editor, UndoType type, u32 data_size, void* data, char* description) {
    assert(data_size + sizeof(UndoFooter) <= sizeof(editor->undo_buffer));

    UndoFooter* prev_footer = 0;
    u32 data_index = 0;
    if (editor->undo_most_recent) {
        prev_footer = get_undo_footer(editor, editor->undo_most_recent);
        data_index = editor->undo_most_recent + sizeof(UndoFooter);
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
    footer->type = type;
    footer->description = description;
    footer->data_size = data_size;
    footer->data_ptr = data;

    copy(footer->data_size, footer->data_ptr, editor->undo_buffer + data_index);

    if (prev_footer) {
        prev_footer->next = footer_index;
    }
    footer->prev = editor->undo_most_recent;

    editor->undo_most_recent = footer_index;
}

// @TODO: SetData is insufficient for operations on entities since they move around
// in the entity array. Make an Undo_SetEntityData or something, and/or ponder the
// way undo should work in general in regard to entities.

inline void undo(EditorState* editor) {
    UndoFooter* footer = get_undo_footer(editor, editor->undo_most_recent);
    if (footer) {
        void* data = get_undo_data(footer);

        TemporaryMemory temp = begin_temporary_memory(editor->arena);
        switch (footer->type) {
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
    // @Note: Weird paranoid assert in case I ever change the hash from a fixed size array and forget to change this...
    assert(sizeof(editor->entity_hash) == sizeof(EntityHash)*MAX_ENTITY_COUNT);

    zero_size(sizeof(editor->entity_hash), editor->entity_hash);

    editor->active_level = level;
    for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
        Entity* entity = editor->active_level->entities + entity_index;
        EntityHash* hash = get_entity_hash_slot(editor, entity->guid);
        hash->guid = entity->guid;
        hash->index = { entity_index };
    }
}

inline void create_debug_level(EditorState* editor) {
    add_player(editor, vec2(-8.0f, 1.5f));

    SoundtrackID soundtrack_id = get_soundtrack_id_by_name(editor->assets, "track1_1");
    add_soundtrack_player(editor, soundtrack_id);

    add_wall(editor, aab_min_max(vec2(-35.0f, -0.75f), vec2(1.25f, 0.75f)));

    for (s32 i = 0; i < 13; i++) {
        Entity* wall = add_wall(editor, aab_center_dim(vec2(2.0f + 1.5f*i, 0.0f), vec2(1.5f, 1.5f)), (i / 4) % 2 == 1).ptr;
        wall->midi_note = 60 + i;
    }

    add_checkpoint(editor, aab_min_dim(vec2(-6.0f, 0.75f), vec2(4.0f, 8.0f)));

    u32 width = editor->render_context.commands->width;
    u32 height = editor->render_context.commands->height;

    f32 aspect_ratio = cast(f32) width / cast(f32) height;
    add_camera_zone(editor, aab_center_dim(vec2(0.0f, 2.0f), vec2(aspect_ratio*20.0f, 20.0f)));
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
                Image* glyph = get_image(editor->assets, glyph_id);
                layout->at_p = vec2(roundf(layout->at_p.x), roundf(layout->at_p.y));
                v2 p = layout->at_p + vec2(layout->depth*font->whitespace_width*4.0f, 0.0f);
                push_image(&editor->render_context, transform2d(p + vec2(1.0f, -1.0f), vec2(glyph->w, glyph->h)), glyph, vec4(0, 0, 0, 0.75f), 1000.0f);
                push_image(&editor->render_context, transform2d(p, vec2(glyph->w, glyph->h)), glyph, color, 1000.0f);

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

inline void editor_print(EditorLayout* layout, v4 color, char* format_string, ...) {
    va_list va_args;
    va_start(va_args, format_string);
    editor_print_va(layout, color, format_string, va_args);
    va_end(va_args);
}

internal AxisAlignedBox2 editor_finish_print(EditorLayout* layout) {
    assert(layout->print_initialized);

    AxisAlignedBox2 result = layout->last_print_bounds;
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
DECLARE_EDITABLE_TYPE_INFERENCER(AxisAlignedBox2)
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
        add_viewable(editables, Entity, dead, Editable_IsBool);
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

    editables = begin_editables(editor, EntityType_Checkpoint);
    {
        add_viewable(editables, Entity, guid);
        add_viewable(editables, Entity, p);
        add_viewable(editables, Entity, checkpoint_zone);
        add_viewable(editables, Entity, most_recent_player_position);
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

        case Editable_AxisAlignedBox2: {
            AxisAlignedBox2 value = *(cast(AxisAlignedBox2*) editable_ptr);
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

    initialize_render_context(&editor->render_context, render_commands, 0.0f);
    editor->render_context.sort_key_bias = 32000.0f;

    editor->assets = &game_state->assets;
    editor->arena = &game_state->transient_arena;

    editor->shown = true;

#if 0
    editor->camera_icon = get_image_by_name(editor->assets, "camera_icon");
    editor->speaker_icon = get_image_by_name(editor->assets, "speaker_icon");
    editor->checkpoint_icon = get_image_by_name(editor->assets, "checkpoint_icon");
#else
    for (u32 member_index = 0; member_index < members_count(EditorAssets); member_index++) {
        MemberDefinition member = members_of(EditorAssets)[member_index];
        void** member_ptr = member_ptr(editor->asset_dependencies, member);
        switch (member.type) {
            case member_type(Image): {
                assert(member.flags & MetaMemberFlag_IsPointer);
                *(cast(Image**) member_ptr) = get_image_by_name(editor->assets, member.name);
            } break;

            INVALID_DEFAULT_CASE;
        }
    }
#endif

    editor->big_font = get_font_by_name(editor->assets, "editor_font_big");
    editor->font = get_font_by_name(editor->assets, "editor_font");

    editor->default_collision = aab_center_dim(vec2(0, 0), vec2(1, 1));

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

internal void execute_editor(GameState* game_state, EditorState* editor, GameInput* input, DebugFrameTimeHistory* frame_history) {
    u32 width = editor->render_context.commands->width;
    u32 height = editor->render_context.commands->height;

    render_screenspace(&editor->render_context);
    editor->top_margin  = cast(f32) editor->render_context.commands->height - get_baseline_from_top(editor->font);
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

    if (!editor->shown) {
        return;
    }

    EditorLayout layout = make_layout(editor, vec2(4.0f, editor->top_margin));

    Level* level = editor->active_level;
    f32 average_frame_time = 0.0f;
    for (u32 frame_index = 0; frame_index < frame_history->valid_entry_count; frame_index++) {
        average_frame_time += frame_history->history[(frame_history->first_valid_entry + frame_index) % ARRAY_COUNT(frame_history->history)];
    }
    average_frame_time /= frame_history->valid_entry_count;

    f32 average_frame_time_in_ms = 1000.0f*average_frame_time;
    f32 frame_target_miss_amount_in_ms = clamp01(1000.0f*(average_frame_time - input->frame_dt));
    v4 timer_color = vec4(1.0f, 1.0f-frame_target_miss_amount_in_ms, 1.0f-frame_target_miss_amount_in_ms, 1.0f);
    editor_print_line(&layout, timer_color, "Average Frame Time: %fms\n", average_frame_time_in_ms);
    editor_print_line(&layout, COLOR_WHITE, "Editing level '%s'", level->name.data);
    editor_print_line(&layout, COLOR_WHITE, "Hot Widget: %s", widget_name(editor->hot_widget));
    editor_print_line(&layout, COLOR_WHITE, "Active Widget: %s", widget_name(editor->active_widget));
    editor_print_line(&layout, COLOR_WHITE, "Last Activated Checkpoint: EntityID { %d }", game_state->last_activated_checkpoint ? game_state->last_activated_checkpoint->guid.value : 0);

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

    u32 undo_buffer_location = editor->undo_most_recent ? editor->undo_most_recent + sizeof(UndoFooter) : 0;
    u32 undo_buffer_size = ARRAY_COUNT(editor->undo_buffer);
    editor_print_line(&undo_log, COLOR_WHITE, "Undo Buffer Location: %dKB/%dKB", undo_buffer_location / 1024, undo_buffer_size / 1024);

    undo_log.depth++;

    u32 undo_log_count = 0;
    for (u32 footer_index = editor->undo_most_recent; undo_log_count < 5;) {
        UndoFooter* footer = get_undo_footer(editor, footer_index);

        if (footer) {
            void* data = get_undo_data(footer);

            v4 color = vec4(0.85f, 0.85f, 0.85f, 1.0f - (cast(f32) undo_log_count / 5.0f));

            editor_print(&undo_log, color, "[%04u] %s: ", footer_index, enum_name(UndoType, footer->type));
            switch (footer->type) {
                case Undo_SetData: {
                    if (footer->description) {
                        editor_print(&undo_log, color, "%s", footer->description);
                    } else {
                        editor_print(&undo_log, color, "0x%I64x", cast(u64) footer->data_ptr);
                    }
                } break;
                case Undo_CreateEntity:
                case Undo_DeleteEntity: {
                    EntityID id = (cast(Entity*) data)->guid;
                    editor_print(&undo_log, color, "EntityID { %d }", id.value);
                } break;
            }
            editor_finish_print(&undo_log);

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
                if (is_in_aab(entity->collision, world_mouse_p - entity->p)) {
                    moused_over = entity;
                }
            }
        }
    }

    if (!moused_over) {
        for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
            Entity* entity = level->entities + entity_index;

            if (is_in_aab(entity->collision, world_mouse_p - entity->p)) {
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

        editor_print_line(&spawn_menu, COLOR_WHITE, "Spawn Entity:");
        spawn_menu.depth++;

        EntityType highlighted_type = EntityType_Null;
        for (u32 entity_type_id = cast(u32) EntityType_Null + 1; entity_type_id < EntityType_Count; entity_type_id++) {
            EntityType entity_type = cast(EntityType) entity_type_id;

            v4 color = (editor->type_to_spawn == entity_type) ? vec4(1, 0, 1, 1) : vec4(1, 1, 1, 1);
            char* entity_name = enum_name(EntityType, entity_type);
            entity_name += sizeof("EntityType_") - 1;

            editor_print_line(&spawn_menu, color, entity_name);

            if (is_in_aab(spawn_menu.last_print_bounds, mouse_p)) {
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
                created_entity = add_camera_zone(editor, aab_center_dim(snap_to_grid(editor, world_mouse_p), vec2(aspect_ratio*20.0f, 20.0f))).ptr;
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
            drag_camera_zone.type = Widget_DragV2;

            Transform2D t = transform2d(selected->p);
            v2 zone = selected->camera_zone;

            {
                EditorWidgetDragV2* drag_v2 = &drag_camera_zone.drag_v2;
                drag_v2->original = selected->camera_zone;
                drag_v2->target = &selected->camera_zone;

                v4 corner_color = is_hot(editor, drag_camera_zone) ? COLOR_RED : COLOR_YELLOW;

                {
                    v2 corner = 0.5f*vec2(-zone.x, -zone.y);
                    AxisAlignedBox2 corner_box = aab_center_dim(corner, vec2(0.25f, 0.25f));
                    push_shape(&game_state->render_context, t, rectangle(corner_box), corner_color, ShapeRenderMode_Fill, 1000.0f);

                    if (is_in_aab(offset(corner_box, selected->p), world_mouse_p)) {
                        drag_v2->scaling = vec2(-2.0f, -2.0f);
                        editor->next_hot_widget = drag_camera_zone;
                    }
                }

                {
                    v2 corner = 0.5f*vec2(zone.x, -zone.y);
                    AxisAlignedBox2 corner_box = aab_center_dim(corner, vec2(0.25f, 0.25f));
                    push_shape(&game_state->render_context, t, rectangle(corner_box), corner_color, ShapeRenderMode_Fill, 1000.0f);

                    if (is_in_aab(offset(corner_box, selected->p), world_mouse_p)) {
                        drag_v2->scaling = vec2(2.0f, -2.0f);
                        editor->next_hot_widget = drag_camera_zone;
                    }
                }

                {
                    v2 corner = 0.5f*vec2(zone.x, zone.y);
                    AxisAlignedBox2 corner_box = aab_center_dim(corner, vec2(0.25f, 0.25f));
                    push_shape(&game_state->render_context, t, rectangle(corner_box), corner_color, ShapeRenderMode_Fill, 1000.0f);

                    if (is_in_aab(offset(corner_box, selected->p), world_mouse_p)) {
                        drag_v2->scaling = vec2(2.0f, 2.0f);
                        editor->next_hot_widget = drag_camera_zone;
                    }
                }

                {
                    v2 corner = 0.5f*vec2(-zone.x, zone.y);
                    AxisAlignedBox2 corner_box = aab_center_dim(corner, vec2(0.25f, 0.25f));
                    push_shape(&game_state->render_context, t, rectangle(corner_box), corner_color, ShapeRenderMode_Fill, 1000.0f);

                    if (is_in_aab(offset(corner_box, selected->p), world_mouse_p)) {
                        drag_v2->scaling = vec2(-2.0f, 2.0f);
                        editor->next_hot_widget = drag_camera_zone;
                    }
                }
            }

            if (is_active(editor, drag_camera_zone)) {
                EditorWidgetDragV2* drag_v2 = &editor->active_widget.drag_v2;
                v2 mouse_delta = snap_to_grid(editor, world_mouse_p - editor->world_mouse_p_on_active);
                *drag_v2->target = drag_v2->original + mouse_delta*drag_v2->scaling;
                if (input->ctrl.is_down) {
                    f32 aspect_ratio = drag_v2->original.x / drag_v2->original.y;
                    drag_v2->target->x = aspect_ratio*drag_v2->target->y;
                }
                if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
                    clear_active(editor);
                }
            } else if (is_hot(editor, drag_camera_zone)) {
                EditorWidgetDragV2* drag_v2 = &editor->hot_widget.drag_v2;
                if (was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
                    add_undo_history(editor, Undo_SetData, sizeof(AxisAlignedBox2), drag_v2->target, "Change Camera Zone Size");
                    set_active(editor, editor->hot_widget);
                }
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

                if (is_in_aab(layout.last_print_bounds, mouse_p)) {
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

    if (game_state->game_mode == GameMode_Editor) {
        if (moused_over) {
            push_shape(&game_state->render_context, transform2d(moused_over->p), rectangle(moused_over->collision), vec4(1, 0, 1, 1), ShapeRenderMode_Outline, 100.0f);
        }

        if (selected && selected != moused_over) {
            push_shape(&game_state->render_context, transform2d(selected->p), rectangle(selected->collision), vec4(0, 1, 0, 1), ShapeRenderMode_Outline, 100.0f);
        }
    }

    editor->hot_widget = editor->next_hot_widget;
    editor->next_hot_widget = {};
}
