#ifndef PULSAR_ASSET_PACK_FORMAT_H
#define PULSAR_ASSET_PACK_FORMAT_H

struct AssetID      { u32 value; };
struct SoundID      { u32 value; };
struct ImageID      { u32 value; };
struct FontID       { u32 value; };
struct MidiID       { u32 value; };
struct SoundtrackID { u32 value; };

introspect() enum AssetType {
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

introspect() struct PackedImage {
    PixelFormat pixel_format;
     // @Note: I had written `u32 w, h;`, but right now the code generator doesn't handle multiple definitions on one line. That ought to be fixed.
    u32 w;
    u32 h;
    v2 align;
    v2 scale;

    /* Data:
     * PixelFormat* pixels[w*h];
     */
};

introspect() struct PackedSound {
    u32 channel_count;
    u32 sample_count;

    /* Data:
     * s16* samples[channel_count];
     */
};

introspect() struct PackedFont {
    u32 first_codepoint;
    u32 one_past_last_codepoint;
    u32 size;
    u32 oversample_amount;
    f32 whitespace_width;
    f32 ascent;
    f32 descent;
    f32 line_gap;

    /* Data:
     * glyph_count = one_past_last_codepoint - first_codepoint;
     * ImageID glyph_table[glyph_count];
     * f32 kerning_table[glyph_count][glyph_count];
     */
};

// @Note: A more complete implementation would allow time signature and bpm to vary across the track.
introspect() struct PackedMidi {
    u32 ticks_per_second;
    f32 beats_per_minute;
    u16 time_signature_numerator;
    u16 time_signature_denominator;
    u32 event_count;

    /* Data:
     * MidiEvent events[event_count];
     */
};

introspect() struct PackedSoundtrack {
    SoundID sound;
    u32 midi_track_count;

    /* Data:
     * MidiID midi_tracks[midi_track_count];
     */
};

enum MidiEventType {
    MidiEvent_NoteOn,
    MidiEvent_NoteOff,
};

introspect() struct MidiEvent {
    u32 absolute_time_in_ticks;
    u8 channel;
    u8 type;
    u8 note_value;
    u8 velocity;
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

#endif /* PULSAR_ASSET_PACK_FORMAT_H */
