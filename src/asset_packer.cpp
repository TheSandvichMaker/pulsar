#include <stdio.h>
#include <stdlib.h>
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

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb_truetype.h"

enum AssetDataType {
    AssetDataType_None,
    AssetDataType_Wav,
    AssetDataType_Bmp,
    AssetDataType_Ttf,
};

struct AssetDescription {
    char* asset_name;
    char* source_file;
    AssetDataType data_type;
    PackedAsset asset;
};

global MemoryArena global_arena;
global Array<AssetDescription> assets_to_load;

internal AssetDescription* add_asset(char* asset_name, char* file_name) {
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
    } else if (strings_are_equal(extension, "ttf")) {
        data_type = AssetDataType_Ttf;
    } else {
        assert(!"Unknown extension for asset file.");
    }

    AssetDescription* asset_desc = array_add(&assets_to_load);
    asset_desc->asset_name = asset_name;
    asset_desc->source_file = file_name;
    asset_desc->data_type = data_type;

    return asset_desc;
}

internal void add_sound(char* asset_name, char* file_name) {
    add_asset(asset_name, file_name);
}

internal void add_image(char* asset_name, char* file_name, f32 align_x = 0.5f, f32 align_y = 0.5f) {
    AssetDescription* desc = add_asset(asset_name, file_name);
    PackedImage* image = &desc->asset.image;
    image->center_point_x = align_x;
    image->center_point_y = align_y;
}

internal void add_font(char* asset_name, char* file_name) {
    AssetDescription* desc = add_asset(asset_name, file_name);
    PackedFont* font = &desc->asset.font;
    font->first_codepoint = '!';
    font->one_past_last_codepoint = '~' + 1;
}

inline b32 expect_char(String* asset_manifest, b32* parse_success, char c, char* additional_info = 0) {
    b32 result = false;
    if (peek(asset_manifest) == c) {
        result = true;
    } else {
        fprintf(stderr, "Asset Manifest Parse Error: Expected '%c'%s%s.\n",
            c,
            additional_info ? " " : "",
            additional_info ? additional_info : ""
        );
        *parse_success = false;
    }
    return result;
}

