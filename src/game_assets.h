#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

struct Sound {
    union { PackedSound packed_sound; struct { BodyOf_PackedSound }; };
    s16* samples;
};

// @Note: Image moved to render_commands.h
struct Image;

struct Font {
    union { PackedFont packed_font; struct { BodyOf_PackedFont }; };
    ImageID* glyph_table;
    f32* kerning_table;
};

struct MidiTrack {
    union { PackedMidi packed_midi; struct { BodyOf_PackedMidi }; };
    MidiEvent* events;
};

struct Asset {
    char* name;
    AssetType type;
    union {
        Sound sound;
        Image image;
        Font font;
        MidiTrack midi_track;
    };
};

struct Assets {
    u32 asset_count;
    Asset* asset_catalog;

    u8* asset_data;
};

#endif /* GAME_ASSETS_H */
