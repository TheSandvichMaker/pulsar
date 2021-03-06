// @TODO: Ponder the desirableness of EntityData<T>

/*
 * Editor controls:
 * Space + LMB     : Pan
 * Shift + W/D     : Zoom in / out
 * Shift + Q/E     : Increase / decrease grid size
 * S               : Toggle grip snapping
 * Ctrl + D        : Duplicate selected entities
 * Delete          : Delete selected entities
 * Shift (Hold)    : Show spawn menu
 * Shift (Release) : Spawn selected entity
 * Ctrl + Z        : Undo
 * Ctrl + R        : Redo
 * Ctrl + S        : Save level
 * M               : Toggle showing all move widgets
 *
 * Some editor actions you will want to undertake are only available as console commands:
 *     toggle_flag [flag]                     : Toggle the given flag on the selected entities
 *     set_soundtrack [soundtrack asset name] : Set soundtrack_id on selected SoundtrackPlayers and listening_to on selected Walls
 */

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
    u32 result = safe_truncate_u64_u32(sizeof(T));
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

    // @Note: Knuth
    u32 hash_value = (guid.value*(guid.value + 3)) % ARRAY_COUNT(editor->entity_hash);

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
    GameState* game_state = editor->game_state; // @DisentangleGameStateFromEditor

    Entity* result = 0;
    EntityHash* hash_slot = get_entity_hash_slot(editor, guid);
    if (hash_slot) {
        if (hash_slot->index && hash_slot->index < ARRAY_COUNT(editor->game_state->active_level->entities)) {
            if (game_state->game_mode == GameMode_Ingame) {
                result = game_state->entities + hash_slot->gamestate_index;
            } else {
                result = game_state->active_level->entities + hash_slot->index;
            }
        }
    }
    return result;
}

inline AddEntityResult add_entity(EditorState* editor, EntityType type, EntityID guid = { 0 }) {
    Level* level = editor->game_state->active_level;

    assert(level->entity_count < ARRAY_COUNT(level->entities));

    u32 index = level->entity_count++;
    Entity* entity = level->entities + index;
    *entity = {};

    if (guid.value) {
        entity->guid = guid;
    } else {
        entity->guid = { level->first_available_guid++ };
    }

    entity->type = type;
    entity->color = COLOR_WHITE;

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

    entity->p         = starting_p;

    entity->collision = vec2(0.4f, 1.0f);
    entity->flags    |= EntityFlag_Physical|EntityFlag_Collides;

    return result;
}

inline AddEntityResult add_wall(EditorState* editor, AxisAlignedBox2 aab, SoundtrackID listening_to = { 0 }) {
    AddEntityResult result = add_entity(editor, EntityType_Wall);
    Entity* entity = result.ptr;

    entity->p                 = get_center(aab);

    entity->listening_to      = listening_to;
    entity->movement_speed_ms = 200.0f;
    entity->end_p             = vec2(0, 5);

    entity->movement_t        = 1.0f;

    entity->collision         = get_dim(aab);
    entity->flags            |= EntityFlag_Collides|EntityFlag_TransfersRetainedDp;

    return result;
}

inline AddEntityResult add_soundtrack_player(EditorState* editor, AxisAlignedBox2 inner_zone, SoundtrackID soundtrack_id, u32 playback_flags = Playback_Looping) {
    AddEntityResult result = add_entity(editor, EntityType_SoundtrackPlayer);
    Entity* entity = result.ptr;

    entity->p              = get_center(inner_zone);
    entity->soundtrack_id  = soundtrack_id;
    entity->playback_flags = playback_flags;

    entity->audible_zone   = get_dim(inner_zone);

    entity->sprite         = editor->speaker_icon;
    entity->collision      = editor->default_collision;
    entity->flags         |= EntityFlag_Invisible;

    return result;
}

inline AddEntityResult add_camera_zone(EditorState* editor, AxisAlignedBox2 active_region, f32 view_region_height, f32 rotation = 0.0f) {
    AddEntityResult result = add_entity(editor, EntityType_CameraZone);
    Entity* entity = result.ptr;

    entity->p                   = get_center(active_region);

    entity->active_region       = get_dim(active_region);
    entity->view_region_height  = view_region_height;
    entity->camera_rotation_arm = arm2(rotation);

    entity->sprite              = editor->camera_icon;
    entity->collision           = editor->default_collision;
    entity->flags              |= EntityFlag_Invisible;

    return result;
}

inline AddEntityResult add_checkpoint(EditorState* editor, AxisAlignedBox2 zone) {
    AddEntityResult result = add_entity(editor, EntityType_Checkpoint);
    Entity* entity = result.ptr;

    entity->p               = get_center(zone);

    entity->checkpoint_zone = get_dim(zone);

    entity->sprite          = editor->checkpoint_icon;
    entity->collision       = editor->default_collision;
    entity->flags          |= EntityFlag_Invisible;

    return result;
}

inline AddEntityResult add_trigger_zone(EditorState* editor, AxisAlignedBox2 zone) {
    AddEntityResult result = add_entity(editor, EntityType_TriggerZone);
    Entity* entity = result.ptr;

    entity->p               = get_center(zone);

    entity->trigger_zone = get_dim(zone);

    entity->sprite          = editor->trigger_icon;
    entity->collision       = editor->default_collision;
    entity->flags          |= EntityFlag_Invisible;

    return result;
}

