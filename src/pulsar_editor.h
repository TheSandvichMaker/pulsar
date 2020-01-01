#ifndef PULSAR_EDITOR_H
#define PULSAR_EDITOR_H

enum EditableFlag {
    Editable_IsMidiNote = 0x1,
    Editable_RangeLimited = 0x2,
};

enum EditableType {
    Editable_u32,
    Editable_v2,
    Editable_EntityID,
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

enum EditorWidgetType {
    Widget_None,
    Widget_DragEditable,
    Widget_ManipulateEntity,
};

enum EntityManipulateType {
    Manipulate_Default,
    Manipulate_DragEntity,
    Manipulate_DeleteEntity,
};

struct EditorWidgetManipulateEntity {
    EntityManipulateType type;
    Entity* entity;
    v2 drag_offset;
};

struct EditorWidget {
    void* guid;

    EditorWidgetType type;
    union {
        void* start_value;
        EditorWidgetManipulateEntity manipulate;
    };
};

inline char* widget_name(EditorWidgetType type) {
    switch (type) {
        enum_to_string(Widget_None);
        enum_to_string(Widget_DragEditable);
        enum_to_string(Widget_ManipulateEntity);
        INVALID_DEFAULT_CASE;
    }
    return 0;
}

inline char* widget_name(EditorWidget widget) {
    return widget_name(widget.type);
}

enum UndoType {
    Undo_Null,
    Undo_SetData,
    Undo_DeleteEntity,
};

inline char* undo_type_name(UndoType type) {
    switch (type) {
        enum_to_string(Undo_Null);
        enum_to_string(Undo_SetData);
        enum_to_string(Undo_DeleteEntity);
        INVALID_DEFAULT_CASE;
    }
    return 0;
}

struct UndoHeader {
    UndoType type;

    u32 data_size;
    void* data_ptr;

    u32 prev;
};

#define UNDO_BUFFER_SIZE KILOBYTES(512)

struct EditorState {
    b32 initialized;

    MemoryArena* arena;

    struct Level* active_level;

    GameRenderCommands* render_commands;
    Assets* assets;
    Font* font;

    v2 mouse_p;
    v2 mouse_p_on_active;

    v2 spawn_menu_p;
    EntityType type_to_spawn;

    b32 moving_entity;
    b32 did_move_entity;

    EditorWidget next_hot_widget;
    EditorWidget hot_widget;
    EditorWidget active_widget;

    Entity* selected_entity;

    EntityType current_editable_type;
    u32 editable_parameter_count[EntityType_Count];
    EditableParameter* editable_parameter_info[EntityType_Count];

    u32 undo_watermark;
    u32 undo_buffer_last_header;
    u8 undo_buffer[UNDO_BUFFER_SIZE];
};

struct EditorLayout {
    EditorState* editor;

    v2 origin;
    v2 at_p;
    f32 spacing;

    b32 print_initialized;
    Rect2 last_print_bounds;
};

inline EditorLayout make_layout(EditorState* editor, v2 origin, f32 spacing = 8.0f) {
    EditorLayout layout = {};
    layout.editor = editor;
    layout.origin = origin;
    layout.at_p = layout.origin;
    layout.spacing = 8.0f;

    return layout;
}

#endif /* PULSAR_EDITOR_H */
