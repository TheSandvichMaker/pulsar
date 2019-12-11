#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

#include "asset_pack_format.h"

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
    union {
        Sound sound;
        Image image;
    };
};

struct Assets {
    Asset asset_catalog[Asset_Count];
    u8* asset_data;
};

#endif /* GAME_ASSETS_H */