inline void add_undo_history(EditorState* editor, UndoType type, u32 data_size, void* data, char* description = 0);
inline void delete_entity(EditorState* editor, EntityID guid, b32 with_undo_history = true) {
    EntityHash* hash_slot = get_entity_hash_slot(editor, guid);

    if (hash_slot) {
        Level* level = editor->game_state->active_level;

        assert(hash_slot->index < ARRAY_COUNT(level->entities));
        Entity* entity = level->entities + hash_slot->index;

        for (u32 selected_index = 0; selected_index < editor->selected_entity_count; ++selected_index) {
            EntityID selected = editor->selected_entities[selected_index];
            if (selected.value == guid.value) {
                editor->selected_entities[selected_index] = editor->selected_entities[--editor->selected_entity_count];
            }
        }

        if (with_undo_history) {
            add_undo_history(editor, Undo_DeleteEntity, sizeof(*entity), entity, 0);
        }

        hash_slot->guid = { 0 }; // Free the deleted entity's hash slot

        if (level->entity_count > 1) {
            level->entities[hash_slot->index] = level->entities[--level->entity_count];
            EntityHash* moved_entity_hash = get_entity_hash_slot(editor, level->entities[hash_slot->index].guid);
            moved_entity_hash->index = hash_slot->index;
        } else {
            level->entity_count--;
        }
    } else {
        log_print(LogLevel_Error, "Tried to delete non-existent EntityID { %u }", guid.value);
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

// @TODO: This is more complicated and hard to understand than I would like
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
    *footer = {};

    if (prev_footer) {
        prev_footer->next = footer_index;
    }
    footer->prev = editor->undo_most_recent;

    editor->undo_most_recent = footer_index;

    return footer;
}

inline void begin_undo_batch(EditorState* editor) {
    assert(!editor->doing_undo_batch);
    editor->doing_undo_batch = true;
    if (!editor->current_batch_id) {
        editor->current_batch_id = 1;
    }
};

inline void end_undo_batch(EditorState* editor) {
    assert(editor->doing_undo_batch);
    editor->doing_undo_batch = false;
    editor->current_batch_id++;
}

inline void add_undo_history(EditorState* editor, UndoType type, u32 data_size, void* data, char* description) {
    assert(data_size + sizeof(UndoFooter) <= sizeof(editor->undo_buffer));

    UndoFooter* footer = add_undo_footer(editor, data_size);

    footer->type = type;
    footer->description = description;
    footer->data_size = data_size;
    footer->data_ptr = data;

    if (editor->doing_undo_batch) {
        footer->batch_id = editor->current_batch_id;
    }

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

        if (editor->doing_undo_batch) {
            footer->batch_id = editor->current_batch_id;
        }

        copy(footer->data_size, cast(u8*) entity + footer->entity_data_offset, get_undo_data(footer));
    }
}

template <typename T>
inline void add_entity_data_undo_history(EditorState* editor, EntityData<T> entity_data, char* description = 0) {
    add_entity_data_undo_history_(editor, entity_data.guid, get_data_size(entity_data), entity_data.offset, description);
}

// @TODO: Think about deduplicating the symmetric undo/redo cases
inline void undo(EditorState* editor, u32 batch_chain_id = 0) {
    UndoFooter* footer = get_undo_footer(editor, editor->undo_most_recent);
    if (footer && (!batch_chain_id || footer->batch_id == batch_chain_id)) {
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

        if (footer->batch_id) {
            undo(editor, footer->batch_id);
        }
    }
}

inline void redo(EditorState* editor, u32 batch_chain_id = 0) {
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
        if (footer && (!batch_chain_id || footer->batch_id == batch_chain_id)) {
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

            if (footer->batch_id) {
                redo(editor, footer->batch_id);
            }
        }
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

    SoundtrackID soundtrack_id = get_soundtrack_id_by_name(editor->assets, string_literal("pulsar_kicktrack_1"));
    add_soundtrack_player(editor, aab_center_dim(vec2(-2.0f, 4.0f), vec2(20.0f, 10.0f)), soundtrack_id);

    add_wall(editor, aab_min_max(vec2(-35.0f, -1.0f), vec2(2.0f, 1.0f)));

    for (s32 i = 0; i < 13; i++) {
        Entity* wall = add_wall(editor, aab_center_dim(vec2(3.0f + 2.0f*i, 0.0f), vec2(2.0f, 2.0f))).ptr;
        wall->wall_behaviour = WallBehaviour_Move;
        wall->midi_note = 60 + i;
        wall->end_p = vec2(0, 1 + i);
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
            /* This is some nonsense input */
            num_bytes = 0;
        }

        if (num_bytes) {
            u32 offset = 6 * (num_bytes - 1);
            for (u32 byte_index = 0; byte_index < num_bytes; byte_index++) {
                if (byte_index == 0) {
                    codepoint = (*at & utf8_mask[num_bytes-1]) << offset;
                } else {
                    if (*at != 0) {
                        codepoint |= (*at & ((1 << 6) - 1)) << offset;
                    } else {
                        /* This is some nonsense input */
                        codepoint = 0;
                        break;
                    }
                }
                offset -= 6;
                at++;
            }
        }
    }

    *out_codepoint = codepoint;
    return num_bytes;
}

inline String push_formatted_string_va(MemoryArena* arena, char* format_string, va_list va_args) {
    u32 text_size = stbsp_vsnprintf(0, 0, format_string, va_args) + 1; // @Note: stbsp_vsprintf doesn't include the null terminator in the returned size, so I add it in.

    char* text = cast(char*) push_size(arena, text_size, no_clear());
    stbsp_vsnprintf(text, text_size, format_string, va_args);

    String result = wrap_string(text_size, text);
    return result;
}

enum LayoutTextOp {
    LayoutTextOp_GetBounds,
    LayoutTextOp_Print,
};

