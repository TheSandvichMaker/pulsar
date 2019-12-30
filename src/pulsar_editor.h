#ifndef PULSAR_EDITOR_H
#define PULSAR_EDITOR_H

enum EditableFlag {
    Editable_IsMidiNote = 0x1,
    Editable_RangeLimited = 0x2,
};

enum EditableType {
    Editable_u32,
    Editable_v2,
};

struct EditableU32 {
    u32 min_value;
    u32 max_value;
};

struct EditableV2 {
    v2 min_value;
    v2 max_value;
};

struct EditableParameter {
    EditableType type;
    char* name;
    u32 offset;
    u32 size;
    u32 flags;

    union {
        EditableU32 e_u32;
        EditableV2  e_v2;
    };
};

struct EditorState {
    b32 initialized;

    MemoryArena* arena;

    struct Level* active_level;

    GameRenderCommands* render_commands;
    Assets* assets;
    Font* font;

    v2 origin;
    v2 at_p;
    f32 spacing;

    b32 bounds_initialized;
    Rect2 last_print_bounds;

    Entity* selected_entity;
    EditableParameter* selected_editable;

    EntityType current_editable_type;
    u32 editable_parameter_count[EntityType_Count];
    EditableParameter* editable_parameter_info[EntityType_Count];
};

#endif /* PULSAR_EDITOR_H */