int main(int argument_count, char** arguments) {
#define MEMORY_POOL_SIZE GIGABYTES(2)
    void* memory_pool = malloc(MEMORY_POOL_SIZE);
    initialize_arena(&global_arena, MEMORY_POOL_SIZE, memory_pool);

    String asset_manifest = read_text_file("asset_manifest.txt", &global_arena);
    assets_to_load = allocate_array<AssetDescription>(64, &global_arena); // @Note: I don't really care about efficient memory usage on this guy, so why not start with space for 64 assets.

    // @TODO: Big fat unjankification project.
    b32 parse_successful = asset_manifest.len > 0;
    if (parse_successful) {
        while (parse_successful && chars_left(&asset_manifest)) {
            String asset_name = advance_to(&asset_manifest, ':');
            if (expect_char(&asset_manifest, &parse_successful, ':', "after asset name")) {
                advance_to(&asset_manifest, '"');
                if (expect_char(&asset_manifest, &parse_successful, '"', "to start asset file path")) {
                    advance(&asset_manifest);
                    String file_name = advance_to(&asset_manifest, '"');
                    if (expect_char(&asset_manifest, &parse_successful, '"', "to end asset file path")) {
                        advance(&asset_manifest);
                        char* asset_name_c = push_string_and_null_terminate(&global_arena, asset_name.len, asset_name.data);
                        char*  file_name_c = push_string_and_null_terminate(&global_arena, file_name.len, file_name.data);
                        AssetDescription* desc = add_asset(asset_name_c, file_name_c);
                        switch (desc->data_type) {
                            case AssetDataType_Wav: {
                            } break;
                            case AssetDataType_Bmp: {
                                PackedImage* image = &desc->asset.image;
                                image->center_point_x = 0.5f;
                                image->center_point_y = 0.5f;

                                if (match_word(&asset_manifest, "align")) {
                                    char* end_ptr = 0;
                                    image->center_point_x = strtof(asset_manifest.data, &end_ptr);
                                    if (end_ptr && asset_manifest.data != end_ptr) {
                                        assert(advance_to_ptr(&asset_manifest, end_ptr));
                                    } else {
                                        parse_successful = false;
                                        fprintf(stderr, "Asset Manifest Parse Error: Expected two valid floats after 'align'.\n");
                                    }
                                    image->center_point_y = strtof(asset_manifest.data, &end_ptr);
                                    if (end_ptr && asset_manifest.data != end_ptr) {
                                        assert(advance_to_ptr(&asset_manifest, end_ptr));
                                    } else {
                                        parse_successful = false;
                                        fprintf(stderr, "Asset Manifest Parse Error: Expected two valid floats after 'align'.\n");
                                    }
                                }
                            } break;
                            case AssetDataType_Ttf: {
                                PackedFont* font = &desc->asset.font;
                                font->first_codepoint = '!';
                                font->one_past_last_codepoint = '~' + 1;
                            } break;

                            INVALID_DEFAULT_CASE;
                        }
                        eat_whitespaces(&asset_manifest);
                    }
                }
            }
        }
    } else {
        fprintf(stderr, "Failed to open asset manifest.\n");
    }

    if (!parse_successful) {
        fprintf(stderr, "Failed to parse asset manifest.\n");
        clear_array(&assets_to_load);
        add_sound("test_sound", "test_sound.wav");
        add_sound("test_music", "test_music.wav");
        add_image("test_image", "test_bitmap.bmp");
    }

    AssetPackHeader header;
    header.magic_value = ASSET_PACK_CODE('p', 'a', 'k', 'f');
    header.version = ASSET_PACK_VERSION;
    header.asset_count = safe_truncate_u64u32(assets_to_load.count);

    u32 asset_catalog_size = header.asset_count*sizeof(PackedAsset);

    header.asset_catalog = sizeof(AssetPackHeader);
    header.asset_name_store = header.asset_catalog + asset_catalog_size;

    FILE* out = fopen("assets.pak", "wb");
    if (out) {
        fseek(out, header.asset_name_store, SEEK_SET);
        for (u32 asset_index = 0; asset_index < assets_to_load.count; asset_index++) {
            AssetDescription* asset_desc = assets_to_load.data + asset_index;
            PackedAsset* packed = &asset_desc->asset;
            packed->name_offset = ftell(out) - header.asset_name_store;
            u32 name_length = cast(u32) cstr_length(asset_desc->asset_name) + 1; // @Note: + 1 to include the null byte.
            fwrite(asset_desc->asset_name, name_length, 1, out);
        }

        header.asset_data = ftell(out);

        fseek(out, 0, SEEK_SET);
        fprintf(stderr, "Writing header (v: %d, ac: %d)\n", header.version, header.asset_count);
        fwrite(&header, sizeof(header), 1, out);

        fseek(out, header.asset_data, SEEK_SET);

        for (u32 asset_index = 0; asset_index < assets_to_load.count; asset_index++) {
            AssetDescription* asset_desc = assets_to_load.data + asset_index;
            PackedAsset* packed = &asset_desc->asset;

            packed->data_offset = ftell(out) - header.asset_data;

            TemporaryMemory temp_mem = begin_temporary_memory(&global_arena);

            EntireFile file = read_entire_file(asset_desc->source_file, 0, &global_arena);
            if (file.size) {
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
            } else {
                fprintf(stderr, "Could not open file '%s'.\n", asset_desc->source_file);
            }

            end_temporary_memory(temp_mem);
        }

        fseek(out, header.asset_catalog, SEEK_SET);
        for (u32 asset_index = 0; asset_index < assets_to_load.count; asset_index++) {
            AssetDescription* asset_desc = assets_to_load.data + asset_index;
            PackedAsset* packed = &asset_desc->asset;
            fwrite(packed, sizeof(PackedAsset), 1, out);
        }
    }

    check_arena(&global_arena);
}