inline AxisAlignedBox2 layout_text_bounds(UILayout* layout, char* format_string, ...);
internal void layout_text_op_va(UILayout* layout, LayoutTextOp op, v4 color, char* format_string, va_list va_args) {
    TemporaryMemory temp = begin_temporary_memory(layout->context.temp_arena);

    String text = push_formatted_string_va(layout->context.temp_arena, format_string, va_args);

    Font* font = layout->font;

    v2 at_p = layout->at_p;
    v2 offset_p = layout->offset_p;
    if (op == LayoutTextOp_Print && (layout->flags & Layout_CenterAlign)) {
        v2 dim = get_dim(layout_text_bounds(layout, text.data));
        if (layout->flags & Layout_HorzCenterAlign) offset_p.x -= 0.5f*dim.x;
        if (layout->flags & Layout_VertCenterAlign) offset_p.y -= 0.5f*dim.y;
    }

    if (!layout->print_initialized) {
        layout->last_print_bounds = inverted_infinity_aab2();
        layout->print_initialized = true;
    } else {
        if (layout->last_codepoint && text.data[0]) {
            at_p.x += get_advance_for_codepoint_pair(font, layout->last_codepoint, text.data[0]);
        }
    }

    u32 last_codepoint = layout->last_codepoint;
    for (char* at = text.data; at[0]; at++) {
        last_codepoint = at[0];
        if (at[0] == ' ') {
            at_p.x += font->whitespace_width;
        } else if (at[0] == '\n') {
            at_p.x  = layout->origin.x;
            at_p.y += layout->vertical_advance;
        } else if (in_font_range(font, at[0])) {
            ImageID glyph_id = get_glyph_id_for_codepoint(font, at[0]);
            if (glyph_id.value) {
                Image* glyph = get_image(layout->context.assets, glyph_id);
                v2 p = vec2(roundf(offset_p.x + at_p.x), roundf(offset_p.y + at_p.y)) + vec2(layout->depth*font->whitespace_width*4.0f, 0.0f);

                if (op == LayoutTextOp_Print) {
                    if (glyph) {
                        push_image(layout->context.rc, transform2d(p + vec2(1.0f, -1.0f), vec2(glyph->w, glyph->h)), glyph, vec4(0, 0, 0, color.a));
                        push_image(layout->context.rc, transform2d(p, vec2(glyph->w, glyph->h)), glyph, color);
                    } else {
                        push_rect(layout->context.rc, aab_min_dim(p, vec2(font->size, font->size)), COLOR_RED);
                    }
                }

                AxisAlignedBox2 glyph_aab = {};
                if (glyph) {
                    glyph_aab = offset(get_aligned_image_aab(glyph), p);
                } else {
                    glyph_aab = aab_min_dim(vec2(0, 0), vec2(font->size, font->size));
                }

                layout->last_print_bounds = aab_union(layout->last_print_bounds, glyph_aab);

                if (in_font_range(font, at[1])) {
                    at_p.x += get_advance_for_codepoint_pair(font, at[0], at[1]);
                }
            }
        } else {
            // @TODO: Make it clear some unsupported characters showed up
        }
    }

    if (op == LayoutTextOp_GetBounds) {
        if (layout->flags & Layout_CenterAlign) {
            v2 dim = get_dim(layout->last_print_bounds);
            v2 the_offset = vec2(0, 0);
            if (layout->flags & Layout_HorzCenterAlign) the_offset.x -= 0.5f*dim.x;
            if (layout->flags & Layout_VertCenterAlign) the_offset.y -= 0.5f*dim.y;
            layout->last_print_bounds = offset(layout->last_print_bounds, the_offset);
        }
    } else {
        layout->at_p = at_p;
        layout->last_codepoint = last_codepoint;
        layout->total_bounds = aab_union(layout->total_bounds, layout->last_print_bounds);
    }

    end_temporary_memory(temp);
}

