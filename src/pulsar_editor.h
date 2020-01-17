#ifndef PULSAR_EDITOR_H
#define PULSAR_EDITOR_H

#define UNDO_BUFFER_SIZE MEGABYTES(2)
#define MAX_ENTITY_COUNT 8192

struct Level {
    u32 name_length;
    char name[256];

    u32 first_available_guid;
    u32 entity_count;
    Entity entities[MAX_ENTITY_COUNT];
};

struct AddEntityResult {
    EntityID guid;
    Entity* ptr;
};

introspect() enum UndoType {
    Undo_Null,
    Undo_SetData,
    Undo_SetEntityData,
    Undo_CreateEntity,
    Undo_DeleteEntity,
};

struct UndoFooter {
    UndoType type;
    char* description;

    u32 data_size;

    union {
        void* data_ptr;
        struct {
            EntityID entity_guid;
            u32 entity_data_offset;
        };
    };

    u32 prev;
    u32 next;
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
    Editable_f32,
    Editable_v2,
    Editable_AxisAlignedBox2,
    Editable_EntityID,
    Editable_SoundtrackID,
    Editable_EntityFlag,
    Editable_EntityPtr,
};

struct EditableParameter {
    EditableType type;
    char* name;
    u32 offset;
    u32 size;
    u32 flags;

    union {
        struct { u32 min_value; u32 max_value; } e_u32;
        struct { f32 min_value; f32 max_value; } e_f32;
        struct { v2  min_value; v2  max_value; } e_v2;
    };
};

template <typename T>
struct EntityData {
    EntityID guid;
    u32 offset;
};

introspect() enum EditorWidgetType {
    Widget_None = 0,
    Widget_DragEditable,
    Widget_ManipulateEntity,
    Widget_DragV2,
};

enum EntityManipulateType {
    Manipulate_Default,
    Manipulate_DragEntity,
    Manipulate_DeleteEntity,
};

struct EditorWidgetManipulateEntity {
    EntityManipulateType type;
    EntityID guid;
    v2 drag_offset;
};

struct EditorWidgetDragV2 {
    v2 scaling;
    v2 original;
    EntityData<v2> target;
};

struct EditorWidget {
    void* guid;

    EditorWidgetType type;
    union {
        void* start_value;
        EditorWidgetManipulateEntity manipulate;
        EditorWidgetDragV2 drag_v2;
    };
};

struct EntityHash {
    EntityID guid;
    u32 index;
};

struct ConsoleState {
    b32 open;
    f32 openness_t;

    u32  input_buffer_count;
    char input_buffer[4096];
};

inline String input_buffer_as_string(ConsoleState* console) {
    String result = wrap_string(console->input_buffer_count, console->input_buffer);
    return result;
}

struct EditorState {
    b32 initialized;
    b32 shown;
    b32 show_statistics;

    ConsoleState console_state;

    MemoryArena* arena;

    Level* active_level;

    RenderContext render_context;
    Assets* assets;

    ImageID camera_icon;
    ImageID speaker_icon;
    ImageID checkpoint_icon;

    Font* big_font;
    Font* font;

    v2 default_collision;

    f32 zoom;

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

    f32 level_saved_timer;

    v2 spawn_menu_p;
    EntityType type_to_spawn;

    EditorWidget next_hot_widget;
    EditorWidget hot_widget;
    EditorWidget active_widget;

    EntityID selected_entity;

    EntityType current_editable_type;
    LinearBuffer<EditableParameter>* editable_parameter_info[EntityType_Count];

    EntityHash entity_hash[MAX_ENTITY_COUNT];

    u32 undo_most_recent;
    u32 undo_oldest;
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
    AxisAlignedBox2 last_print_bounds;
    AxisAlignedBox2 total_bounds;
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
    layout.last_print_bounds = inverted_infinity_aab2();
    layout.total_bounds = inverted_infinity_aab2();

    return layout;
}

#endif /* PULSAR_EDITOR_H */
