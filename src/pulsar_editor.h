#ifndef PULSAR_EDITOR_H
#define PULSAR_EDITOR_H

#define MAX_ENTITY_COUNT 8192
struct Level {
    String name;

    u32 last_used_guid;
    u32 entity_count;
    Entity entities[MAX_ENTITY_COUNT];
};

struct AddEntityResult {
    EntityID guid;
    Entity* ptr;
};

enum EditableFlag {
    Editable_IsMidiNote = 0x1,
    Editable_RangeLimited = 0x2,
    Editable_Static = 0x4,
    Editable_IsBool = 0x8,
};

enum EditableType {
    Editable_u32,
    Editable_s32,
    Editable_v2,
    Editable_Rect2,
    Editable_EntityID,
    Editable_SoundtrackID,
    Editable_EntityFlag,
    Editable_EntityPtr,
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

introspect() enum EditorWidgetType {
    Widget_None,
    Widget_DragEditable,
    Widget_ManipulateEntity,
    Widget_DragRect,
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

introspect() enum UndoType {
    Undo_Null,
    Undo_SetData,
    Undo_CreateEntity,
    Undo_DeleteEntity,
};

struct UndoHeader {
    UndoType type;

    u32 data_size;
    void* data_ptr;

    u32 prev;
};

struct EntityHash {
    EntityID guid;
    u32 index;
};

#define UNDO_BUFFER_SIZE KILOBYTES(512)

struct EditorState {
    b32 initialized;
    b32 shown;

    MemoryArena* arena;

    Level* active_level;

    RenderGroup render_group;
    Assets* assets;

    Image* camera_icon;
    Image* speaker_icon;

    Font* big_font;
    Font* font;

    f32 top_margin;
    f32 left_margin;

    v2 mouse_p;
    v2 world_mouse_p;

    v2 mouse_p_on_active;
    v2 world_mouse_p_on_active;

    b32 grid_snapping_enabled;
    v2 grid_size;

    b32 panning;
    v2 world_mouse_p_on_pan;
    v2 camera_p_on_pan;

    v2 camera_p_on_exit;

    v2 spawn_menu_p;
    EntityType type_to_spawn;

    EditorWidget next_hot_widget;
    EditorWidget hot_widget;
    EditorWidget active_widget;

    Entity* selected_entity;

    EntityType current_editable_type;
    u32 editable_parameter_count[EntityType_Count];
    EditableParameter* editable_parameter_info[EntityType_Count];

    EntityHash entity_hash[MAX_ENTITY_COUNT];

    u32 undo_watermark;
    u32 undo_buffer_last_header;
    u8 undo_buffer[UNDO_BUFFER_SIZE];
};

struct EditorLayout {
    EditorState* editor;

    Font* active_font;

    v2 origin;
    v2 at_p;
    f32 spacing;
    u32 depth;

    f32 vertical_advance;

    b32 print_initialized;
    u32 last_codepoint;

    f32 font_scale;
    Rect2 last_print_bounds;
    Rect2 total_bounds;
};

inline EditorLayout make_layout(EditorState* editor, v2 origin, b32 bottom_up = false) {
    EditorLayout layout = {};

    layout.editor = editor;
    layout.active_font = editor->font;
    layout.origin = origin;
    layout.at_p = layout.origin;
    layout.spacing = 8.0f;

    layout.vertical_advance = -(get_line_spacing(editor->font) + layout.spacing);
    if (bottom_up) {
        layout.vertical_advance = -layout.vertical_advance;
    }

    layout.font_scale = 1.0f;
    layout.last_print_bounds = inverted_infinity_rectangle2();
    layout.total_bounds = inverted_infinity_rectangle2();

    return layout;
}

#endif /* PULSAR_EDITOR_H */
