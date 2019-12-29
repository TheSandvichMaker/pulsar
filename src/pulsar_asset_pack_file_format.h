#ifndef ASSET_PACK_FORMAT_H
#define ASSET_PACK_FORMAT_H

struct SoundID      { u32 value; };
struct ImageID      { u32 value; };
struct FontID       { u32 value; };
struct MidiID       { u32 value; };
struct SoundtrackID { u32 value; };

enum AssetType {
    AssetType_Unknown,

    AssetType_Image,
    AssetType_Sound,
    AssetType_Font,
    AssetType_Midi,

    AssetType_Soundtrack,
};

enum PixelFormat {
    PixelFormat_BGRA8,
    PixelFormat_A8,
};

#define BodyOf_PackedImage    \
    PixelFormat pixel_format; \
    u32 w, h;                 \
    v2 align;

struct PackedImage {
    BodyOf_PackedImage;

    /* Data:
     * PixelFormat* pixels[w*h];
     */
};

#define BodyOf_PackedSound \
    u32 channel_count;     \
    u32 sample_count;

struct PackedSound {
    BodyOf_PackedSound;

    /* Data:
     * s16* samples[channel_count];
     */
};

#define BodyOf_PackedFont        \
    u32 first_codepoint;         \
    u32 one_past_last_codepoint; \
    u32 size;                    \
    f32 whitespace_width;        \
    f32 ascent;                  \
    f32 descent;                 \
    f32 line_gap;

struct PackedFont {
    BodyOf_PackedFont;

    /* Data:
     * glyph_count = one_past_last_codepoint - first_codepoint;
     * ImageID glyph_table[glyph_count];
     * f32 kerning_table[glyph_count][glyph_count];
     */
};

// @Note: A more complete implementation would allow time signature and bpm to vary across the track.
#define BodyOf_PackedMidi           \
    u32 ticks_per_second;           \
    u32 beats_per_minute;           \
    u16 time_signature_numerator;   \
    u16 time_signature_denominator; \
    u32 event_count;

struct PackedMidi {
    BodyOf_PackedMidi;

    /* Data:
     * MidiEvent events[event_count];
     */
};

#define BodyOf_PackedSoundtrack \
    SoundID sound;              \
    u32 midi_track_count;

struct PackedSoundtrack {
    BodyOf_PackedSoundtrack;

    /* Data:
     * MidiID midi_tracks[midi_track_count];
     */
};

enum MidiEventType {
    MidiEvent_NoteOn,
    MidiEvent_NoteOff,
};

#define BodyOf_MidiEvent        \
    u32 absolute_time_in_ticks; \
    u8 channel;                 \
    u8 type;                    \
    u8 note_value;              \
    u8 velocity;

struct MidiEvent {
    BodyOf_MidiEvent;
};

struct PackedAsset {
    u64 data_offset;
    u32 name_offset;
    AssetType type;
    union {
        PackedSound sound;
        PackedImage image;
        PackedFont font;
        PackedMidi midi;
        PackedSoundtrack soundtrack;
    };
};

#define ASSET_PACK_CODE(a, b, c, d) (((u32)(a) << 0) | ((u32)(b) << 8) | ((u32)(c) << 16) | ((u32)(d) << 24))
struct AssetPackHeader {
    u32 magic_value;

#define ASSET_PACK_VERSION 0
    u32 version;

    u32 asset_count;
    u32 asset_catalog;
    u32 asset_name_store;
    u32 asset_data;
};

#endif /* ASSET_PACK_FORMAT_H */
