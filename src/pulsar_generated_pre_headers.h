#ifndef PULSAR_GENERATED_PRE_HEADERS_H
#define PULSAR_GENERATED_PRE_HEADERS_H

#define PULSAR_CODE_GENERATION_SUCCEEDED 1

enum MetaType {
    MetaType_AssetType,
    MetaType_PackedImage,
    MetaType_PixelFormat,
    MetaType_u32,
    MetaType_v2,
    MetaType_PackedSound,
    MetaType_PackedFont,
    MetaType_f32,
    MetaType_PackedMidi,
    MetaType_u16,
    MetaType_PackedSoundtrack,
    MetaType_SoundID,
    MetaType_MidiEvent,
    MetaType_u8,
    MetaType_UndoType,
    MetaType_EditorWidgetType,
    MetaType_EntityPrefab,
    MetaType_EntityFlag,
    MetaType_EntityType,
    MetaType_WallBehaviour,
    MetaType_GameMode,
    MetaType_GameConfig,
    MetaType_b32,
    MetaType_String,
    MetaType_DummyIntrospectStruct,
    MetaType_s8,
    MetaType_s16,
    MetaType_s32,
    MetaType_s64,
    MetaType_u64,
    MetaType_f64,
    MetaType_char,
};

#define BodyOf_PackedImage \
    PixelFormat pixel_format; \
    u32 w; \
    u32 h; \
    v2 align; \
    v2 scale;

#define BodyOf_PackedSound \
    u32 channel_count; \
    u32 sample_count;

#define BodyOf_PackedFont \
    u32 first_codepoint; \
    u32 one_past_last_codepoint; \
    u32 size; \
    u32 oversample_amount; \
    f32 whitespace_width; \
    f32 ascent; \
    f32 descent; \
    f32 line_gap;

#define BodyOf_PackedMidi \
    u32 ticks_per_second; \
    f32 beats_per_minute; \
    u16 time_signature_numerator; \
    u16 time_signature_denominator; \
    u32 event_count;

#define BodyOf_PackedSoundtrack \
    SoundID sound; \
    u32 midi_track_count;

#define BodyOf_MidiEvent \
    u32 absolute_time_in_ticks; \
    u8 channel; \
    u8 type; \
    u8 note_value; \
    u8 velocity;

#define BodyOf_GameConfig \
    b32 start_fullscreen; \
    String startup_level; \
    u8 up; \
    u8 left; \
    u8 down; \
    u8 right; \
    u8 jump; \
    u8 interact; \
    u8 alternate_up; \
    u8 alternate_left; \
    u8 alternate_down; \
    u8 alternate_right; \
    u8 alternate_jump; \
    u8 alternate_interact; \
    u32 max_collision_iterations; \
    f32 gravity; \
    f32 downward_gravity_multiplier; \
    f32 death_by_crushing_threshold; \
    f32 support_dp_retention_time; \
    f32 movement_speed; \
    f32 max_x_vel; \
    f32 min_y_vel; \
    f32 max_y_vel; \
    f32 jump_force; \
    f32 early_jump_window; \
    f32 late_jump_window; \
    f32 camera_transition_speed; \
    f32 menu_bob_speed; \
    f32 menu_bob_magnitude; \
    f32 menu_quit_speed; \
    f32 simulation_rate; \
    f32 console_open_speed; \
    f32 console_close_speed;

#define BodyOf_DummyIntrospectStruct \
    b32 dummy_b32; \
    s8 dummy_s8; \
    s16 dummy_s16; \
    s32 dummy_s32; \
    s64 dummy_s64; \
    u8 dummy_u8; \
    u16 dummy_u16; \
    u32 dummy_u32; \
    u64 dummy_u64; \
    f32 dummy_f32; \
    f64 dummy_f64; \
    char dummy_char; \
    String dummy_string;

#endif
