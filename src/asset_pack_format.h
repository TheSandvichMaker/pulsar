#ifndef ASSET_PACK_FORMAT_H
#define ASSET_PACK_FORMAT_H

#define NULL_NAME_OFFSET UINT32_MAX

struct SoundID {
    u32 value;
};

struct ImageID {
    u32 value;
};

struct FontID {
    u32 value;
};

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

#define PACKED_IMAGE_BODY               \
    PixelFormat pixel_format;           \
    u32 w, h;                           \
    v2 align;

struct PackedImage {
    PACKED_IMAGE_BODY;

    /* Data:
     * PixelFormat* pixels[w*h];
     */
};

#define PACKED_SOUND_BODY \
    u32 channel_count;    \
    u32 sample_count;

struct PackedSound {
    PACKED_SOUND_BODY;

    /* Data:
     * s16* samples[channel_count];
     */
};

#define PACKED_FONT_BODY         \
    u32 first_codepoint;         \
    u32 one_past_last_codepoint; \
    u32 size;

struct PackedFont {
    PACKED_FONT_BODY;

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

/*
   ------------------
   magic_value: APK_CODE('a', 'p', 'k', 'f')
   version: 0
   asset_count: 3
   asset_catalog: sizeof(header)
   asset_name_store: sizeof(header) + asset_catalog
   asset_data: sizeof(header) + asset_catalog * sizeof(PackedAsset)
   ------------------
   0
   "test music"
   "test sound"
   "test image"
   ------------------
   PackedAsset Asset_TestMusic {
       data_offset: 0
       apk_sound {
           channel_count: 2
           sample_count: ~
        }
    }
    PackedAsset Asset_TestSound {
       data_offset: TestMusic->data_offset + sizeof(TestMusic)
       apk_sound {
           channel_count: 2
           sample_count: ~
        }
    }
    PackedAsset Asset_TestImage {
       data_offset: TestSound->data_offset + sizeof(TestSound)
       apk_bitmap {
           channel_count: 2
           sample_count: ~
        }
    }
    ------------------
    [ data for TestMusic ]
    [ data for TestSound ]
    [ data for TestImage ]
*/

#endif /* ASSET_PACK_FORMAT_H */
