#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

#include "asset_pack_format.h"

#define INVALID_ASSET UINT32_MAX

struct SoundID {
    u32 value;
};

struct ImageID {
    u32 value;
};

struct FontID {
    u32 value;
};

struct Sound {
    u32 channel_count;
    u32 sample_count;
    s16* samples;
};

struct Image {
    u32 w, h;
    v2 center_point;
    void* pixels;

    void* handle;
};

struct Asset {
    char* name;
    AssetType type;
    union {
        Sound sound;
        Image image;
    };
};

struct Assets {
    u32 asset_count;
    Asset* asset_catalog;

    u8* asset_data;
};

#endif /* GAME_ASSETS_H */
