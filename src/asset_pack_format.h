#ifndef ASSET_PACK_FORMAT_H
#define ASSET_PACK_FORMAT_H

enum AssetType {
    AssetType_Image,
    AssetType_Sound,
};

enum AssetID {
    Asset_TestMusic,
    Asset_TestSound,
    Asset_TestImage,

    Asset_Count,
};

struct PackedImage {
    u32 w, h;
    f32 center_point_x, center_point_y;
};

struct PackedSound {
    u32 channel_count;
    u32 sample_count;
};

struct PackedAsset {
    u64 data_offset;
    AssetType type;
    union {
        PackedSound sound;
        PackedImage image;
    };
};

/*
   ------------------
   magic_value: APK_CODE('a', 'p', 'k', 'f')
   version: 0
   asset count: 3
   asset catalog: sizeof(header)
   data: sizeof(header) + asset_catalog * sizeof(PackedAsset)
   ------------------
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

#define ASSET_PACK_CODE(a, b, c, d) (((u32)(a) << 0) | ((u32)(b) << 8) | ((u32)(c) << 16) | ((u32)(d) << 24))
struct AssetPackHeader {
    u32 magic_value;

#define ASSET_PACK_VERSION 0
    u32 version;

    u32 asset_count;
    u32 asset_catalog; // @Note: indexed into by AssetType, so the values of AssetType must be in sync with the file.
    u32 asset_data;
};

#endif /* ASSET_PACK_FORMAT_H */
