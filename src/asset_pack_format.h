#ifndef ASSET_PACK_FORMAT_H
#define ASSET_PACK_FORMAT_H

struct SoundID { u32 value; };
struct ImageID { u32 value; };
struct FontID  { u32 value; };

enum AssetType {
    AssetType_Unknown,

    AssetType_Image,
    AssetType_Sound,
    AssetType_Font,
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
    u32 size;

struct PackedFont {
    BodyOf_PackedFont;

    /* Data:
     * ImageID glyph_table[one_past_last_codepoint - first_codepoint];
     */
};

struct PackedAsset {
    u64 data_offset;
    u32 name_offset;
    AssetType type;
    union {
        PackedSound sound;
        PackedImage image;
        PackedFont font;
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
