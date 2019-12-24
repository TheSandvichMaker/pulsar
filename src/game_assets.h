#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

#include "asset_pack_format.h"

struct Sound {
    union { PackedSound packed_sound; struct { BodyOf_PackedSound }; };
    s16* samples;
};

struct Image {
    union { PackedImage packed_image; struct { BodyOf_PackedImage }; };
    void* pixels;
    void* handle;
};

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
