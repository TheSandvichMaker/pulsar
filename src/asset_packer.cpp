#include <stdio.h>
#include <memory.h>

#include "common.h"
#include "intrinsics.h"
#include "memory_arena.h"
#include "template_array.h"
#include "math.h"
#include "string.h"
#include "file_io.h"
#include "asset_loading.h"

#include "file_io.cpp"
#include "asset_loading.cpp"

#include "asset_pack_format.h"

enum AssetDataType {
    AssetDataType_None,
    AssetDataType_Wav,
    AssetDataType_Bmp,
};

struct AssetDescription {
    char* source_file;
    AssetDataType data_type;
};

global MemoryArena global_arena;
global AssetDescription assets_to_load[Asset_Count];
global PackedAsset asset_catalog[Asset_Count];

internal void add_asset(AssetID type, char* file_name) {
    assert(type < Asset_Count);

    char* extension = file_name;
    for (char* at = file_name; at[0]; at++) {
        if (at[0] && at[-1] == '.') {
            extension = at;
        }
    }

    AssetDataType data_type = AssetDataType_None;
    if (strings_are_equal(extension, "wav")) {
        data_type = AssetDataType_Wav;
    } else if (strings_are_equal(extension, "bmp")) {
        data_type = AssetDataType_Bmp;
    } else {
        assert(!"Unknown extension for asset file.");
    }

    AssetDescription* asset_desc = assets_to_load + type;
    asset_desc->source_file = file_name;
    asset_desc->data_type = data_type;
}

int main(int argument_count, char** arguments) {
#define MEMORY_POOL_SIZE GIGABYTES(1)
    void* memory_pool = malloc(MEMORY_POOL_SIZE);
    initialize_arena(&global_arena, MEMORY_POOL_SIZE, memory_pool);

    add_asset(Asset_TestMusic, "test_music.wav");
    add_asset(Asset_TestSound, "test_sound.wav");
    add_asset(Asset_TestImage, "test_bitmap.bmp");

    AssetPackHeader header;
    header.magic_value = ASSET_PACK_CODE('p', 'a', 'k', 'f');
    header.version = ASSET_PACK_VERSION;
    header.asset_count = Asset_Count;

    u32 asset_catalog_size = Asset_Count*sizeof(PackedAsset);

    header.asset_catalog = sizeof(AssetPackHeader);
    header.asset_data = header.asset_catalog + asset_catalog_size;

    FILE* out = fopen("assets.pak", "wb");
    if (out) {
        fwrite(&header, sizeof(header), 1, out);
        fseek(out, header.asset_data, SEEK_SET);

        for (u32 asset_index = 0; asset_index < ARRAY_COUNT(asset_catalog); asset_index++) {
            AssetDescription* asset_desc = assets_to_load + asset_index;
            PackedAsset* packed = asset_catalog + asset_index;

            packed->data_offset = ftell(out);

            TemporaryMemory temp_mem = begin_temporary_memory(&global_arena);

            EntireFile file = read_entire_file(asset_desc->source_file, 0, &global_arena);
            assert(file.size);
            switch (asset_desc->data_type) {
                case AssetDataType_Wav: {
                    u32 channel_count = 0;
                    u32 sample_rate = 0;
                    u32 sample_count = 0;
                    s16* samples = load_wav(file.size, file.data, &channel_count, &sample_rate, &sample_count);
                    assert(sample_rate == 48000);

                    u32 sound_size = sample_count*channel_count*sizeof(s16);

                    s16* deinterlaced_samples = cast(s16*) push_size(&global_arena, sound_size);

                    for (u32 sample_index = 0; sample_index < sample_count; sample_index++) {
                        for (u32 channel_index = 0; channel_index < channel_count; channel_index++) {
                            size_t channel_offset = channel_index*sample_count;
                            deinterlaced_samples[sample_index + channel_offset] = samples[sample_index*channel_count + channel_index];
                        }
                    }

                    packed->type = AssetType_Sound;
                    packed->sound.channel_count = channel_count;
                    packed->sound.sample_count  = sample_count;

                    fwrite(deinterlaced_samples, sound_size, 1, out);

                    fprintf(stderr, "Packed sound '%s' (ch: %d, s: %d)\n", asset_desc->source_file, packed->sound.channel_count, packed->sound.sample_count);
                } break;

                case AssetDataType_Bmp: {
                    u32 w = 0;
                    u32 h = 0;
                    void* pixels = load_bitmap(file.size, file.data, &w, &h);

                    packed->type = AssetType_Image;
                    packed->image.w = w;
                    packed->image.h = h;
                    // @TODO: Allow alignment specification
                    packed->image.center_point_x = 0.5f;
                    packed->image.center_point_y = 0.5f;

                    u32 bitmap_size = w*h*sizeof(u32);
                    fwrite(pixels, bitmap_size, 1, out);

                    fprintf(stderr, "Packed image '%s' (w: %d, h: %d, cx: %f, cy: %f)\n",
                        asset_desc->source_file,
                        packed->image.w, packed->image.h,
                        packed->image.center_point_x, packed->image.center_point_y
                    );
                } break;

                default: {
                    *packed = {};
                } break;
            }

            end_temporary_memory(temp_mem);
        }

        fseek(out, header.asset_catalog, SEEK_SET);
        fwrite(asset_catalog, asset_catalog_size, 1, out);
    }

    check_arena(&global_arena);
}
