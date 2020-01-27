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

    u32 batch_id;
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

global char* midi_note_names[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

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
    Editable_WallBehaviour,
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
    Widget_Generic,
    Widget_DragEditable,
    Widget_ManipulateEntity,
    Widget_DragRegion,
    Widget_DragP,
};

enum EntityManipulateType {
    Manipulate_Default,
    Manipulate_DragEntity,
    Manipulate_DeleteEntity,
};

struct EditorWidgetManipulateEntity {
    EntityManipulateType type;
    EntityID guid;
    v2 original_p;
    v2 drag_offset;
};

struct EditorWidgetDragP {
    v2 original_p;
    v2 drag_offset;
    EntityData<v2> target;
};

struct EditorWidgetDragRegion {
    v2 scaling;
    v2 original;
    v2 original_p;
    EntityData<v2> target;
};

struct EditorWidget {
    void* guid;

    EditorWidgetType type;
    union {
        void* start_value;
        EditorWidgetManipulateEntity manipulate;
        EditorWidgetDragRegion drag_region;
        EditorWidgetDragP drag_p;
    };
};

inline EditorWidget generic_widget(void* guid) {
    EditorWidget result = {};
    result.guid = guid;
    result.type = Widget_Generic;
    return result;
}

struct EntityHash {
    EntityID guid;
    u32 index;
};

introspect() enum EntityPrefab {
    EntityPrefab_Null,

    EntityPrefab_Hazard,
    EntityPrefab_InvisibleHazard,
};

struct EditorState {
    // @TODO: Disentangle GameState from EditorState
    struct GameState* game_state;

    b32 initialized;
    b32 shown;
    b32 show_statistics;

    MemoryArena* arena;

    RenderContext render_context;
    Assets* assets;

    ImageID camera_icon;
    ImageID speaker_icon;
    ImageID checkpoint_icon;

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

    v2 camera_p_on_pan;
    v2 camera_p_on_exit;

    f32 level_saved_timer;

    v2 spawn_menu_p;
    EntityType type_to_spawn;
    EntityPrefab prefab_to_spawn;
    LinearBuffer<EntityPrefab>* entity_prefabs[EntityType_Count];

    b32 show_all_move_widgets;
    b32 show_camera_zones;
    b32 show_soundtrack_player_zones;
    b32 show_checkpoint_zones;

    EditorWidget next_hot_widget;
    EditorWidget hot_widget;
    EditorWidget active_widget;

    EntityID selected_entity;

    u32 selected_entity_count;
    EntityID selected_entities[32];
    v2 drag_group_anchor;

    EntityType current_editable_type;
    LinearBuffer<EditableParameter>* editable_parameter_info[EntityType_Count];

    EntityHash entity_hash[MAX_ENTITY_COUNT];

    u32 doing_undo_batch;
    u32 current_batch_id;
    u32 undo_most_recent;
    u32 undo_oldest;
    u8 undo_buffer[UNDO_BUFFER_SIZE];
};

struct UILayoutContext {
    RenderContext* rc;
    Assets* assets;
    MemoryArena* temp_arena;
};

struct UILayout {
    UILayoutContext context;

    u32 flags;

    Font* font;

    v2 origin;
    v2 at_p;
    v2 offset_p;
    f32 spacing;
    u32 depth;

    f32 vertical_advance;

    b32 print_initialized;
    u32 last_codepoint;

    f32 font_scale;
    AxisAlignedBox2 last_print_bounds;
    AxisAlignedBox2 total_bounds;
};

enum LayoutFlag {
    Layout_BottomUp        = 0x1,
    // @TODO: Make these do something
    Layout_HorzCenterAlign = 0x2,
    Layout_VertCenterAlign = 0x4,
    Layout_CenterAlign     = Layout_HorzCenterAlign|Layout_VertCenterAlign,
};

inline UILayout make_layout(UILayoutContext context, Font* font, v2 origin, u32 flags = 0) {
    UILayout layout = {};

    layout.context = context;

    layout.flags = flags;

    layout.font = font;

    layout.origin = origin;
    layout.at_p = layout.origin;
    layout.spacing = 8.0f;

    layout.vertical_advance = -(get_line_spacing(layout.font) + layout.spacing);
    if (layout.flags & Layout_BottomUp) {
        layout.vertical_advance = -layout.vertical_advance;
    }

    layout.font_scale = 1.0f;
    layout.last_print_bounds = inverted_infinity_aab2();
    layout.total_bounds = inverted_infinity_aab2();

    return layout;
}

inline void set_spacing(UILayout* layout, f32 spacing) {
    layout->spacing = spacing;

    layout->vertical_advance = -(get_line_spacing(layout->font) + layout->spacing);
    if (layout->flags & Layout_BottomUp) {
        layout->vertical_advance = -layout->vertical_advance;
    }
}

inline void set_font(UILayout* layout, Font* font) {
    layout->font = font;
    set_spacing(layout, layout->spacing);
}

inline UILayout make_layout(EditorState* editor, v2 origin, b32 bottom_up = false) {
    UILayout layout = make_layout({ &editor->render_context, editor->assets, editor->arena }, editor->font, origin, bottom_up);
    return layout;
}

#endif /* PULSAR_EDITOR_H */