inline void layout_print_va(UILayout* layout, v4 color, char* format_string, va_list va_args) {
    layout_text_op_va(layout, LayoutTextOp_Print, color, format_string, va_args);
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

inline AxisAlignedBox2 layout_text_bounds_va(UILayout* layout, char* format_string, va_list va_args) {
    layout_text_op_va(layout, LayoutTextOp_GetBounds, COLOR_WHITE, format_string, va_args);
    AxisAlignedBox2 result = layout->last_print_bounds;
    layout->print_initialized = false;
    return result;
}

inline AxisAlignedBox2 layout_text_bounds(UILayout* layout, char* format_string, ...) {
    va_list va_args;
    va_start(va_args, format_string);
    AxisAlignedBox2 result = layout_text_bounds_va(layout, format_string, va_args);
    va_end(va_args);
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
DECLARE_EDITABLE_TYPE_INFERENCER(TriggerBehaviour)

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

inline void default_editables(LinearBuffer<EditableParameter>* editables) {
    add_viewable(editables, Entity, guid);
    EditableParameter* editable = add_viewable(editables, Entity, flags);
    editable->type = Editable_EntityFlag;
    add_viewable(editables, Entity, p);
}

internal void set_up_editable_parameters(EditorState* editor) {
    EditableParameter* editable = 0;
    LinearBuffer<EditableParameter>* editables = 0;

    editables = begin_editables(editor, EntityType_Player);
    {
        default_editables(editables);
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
        default_editables(editables);
        add_viewable(editables, Entity, collision);
        add_editable(editables, Entity, wall_behaviour);
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
        default_editables(editables);
        add_viewable(editables, Entity, active_region);
        add_editable(editables, Entity, view_region_height);
        add_editable(editables, Entity, primary_camera_zone, Editable_IsBool);
    }
    end_editables(editor, editables);

    editables = begin_editables(editor, EntityType_Checkpoint);
    {
        default_editables(editables);
        add_viewable(editables, Entity, checkpoint_zone);
        add_viewable(editables, Entity, respawn_p);
    }
    end_editables(editor, editables);

    editables = begin_editables(editor, EntityType_TriggerZone);
    {
        default_editables(editables);
        add_editable(editables, Entity, trigger_touch_behaviour);
        add_editable(editables, Entity, trigger_envelop_behaviour);
        add_editable(editables, Entity, trigger_leave_behaviour);
        add_viewable(editables, Entity, trigger_zone);
        add_viewable(editables, Entity, touching_player, Editable_IsBool);
        add_viewable(editables, Entity, enveloping_player, Editable_IsBool);
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

inline void print_editable(GameState* game_state, UILayout* layout, EditableParameter* editable, void** editable_ptr, v4 color) {
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
            if (id.value) {
                Soundtrack* soundtrack = get_soundtrack(&game_state->assets, id);
                if (soundtrack) {
                    Asset* asset = cast(Asset*) soundtrack;
                    layout_print(layout, color, ": '%.*s'", string_expand(asset->name));
                }
            }
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
            layout_print(layout, color, enum_name_safe(WallBehaviour, value));
        } break;

        case Editable_TriggerBehaviour: {
            TriggerBehaviour value = *(cast(TriggerBehaviour*) editable_ptr);
            layout_print(layout, color, enum_name_safe(TriggerBehaviour, value));
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
    b32 result = !editor->active_widget.type && widgets_equal(editor->hot_widget, widget);
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

inline EditorWidget drag_region_widget(GameState* game_state, EditorState* editor, v2 p, EntityData<v2> target) {
    EditorWidget widget = {};
    widget.guid = &widget;
    widget.type = Widget_DragRegion;
    widget.description = "Drag Region";

    Entity* entity = get_entity_from_guid(editor, target.guid);

    v2* region = get_data(editor, target);
    v2  region_value = *region;

    widget.drag_region.original   = region_value;
    widget.drag_region.original_p = entity->p;
    widget.drag_region.target     = target;

    v2 corner_box_size = vec2(0.25f, 0.25f) / editor->zoom;
    v4 corner_color = is_hot(editor, widget) || is_active(editor, widget) ? COLOR_RED : COLOR_YELLOW;

    Transform2D t = transform2d(p);

    {
        v2 corner = 0.5f*vec2(-region_value.x, -region_value.y);
        AxisAlignedBox2 corner_box = aab_center_dim(corner, corner_box_size);
        push_shape(&game_state->render_context, t, rectangle(corner_box), corner_color, ShapeRenderMode_Fill, 2000.0f);

        if (is_in_aab(offset(corner_box, t.offset), editor->world_mouse_p)) {
            widget.drag_region.scaling = vec2(-1.0f, -1.0f);
            editor->next_hot_widget = widget;
        }
    }

    {
        v2 corner = 0.5f*vec2(region_value.x, -region_value.y);
        AxisAlignedBox2 corner_box = aab_center_dim(corner, corner_box_size);
        push_shape(&game_state->render_context, t, rectangle(corner_box), corner_color, ShapeRenderMode_Fill, 2000.0f);

        if (is_in_aab(offset(corner_box, t.offset), editor->world_mouse_p)) {
            widget.drag_region.scaling = vec2(1.0f, -1.0f);
            editor->next_hot_widget = widget;
        }
    }

    {
        v2 corner = 0.5f*vec2(region_value.x, region_value.y);
        AxisAlignedBox2 corner_box = aab_center_dim(corner, corner_box_size);
        push_shape(&game_state->render_context, t, rectangle(corner_box), corner_color, ShapeRenderMode_Fill, 2000.0f);

        if (is_in_aab(offset(corner_box, t.offset), editor->world_mouse_p)) {
            widget.drag_region.scaling = vec2(1.0f, 1.0f);
            editor->next_hot_widget = widget;
        }
    }

    {
        v2 corner = 0.5f*vec2(-region_value.x, region_value.y);
        AxisAlignedBox2 corner_box = aab_center_dim(corner, corner_box_size);
        push_shape(&game_state->render_context, t, rectangle(corner_box), corner_color, ShapeRenderMode_Fill, 2000.0f);

        if (is_in_aab(offset(corner_box, t.offset), editor->world_mouse_p)) {
            widget.drag_region.scaling = vec2(-1.0f, 1.0f);
            editor->next_hot_widget = widget;
        }
    }

    return widget;
}

global EditorWidget pan_widget      = stateless_widget(&pan_widget, "Pan");
global EditorWidget select_entities = stateless_widget(&select_entities, "Entity Select");
global EditorWidget drag_entities   = stateless_widget(&drag_entities, "Entity Drag");

inline void activate_drag_entities(EditorState* editor) {
    editor->drag_entities_anchor = editor->world_mouse_p;
    editor->added_drag_entites_undo_history = false;
    set_active(editor, drag_entities);
}

inline void deselect_entities(EditorState* editor) {
    editor->selected_entity_count = 0;
    if (editor->active_widget.type == Widget_ManipulateEditable) {
        clear_active(editor);
    }
}

// @TODO: I'm sick of passing around things like GameInput
inline b32 editor_button(EditorState* editor, UILayout* layout, GameInput* input, char* title) {
    b32 result = false;
    v4 color = COLOR_WHITE;

    // @TODO: title isn't a very good guid for this widget, what if you want two buttons with the same name that do different things?
    EditorWidget button = stateless_widget(title, "Button");
    if (is_active(editor, button)) {
        color = COLOR_RED;
        if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
            result = true;
            clear_active(editor);
        }
    } else if (is_hot(editor, button)) {
        color = COLOR_YELLOW;
        if (was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
            set_active(editor, editor->hot_widget);
        }
    }

    layout_print_line(layout, color, "[%s]", title);

    if (is_in_aab(layout->last_print_bounds, editor->mouse_p)) {
        editor->next_hot_widget = button;
    }

    return result;
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
    editor->trigger_icon    = get_image_id_by_name(editor->assets, string_literal("trigger_icon"));

    editor->font = get_font_by_name(editor->assets, string_literal("editor_font"));

    editor->default_collision = vec2(1, 1);

    editor->top_margin  = cast(f32) render_commands->height - get_baseline_from_top(editor->font);
    editor->left_margin = 4.0f;

    editor->grid_snapping_enabled = true;
    editor->grid_size = vec2(0.5f, 0.5f);

    editor->show_camera_zones            = true;
    editor->show_soundtrack_player_zones = true;
    editor->show_checkpoint_zones        = true;
    editor->show_trigger_zones           = true;
    editor->zoom = 1.0f;

    set_up_editable_parameters(editor);

    return editor;
}

internal void execute_editor(GameState* game_state, EditorState* editor, GameInput* input) {
    u32 width = editor->render_context.commands->width;
    u32 height = editor->render_context.commands->height;

    render_screenspace(&editor->render_context);
    editor->top_margin  = cast(f32) height - get_baseline_from_top(editor->font);
    editor->left_margin = 4.0f;

    UILayout layout = make_layout(editor, vec2(4.0f, editor->top_margin));

    if (editor->shown || editor->show_statistics) {
        DebugFrameTimeHistory* frame_history = &global_debug_info.frame_history;
        f32 average_frame_time = 0.0f;
        f32 peak_frame_time = 0.0f;
        u32 average_render_commands = 0;
        u32 peak_render_commands = 0;
        // @TODO: be a good statistician and don't use a stupid average for this, but some cool gaussian or something
        for (u32 frame_index = 0; frame_index < frame_history->valid_entry_count; frame_index++) {
            DebugFrameInfo* frame = frame_history->history + ((frame_history->first_valid_entry + frame_index) % ARRAY_COUNT(frame_history->history));
            average_frame_time += frame->time;
            peak_frame_time = max(peak_frame_time, frame->time);
            average_render_commands += frame->render_commands;
            peak_render_commands = MAX(peak_render_commands, frame->render_commands);
        }
        average_frame_time /= frame_history->valid_entry_count;
        average_render_commands /= frame_history->valid_entry_count;

        f32 adjusted_frame_dt = input->frame_dt / game_config->simulation_rate; // @Note: This way the frame time counter won't go bright red if you lower the simulation rate

        f32 average_frame_time_in_ms = 1000.0f*average_frame_time;
        f32 frame_target_miss_amount_in_ms = clamp01(1000.0f*(peak_frame_time - adjusted_frame_dt));
        v4 timer_color = vec4(1.0f, 1.0f-frame_target_miss_amount_in_ms, 1.0f-frame_target_miss_amount_in_ms, 1.0f);
        layout_print_line(&layout, COLOR_WHITE, "Target Update Rate: %ghz, %fms/f", input->update_rate, 1000.0f / input->update_rate);
        layout_print_line(&layout, timer_color, "Average Frame Time: %fms, Peak: %fms", average_frame_time_in_ms, 1000.0f*peak_frame_time);
        layout_print_line(&layout, COLOR_WHITE, "Average Render Commands: %u, Peak: %u\n", average_render_commands, peak_render_commands);
    }

    if (!editor->shown) {
        return;
    }

    Level* level = game_state->active_level;

    if (game_state->game_mode == GameMode_Editor) {
        if (input->shift.is_down) {
            if (was_pressed(get_key(input, 'W'))) {
                editor->zoom *= 2.0f;
            }
            if (was_pressed(get_key(input, 'D'))) {
                editor->zoom *= 0.5f;
            }
            if (was_pressed(get_key(input, 'Q'))) {
                editor->grid_size *= 0.5f;
            }
            if (was_pressed(get_key(input, 'E'))) {
                editor->grid_size *= 2.0f;
            }
        }

        game_state->render_context.vertical_fov /= editor->zoom;
    }

    v2 mouse_p = vec2(input->mouse_x, input->mouse_y);
    v2 world_mouse_p = screen_to_world(&game_state->render_context, transform2d(mouse_p)).offset;

    editor->mouse_p = mouse_p;
    editor->world_mouse_p = world_mouse_p;

    if (was_pressed(get_key(input, 'S'))) {
        if (input->ctrl.is_down) {
            write_level_to_disk(game_state, game_state->active_level, wrap_string(game_state->active_level->name_length, game_state->active_level->name));
            editor->level_saved_timer = 2.0f;
        } else {
            editor->grid_snapping_enabled = !editor->grid_snapping_enabled;
        }
    }

    // @Note: It's important these come above any other widget, because any other widget should be able to override them.
    if (input->space.is_down) {
        editor->next_hot_widget = pan_widget;
    } else {
        editor->next_hot_widget = select_entities;
    }

    UILayout sound_log = make_layout(editor, vec2(width - 700.0f, editor->top_margin));
    layout_print_line(&sound_log, COLOR_WHITE, "Playing Sounds:");
    sound_log.depth++;
    for (AudioGroup* group = game_state->audio_mixer.first_audio_group; group; group = group->next_audio_group) {
        if (group->first_playing_sound) {
            layout_print_line(&sound_log, COLOR_WHITE, "Audio Group%s, volume { %g, %g }:",
                group->paused ? " (paused)" : "",
                group->volume.current_volume[0], group->volume.current_volume[1]
            );
        }
        sound_log.depth++;
        for (PlayingSound* playing_sound = group->first_playing_sound; playing_sound; playing_sound = playing_sound->next) {
            Sound* sound = playing_sound->sound;
            Asset* asset = cast(Asset*) sound;

            f32 volume_left  = playing_sound->volume.current_volume[0];
            f32 volume_right = playing_sound->volume.current_volume[1];

            v4 color = COLOR_WHITE;
            if (volume_left < 1.0e-4f && volume_right < 1.0e-4f) {
                color = COLOR_GREY;
            }

            layout_print_line(&sound_log, color, "Sound %.*s: Samples played: %u, Volume: { %g, %g }",
                string_expand(asset->name),
                playing_sound->samples_played,
                volume_left, volume_right
            );
        }
        sound_log.depth--;
    }
    sound_log.depth--;

    layout_print_line(&sound_log, COLOR_WHITE, "Playing Midi:");
    sound_log.depth++;
    for (PlayingMidi* playing_midi = game_state->first_playing_midi; playing_midi; playing_midi = playing_midi->next) {
        MidiTrack* track = playing_midi->track;

        v4 color = playing_midi->source_soundtrack_player->can_be_heard_by_player ? COLOR_WHITE : COLOR_GREY;

        layout_print_line(&sound_log, color, "Track from SoundtrackID { %u }: bpm: %g, %u events, tick timer: %u, event_index: %u",
            playing_midi->source_soundtrack_player->soundtrack_id.value,
            track->beats_per_minute,
            track->event_count,
            playing_midi->tick_timer,
            playing_midi->event_index
        );
    }
    sound_log.depth--;

    if (game_state->game_mode == GameMode_Menu) {
        return;
    }

    layout_print(&layout, COLOR_WHITE, "Editing level '%.*s'", cast(int) level->name_length, level->name);
    if (editor->level_saved_timer > 0.0f) {
        layout_print(&layout, vec4(1.0f, 1.0f, 1.0f, clamp01(editor->level_saved_timer)), "  (Saved...)");
    }
    layout_finish_print(&layout);

    layout_print_line(&layout, COLOR_WHITE, "Hot Widget: %s, desc: %s", widget_name(editor->hot_widget), editor->hot_widget.description);
    layout_print_line(&layout, COLOR_WHITE, "Active Widget: %s, desc: %s", widget_name(editor->active_widget), editor->active_widget.description);
    layout_print_line(&layout, COLOR_WHITE, "Last Activated Checkpoint: EntityID { %d }", game_state->last_activated_checkpoint ? game_state->last_activated_checkpoint->guid.value : 0);

    layout_print_line(&layout, COLOR_WHITE, "");

    if (editor_button(editor, &layout, input, editor->show_camera_zones ? "Hide Camera Zones" : "Show Camera Zones")) {
        editor->show_camera_zones = !editor->show_camera_zones;
    }

    if (editor_button(editor, &layout, input, editor->show_soundtrack_player_zones ? "Hide Soundtrack Player Zones" : "Show Soundtrack Player Zones")) {
        editor->show_soundtrack_player_zones = !editor->show_soundtrack_player_zones;
    }

    if (editor_button(editor, &layout, input, editor->show_checkpoint_zones ? "Hide Checkpoint Zones" : "Show Checkpoint Zones")) {
        editor->show_checkpoint_zones = !editor->show_checkpoint_zones;
    }

    if (editor_button(editor, &layout, input, editor->show_trigger_zones ? "Hide Trigger Zones" : "Show Trigger Zones")) {
        editor->show_trigger_zones = !editor->show_trigger_zones;
    }

    if (editor_button(editor, &layout, input, editor->show_all_move_widgets ? "Hide Move Widgets" : "Show Move Widgets") || was_pressed(get_key(input, 'M'))) {
        editor->show_all_move_widgets = !editor->show_all_move_widgets;
    }

    if (editor->level_saved_timer > 0.0f) {
        editor->level_saved_timer -= input->frame_dt;
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

    u32 last_seen_batch_id = 0;
    u32 undo_log_count = 0;
    for (u32 footer_index = editor->undo_most_recent; undo_log_count < 5;) {
        UndoFooter* footer = get_undo_footer(editor, footer_index);

        if (footer) {
            if (!footer->batch_id || (footer->batch_id != last_seen_batch_id)) {
                void* data = get_undo_data(footer);

                v4 color = vec4(0.85f, 0.85f, 0.85f, 1.0f - (cast(f32) undo_log_count / 4.5f));
                layout_print(&undo_log, color, "[%04u] %s: ", footer_index, enum_name_safe(UndoType, footer->type));
                switch (footer->type) {
                    case Undo_SetEntityData: {
                        layout_print(&undo_log, color, "EntityID { %u }", footer->entity_guid.value);
                    } break;
                    case Undo_SetData: {
                        layout_print(&undo_log, color, "0x%I64x", cast(u64) footer->data_ptr);
                    } break;
                    case Undo_CreateEntity:
                    case Undo_DeleteEntity: {
                        EntityID id = (cast(Entity*) data)->guid;
                        layout_print(&undo_log, color, "EntityID { %d }", id.value);
                    } break;
                }

                if (footer->description) {
                    layout_print(&undo_log, color, " [%s]", footer->description);
                }

                if (footer->batch_id) {
                    // layout_print(&undo_log, color, " batch id: %u", footer->batch_id);
                    last_seen_batch_id = footer->batch_id;
                }
                layout_finish_print(&undo_log);

                undo_log_count++;
            }

            footer_index = footer->prev;
        } else {
            break;
        }
    }

    Entity* moused_over = 0;
    if (game_state->game_mode == GameMode_Ingame) {
        for (u32 entity_index = 1; entity_index < game_state->entity_count; entity_index++) {
            Entity* entity = game_state->entities + entity_index;

            if (editor->shown || !(entity->flags & EntityFlag_Invisible)) {
                if (is_in_entity_local_region(entity, entity->collision, world_mouse_p)) {
                    moused_over = entity;
                }
            }
        }
    }

    if (!moused_over) {
        for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
            Entity* entity = level->entities + entity_index;

            if (is_in_entity_local_region(entity, entity->collision, world_mouse_p)) {
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

    if (is_active(editor, pan_widget)) {
        game_state->render_context.camera_p = editor->camera_p_on_pan;
        v2 pan_world_mouse_p = screen_to_world(&game_state->render_context, transform2d(mouse_p)).offset;
        game_state->render_context.camera_p += editor->world_mouse_p_on_active - pan_world_mouse_p;
        if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
            clear_active(editor);
        }
    } else if (is_hot(editor, pan_widget)) {
        if (was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
            editor->camera_p_on_pan = game_state->render_context.camera_p;
            set_active(editor, editor->hot_widget);
        }
    }

    if (is_active(editor, select_entities)) {
        AxisAlignedBox2 selection_box = bounding_aab(editor->world_mouse_p_on_active, world_mouse_p);
        v2 selection_dim = get_dim(selection_box);

        push_rect(&game_state->render_context, selection_box, vec4(0.3f, 0.3f, 0.5f, 0.2f), ShapeRenderMode_Fill);
        push_rect(&game_state->render_context, selection_box, vec4(0.5f, 0.5f, 0.8f, 0.8f), ShapeRenderMode_Outline);

        if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
            selection_box = correct_aab_winding(selection_box);

            u32 entity_count = game_state->game_mode == GameMode_Ingame ? game_state->entity_count : level->entity_count;
            Entity* entities = game_state->game_mode == GameMode_Ingame ? game_state->entities     : level->entities;
            for (u32 entity_index = 1; entity_index < entity_count; entity_index++) {
                Entity* entity = entities + entity_index;
                AxisAlignedBox2 test_box = grow_by_diameter(selection_box, entity->collision);
                if (editor->selected_entity_count < ARRAY_COUNT(editor->selected_entities) && is_in_aab(test_box, entity->p)) {
                    editor->selected_entities[editor->selected_entity_count++] = entity->guid;
                }
            }

            clear_active(editor);
        }
    } else if (is_hot(editor, select_entities)) {
        if (was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
            deselect_entities(editor);
            if (moused_over) {
                editor->selected_entity_count = 1;
                editor->selected_entities[0] = moused_over->guid;
                activate_drag_entities(editor);
            } else {
                set_active(editor, select_entities);
            }
        }
    }

    if (is_active(editor, drag_entities)) {
        v2 relative_world_mouse_p = world_mouse_p - editor->drag_entities_anchor;
        v2 delta = snap_to_grid(editor, relative_world_mouse_p);

        if (!editor->added_drag_entites_undo_history && length_sq(delta) > 0.0f) {
            begin_undo_batch(editor);
            for (u32 selected_index = 0; selected_index < editor->selected_entity_count; selected_index++) {
                Entity* entity = get_entity_from_guid(editor, editor->selected_entities[selected_index]);
                add_entity_data_undo_history(editor, wrap_entity_data(entity, &entity->p), "Entity Drag");
            }
            end_undo_batch(editor);
            editor->added_drag_entites_undo_history = true;
        }

        for (u32 selected_index = 0; selected_index < editor->selected_entity_count; selected_index++) {
            Entity* entity = get_entity_from_guid(editor, editor->selected_entities[selected_index]);
            entity->p += delta;
        }
        editor->drag_entities_anchor += delta;

        if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
            clear_active(editor);
        }
    } else if (is_hot(editor, drag_entities)) {
        if (was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
            Entity* manipulated = 0;
            for (u32 selected_index = 0; selected_index < editor->selected_entity_count; selected_index++) {
                if (moused_over->guid.value == editor->selected_entities[selected_index].value) {
                    manipulated = get_entity_from_guid(editor, editor->selected_entities[selected_index]);
                    if (manipulated) {
                        break;
                    }
                }
            }
            if (manipulated) {
                activate_drag_entities(editor);
            } else {
                deselect_entities(editor);
            }
        }
    }

    if (editor->selected_entity_count) {
        layout_print_line(&layout, COLOR_WHITE, "%u entities selected", editor->selected_entity_count);

        if (was_pressed(input->del)) {
            begin_undo_batch(editor);
            for (u32 selected_index = 0; selected_index < editor->selected_entity_count; selected_index++) {
                delete_entity(editor, editor->selected_entities[selected_index], true);
            }
            end_undo_batch(editor);
            deselect_entities(editor);
        }

        if (moused_over) {
            Entity* manipulated = 0;
            for (u32 selected_index = 0; selected_index < editor->selected_entity_count; selected_index++) {
                if (moused_over->guid.value == editor->selected_entities[selected_index].value) {
                    manipulated = get_entity_from_guid(editor, editor->selected_entities[selected_index]);
                    if (manipulated) {
                        break;
                    }
                }
            }
            if (manipulated) {
                editor->next_hot_widget = drag_entities;
            }
        }

        if (input->ctrl.is_down && was_pressed(get_key(input, 'D'))) {
            begin_undo_batch(editor);
            for (u32 selected_index = 0; selected_index < editor->selected_entity_count; selected_index++) {
                Entity* source_entity = get_entity_from_guid(editor, editor->selected_entities[selected_index]);
                if (source_entity) {
                    AddEntityResult duplicate = add_entity(editor, EntityType_Null);
                    copy(sizeof(*duplicate.ptr), source_entity, duplicate.ptr);
                    duplicate.ptr->guid = duplicate.guid;
                    duplicate.ptr->p += vec2(0.5f, 0.5f);
                    editor->selected_entities[selected_index] = duplicate.guid;

                    add_undo_history(editor, Undo_CreateEntity, sizeof(*duplicate.ptr), duplicate.ptr, "Duplicate");
                }
            }
            end_undo_batch(editor);
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

            case EntityType_TriggerZone: {
                created_entity = add_trigger_zone(editor, aab_center_dim(snap_to_grid(editor, world_mouse_p), vec2(4.0f, 6.0f))).ptr;
            } break;
        }

        if (created_entity) {
            add_undo_history(editor, Undo_CreateEntity, sizeof(*created_entity), created_entity);
        }

        editor->type_to_spawn = EntityType_Null;
    }

    if (editor->selected_entity_count || editor->show_all_move_widgets) {
        u32 entity_count = editor->show_all_move_widgets ? level->entity_count : editor->selected_entity_count;

        for (u32 entity_index = 0; entity_index < entity_count; entity_index++) {
            Entity* entity = 0;

            if (editor->show_all_move_widgets) {
                entity = level->entities + entity_index;
            } else {
                entity = get_entity_from_guid(editor, editor->selected_entities[entity_index]);
            }

            // @TODO: Get a for_entity_type in the editor?
            if (entity->type == EntityType_Wall && entity->wall_behaviour == WallBehaviour_Move) {
                EditorWidget widget = {};
                widget.guid = entity;
                widget.type = Widget_DragP;
                widget.description = "Drag Position";

                v2 world_end_p = entity->p + entity->end_p;

                widget.drag_p.original_p  = entity->end_p;
                widget.drag_p.drag_offset = world_end_p - world_mouse_p;
                widget.drag_p.target      = wrap_entity_data(entity, &entity->end_p);

                v3 fill_color    = is_active(editor, widget) ? COLOR_RED.rgb : entity->color.rgb;
                v3 outline_color = is_active(editor, widget) || is_hot(editor, widget) ? COLOR_RED.rgb : entity->color.rgb;

                push_rect(&game_state->render_context, aab_center_dim(world_end_p, entity->collision), vec4(fill_color, 0.25f));
                push_rect(&game_state->render_context, aab_center_dim(world_end_p, entity->collision), vec4(outline_color, 0.85f), ShapeRenderMode_Outline);
                push_line(&game_state->render_context, entity->p, world_end_p, vec4(outline_color, 0.85f));

                if (is_in_region(entity->collision, world_mouse_p - world_end_p)) {
                    editor->next_hot_widget = widget;
                }
            }
        }
    }

    layout_print_line(&layout, COLOR_WHITE, "");

    if (moused_over || editor->selected_entity_count == 1) {
        b32 is_selected = editor->selected_entity_count;
        Entity* selected = (editor->selected_entity_count == 1) ? get_entity_from_guid(editor, editor->selected_entities[0]) : moused_over;

        if (game_state->game_mode == GameMode_Editor) {
            if (editor->show_camera_zones && selected->type == EntityType_CameraZone) {
                drag_region_widget(game_state, editor, selected->p, wrap_entity_data(selected, &selected->active_region));
            } else if (editor->show_checkpoint_zones && selected->type == EntityType_Checkpoint) {
                drag_region_widget(game_state, editor, selected->p, wrap_entity_data(selected, &selected->checkpoint_zone));
            } else if (selected->type == EntityType_Wall) {
                drag_region_widget(game_state, editor, selected->p, wrap_entity_data(selected, &selected->collision));
            } else if (editor->show_soundtrack_player_zones && selected->type == EntityType_SoundtrackPlayer) {
                drag_region_widget(game_state, editor, selected->p, wrap_entity_data(selected, &selected->audible_zone));
            } else if (editor->show_trigger_zones && selected->type == EntityType_TriggerZone) {
                drag_region_widget(game_state, editor, selected->p, wrap_entity_data(selected, &selected->trigger_zone));
            }
        }

        layout_print_line(&layout, COLOR_WHITE, "Entity (%d, %s)", selected->guid.value, enum_name_safe(EntityType, selected->type));

        layout.depth++;

        LinearBuffer<EditableParameter>* editables = editor->editable_parameter_info[selected->type];
        if (editables) {
            for (u32 editable_index = 0; editable_index < editables->count; editable_index++) {
                EditableParameter* editable = editables->data + editable_index;
                void** editable_ptr = cast(void**) (cast(u8*) selected + editable->offset);

                EditorWidget widget = {};
                widget.guid = editable_ptr;
                widget.type = Widget_ManipulateEditable;
                widget.description = "Manipulate Editable";
                widget.start_value = cast(void*) *(cast(u64*) editable_ptr);

                v4 color = vec4(0.95f, 0.95f, 0.95f, 1);
                if (!(editable->flags & Editable_Static)) {
                    if (is_hot(editor, widget) || is_active(editor, widget)) {
                        color = vec4(1, 1, 0.5f, 1);
                    }
                }

                layout_print(&layout, color, "%s: ", editable->name);
                print_editable(game_state, &layout, editable, editable_ptr, color);

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
                        switch (editable->type) {
                            case Editable_WallBehaviour: {
                                WallBehaviour value = *(cast(WallBehaviour*) editable_ptr);
                                value = cast(WallBehaviour) (cast(u32) value + 1);
                                if (value >= WallBehaviour_Count) {
                                    value = WallBehaviour_None;
                                }
                                *(cast(WallBehaviour*) editable_ptr) = value;
                            } break;

                            case Editable_TriggerBehaviour: {
                                TriggerBehaviour value = *(cast(TriggerBehaviour*) editable_ptr);
                                value = cast(TriggerBehaviour) (cast(u32) value + 1);
                                if (value >= TriggerBehaviour_Count) {
                                    value = TriggerBehaviour_None;
                                }
                                *(cast(TriggerBehaviour*) editable_ptr) = value;
                            } break;

                            case Editable_s32: {
                                if (editable->flags & Editable_IsBool) {
                                    *(cast(b32*) editable_ptr) = !*(cast(b32*) editable_ptr);
                                }
                            } break;
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

                if (is_selected && is_in_aab(layout.last_print_bounds, mouse_p)) {
                    editor->next_hot_widget = widget;
                }

                layout_finish_print(&layout);
            }
        }
    }

    if (moused_over) {
        push_shape(&game_state->render_context, transform2d(moused_over->p), rectangle(moused_over->collision + vec2(0.1f, 0.1f)), COLOR_PINK, ShapeRenderMode_Outline, 1000.0f);
    }

    if (editor->selected_entity_count) {
        for (u32 selected_index = 0; selected_index < editor->selected_entity_count; selected_index++) {
            Entity* this_selected = get_entity_from_guid(editor, editor->selected_entities[selected_index]);
            if (this_selected) {
                push_shape(&game_state->render_context, transform2d(this_selected->p), rectangle(this_selected->collision + vec2(0.1f, 0.1f)), this_selected == moused_over ? COLOR_YELLOW : COLOR_GREEN, ShapeRenderMode_Outline, 1000.0f);
            }
        }
    }

    if (editor->active_widget.type) {
        switch (editor->active_widget.type) {
            case Widget_DragRegion: {
                EditorWidgetDragRegion* drag_region = &editor->active_widget.drag_region;
                Entity* entity = get_entity_from_guid(editor, drag_region->target.guid); // @WidgetEntityData
                v2* target = get_data(editor, drag_region->target);

                v2 mouse_delta = world_mouse_p - editor->world_mouse_p_on_active;
                *target = drag_region->original + snap_to_grid(editor, drag_region->scaling*mouse_delta);
                if (input->ctrl.is_down) {
                    f32 aspect_ratio = drag_region->original.x / drag_region->original.y;
                    target->x = aspect_ratio*target->y;
                }
                entity->p = drag_region->original_p + 0.5f*snap_to_grid(editor, mouse_delta);

                if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
                    target->x = abs(target->x);
                    target->y = abs(target->y);
                    clear_active(editor);
                }
            } break;

            case Widget_DragP: {
                v2* target = get_data(editor, editor->active_widget.drag_p.target);
                v2 original_p = editor->active_widget.drag_p.original_p;
                v2 drag_offset = editor->active_widget.drag_p.drag_offset;
                *target = original_p + snap_to_grid(editor, world_mouse_p - editor->world_mouse_p_on_active);
                if (was_released(input->mouse_buttons[PlatformMouseButton_Left])) {
                    clear_active(editor);
                }
            } break;
        }
    } else if (editor->hot_widget.type) {
        switch (editor->hot_widget.type) {
            case Widget_DragRegion: {
                EditorWidgetDragRegion* drag_region = &editor->hot_widget.drag_region;
                if (was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
                    Entity* entity = get_entity_from_guid(editor, drag_region->target.guid); // @WidgetEntityData
                    begin_undo_batch(editor);
                    add_entity_data_undo_history(editor, drag_region->target);
                    add_entity_data_undo_history(editor, wrap_entity_data(entity, &entity->p));
                    end_undo_batch(editor);
                    set_active(editor, editor->hot_widget);
                }
            } break;

            case Widget_DragP: {
                if (was_pressed(input->mouse_buttons[PlatformMouseButton_Left])) {
                    add_entity_data_undo_history(editor, editor->hot_widget.drag_p.target);
                    set_active(editor, editor->hot_widget);
                }
            } break;
        }
    }

    editor->hot_widget = editor->next_hot_widget;
    editor->next_hot_widget = {};
}
