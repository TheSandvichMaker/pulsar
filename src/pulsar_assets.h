#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

struct Sound {
    using_struct(PackedSound, packed_sound);
    s16* samples;
};

// @Note: Image moved to render_commands.h
struct Image;

struct Font {
    using_struct(PackedFont, packed_font);
    ImageID* glyph_table;
    f32* kerning_table;
};

struct MidiTrack {
    using_struct(PackedMidi, packed_midi);
    MidiEvent* events;
};

struct Soundtrack {
    using_struct(PackedSoundtrack, packed_soundtrack);
    MidiID* midi_tracks;
};

struct Asset {
    union {
        Sound sound;
        Image image;
        Font font;
        MidiTrack midi_track;
        Soundtrack soundtrack;
    };
    String name;
    AssetType type;
};

struct Assets {
    u32 asset_count;
    Asset* asset_catalog;

    u8* asset_data;
};

inline f32 get_line_spacing(Font* font);

#endif /* GAME_ASSETS_H */
