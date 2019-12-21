#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

#include "asset_pack_format.h"

struct Sound {
    union {
        PackedSound packed_sound;
        struct {
            PACKED_SOUND_BODY;
        };
    };

    s16* samples;
};

struct Image {
    union {
        PackedImage packed_image;
        struct {
            PACKED_IMAGE_BODY;
        };
    };

    void* pixels;
    void* handle;
};

struct Font {
    union {
        PackedFont packed_font;
        struct {
            PACKED_FONT_BODY;
        };
    };

    ImageID* glyph_table;
};

struct Asset {
    char* name;
    AssetType type;
    union {
        Sound sound;
        Image image;
        Font font;
    };
};

struct Assets {
    u32 asset_count;
    Asset* asset_catalog;

    u8* asset_data;
};

#endif /* GAME_ASSETS_H */
