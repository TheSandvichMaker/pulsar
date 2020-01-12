#ifndef PULSAR_GENERATED_PRE_HEADERS_H
#define PULSAR_GENERATED_PRE_HEADERS_H

#define PULSAR_CODE_GENERATION_SUCCEEDED 1

enum MetaType {
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
    MetaType_EditorWidgetType,
    MetaType_UndoType,
    MetaType_EditorAssets,
    MetaType_ImageID,
    MetaType_EntityFlag,
    MetaType_EntityType,
    MetaType_Entity,
    MetaType_EntityID,
    MetaType_b32,
    MetaType_AxisAlignedBox2,
    MetaType_v4,
    MetaType_SoundtrackID,
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
    u32 beats_per_minute; \
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

#define BodyOf_EditorAssets \
    ImageID camera_icon; \
    ImageID speaker_icon; \
    ImageID checkpoint_icon;

#define BodyOf_Entity \
    EntityID guid; \
    EntityType type; \
    u32 flags; \
    b32 dead; \
    v2 p; \
    v2 dp; \
    v2 ddp; \
    AxisAlignedBox2 collision; \
    ImageID sprite; \
    v4 color; \
    f32 off_ground_timer; \
    f32 friction_of_last_touched_surface; \
    Entity* support; \
    v2 support_normal; \
    v2 local_p; \
    f32 surface_friction; \
    u32 midi_note; \
    f32 movement_t; \
    Entity* sticking_entity; \
    v2 sticking_dp; \
    v2 midi_test_target; \
    SoundtrackID soundtrack_id; \
    u32 playback_flags; \
    b32 soundtrack_has_been_played; \
    v2 camera_zone; \
    v2 camera_rotation_arm; \
    v2 checkpoint_zone; \
    v2 most_recent_player_position;

#endif
