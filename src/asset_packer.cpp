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
    AssetDataType_Unknown,

    AssetDataType_Wav,
    AssetDataType_Bmp,
    AssetDataType_Ttf,
    AssetDataType_TtfGlyph,
};

struct AssetDescription {
    char* asset_name;
    char* source_file;
    AssetDataType data_type;

    u32 asset_index;
    PackedAsset packed;
};

global MemoryArena global_arena;
global Array<AssetDescription> asset_descriptions;
global u32 packed_asset_count = 1; // @Note: Reserving the 0th index for the null asset

// @Note: Neither asset name nor file name is mandatory, because font glyphs have no need for them.
internal AssetDescription* add_asset(char* asset_name = 0, char* file_name = 0) {
    char* extension = file_name;
    for (char* at = file_name; at[0]; at++) {
        if (at[0] && at[-1] == '.') {
            extension = at;
        }
    }

    AssetDataType data_type = AssetDataType_Unknown;
    if (file_name) {
        if (strings_are_equal(extension, "wav")) {
            data_type = AssetDataType_Wav;
        } else if (strings_are_equal(extension, "bmp")) {
            data_type = AssetDataType_Bmp;
        } else if (strings_are_equal(extension, "ttf")) {
            data_type = AssetDataType_Ttf;
        } else {
            assert(!"Unknown extension for asset file.");
        }
    }

    AssetDescription* asset_desc = array_add(&asset_descriptions);
    asset_desc->asset_index = packed_asset_count++;
    asset_desc->asset_name = asset_name;
    asset_desc->source_file = file_name;
    asset_desc->data_type = data_type;

    return asset_desc;
}

internal AssetDescription* add_sound(char* asset_name, char* file_name) {
    return add_asset(asset_name, file_name);
}

internal AssetDescription* add_image(char* asset_name, char* file_name, v2 align = vec2(0.5f, 0.5f)) {
    AssetDescription* desc = add_asset(asset_name, file_name);
    PackedImage* image = &desc->packed.image;
    image->pixel_format = PixelFormat_BGRA8;
    image->align = align;
    return desc;
}

internal AssetDescription* add_font(char* asset_name, char* file_name, u32 size) {
    AssetDescription* desc = add_asset(asset_name, file_name);

    PackedFont* font = &desc->packed.font;
    // @TODO: Actual unicode support, instead of just a hardcoded ascii range.
    font->first_codepoint = '!';
    font->one_past_last_codepoint = '~' + 1;
    font->size = size;

    // @Note: We will pack an image asset for every individual glyph for now.
    packed_asset_count += font->one_past_last_codepoint - font->first_codepoint;

    return desc;
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
#define MEMORY_POOL_SIZE GIGABYTES(2ul)
    void* memory_pool = malloc(MEMORY_POOL_SIZE);
    // @Note: Zero the memory pool so everything allocated from the global
    // arena is automatically initialized to zero.
    memset(memory_pool, 0, MEMORY_POOL_SIZE);
    initialize_arena(&global_arena, MEMORY_POOL_SIZE, memory_pool);

    // String asset_manifest = read_text_file("asset_manifest.txt", &global_arena);
    asset_descriptions = allocate_array<AssetDescription>(64, &global_arena);

    add_sound("test_sound", "test_sound.wav");
    add_sound("test_music", "test_music.wav");
    add_image("test_image", "test_bitmap.bmp");
    add_font("test_font", "C:/Windows/Fonts/consola.ttf", 22);

    AssetPackHeader header;
    header.magic_value = ASSET_PACK_CODE('p', 'a', 'k', 'f');
    header.version = ASSET_PACK_VERSION;
    header.asset_count = packed_asset_count;

    u32 asset_catalog_size = header.asset_count*sizeof(PackedAsset);

    header.asset_catalog = sizeof(AssetPackHeader);
    header.asset_name_store = header.asset_catalog + asset_catalog_size;

    FILE* out = fopen("assets.pak", "wb");
    if (out) {
        fseek(out, header.asset_name_store, SEEK_SET);
        fputc(0, out); // @Note: The name store starts with a null byte for assets without a name.
        for (u32 asset_index = 0; asset_index < asset_descriptions.count; asset_index++) {
            AssetDescription* asset_desc = asset_descriptions.data + asset_index;
            PackedAsset* packed = &asset_desc->packed;

            packed->name_offset = ftell(out) - header.asset_name_store;
            u32 name_length = cast(u32) cstr_length(asset_desc->asset_name) + 1; // @Note: + 1 to include the null byte.

            fwrite(asset_desc->asset_name, name_length, 1, out);
        }

        header.asset_data = ftell(out);

        fseek(out, 0, SEEK_SET);
        fprintf(stderr, "Writing header (v: %d, ac: %d)\n", header.version, header.asset_count);
        fwrite(&header, sizeof(header), 1, out);

        fseek(out, header.asset_data, SEEK_SET);

        // @Note: Reserving the null index for the null asset
        u32 asset_catalog_index = 1;
        PackedAsset* asset_catalog = push_array(&global_arena, packed_asset_count, PackedAsset, no_clear());

        for (u32 asset_index = 0; asset_index < asset_descriptions.count; asset_index++) {
            AssetDescription* asset_desc = asset_descriptions.data + asset_index;
            asset_catalog[asset_catalog_index++] = asset_desc->packed;
            PackedAsset* packed = asset_catalog + asset_catalog_index - 1;

            packed->data_offset = ftell(out) - header.asset_data;

            TemporaryMemory temp_mem = begin_temporary_memory(&global_arena);

            EntireFile file = read_entire_file(asset_desc->source_file, 0, &global_arena);
            if (file.size) {
                switch (asset_desc->data_type) {
                    case AssetDataType_Wav: {
                        packed->type = AssetType_Sound;

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

                        packed->sound.channel_count = channel_count;
                        packed->sound.sample_count  = sample_count;

                        fwrite(deinterlaced_samples, sound_size, 1, out);

                        fprintf(stderr, "Packed sound '%s' (ch: %d, s: %d)\n", asset_desc->source_file, packed->sound.channel_count, packed->sound.sample_count);
                    } break;

                    case AssetDataType_Bmp: {
                        packed->type = AssetType_Image;

                        u32 w = 0;
                        u32 h = 0;
                        void* pixels = load_bitmap(file.size, file.data, &w, &h);

                        packed->image.w = w;
                        packed->image.h = h;

                        u32 bitmap_size = w*h*sizeof(u32);
                        fwrite(pixels, bitmap_size, 1, out);

                        fprintf(stderr, "Packed image '%s' (w: %d, h: %d, cx: %f, cy: %f)\n",
                            asset_desc->source_file,
                            packed->image.w, packed->image.h,
                            packed->image.align.x, packed->image.align.y
                        );
                    } break;

                    case AssetDataType_Ttf: {
                        packed->type = AssetType_Font;

                        const unsigned char* ttf_source = cast(const unsigned char*) file.data;
                        stbtt_fontinfo font_info;
                        stbtt_InitFont(&font_info, ttf_source, stbtt_GetFontOffsetForIndex(ttf_source, 0));

                        // @Note: The glyphs are stored in the asset catalog
                        // immediately after the font.
                        u32 first_glyph_index = asset_desc->asset_index + 1;
                        u32 glyph_count = packed->font.one_past_last_codepoint - packed->font.first_codepoint;
                        ImageID* glyph_table = push_array(&global_arena, glyph_count, ImageID);
                        for (u32 glyph_index = 0; glyph_index < glyph_count; glyph_index++) {
                            glyph_table[glyph_index] = { first_glyph_index + glyph_index };
                        }

                        fwrite(glyph_table, sizeof(ImageID), glyph_count, out);
                        fprintf(stderr, "Packed metadata for font '%s' (glyph count: %d)\n", asset_desc->source_file, glyph_count);

                        for (u32 glyph_index = 0; glyph_index < glyph_count; glyph_index++) {
                            PackedAsset* packed_glyph = asset_catalog + glyph_table[glyph_index].value;

                            packed_glyph->type = AssetType_Image;
                            packed_glyph->data_offset = ftell(out) - header.asset_data;

                            u32 codepoint = packed->font.first_codepoint + glyph_index;

                            int w, h, offset_x, offset_y;
                            u8* stb_glyph = stbtt_GetCodepointBitmap(
                                &font_info,
                                0, stbtt_ScaleForPixelHeight(&font_info, cast(f32) packed->font.size),
                                codepoint,
                                &w, &h,
                                &offset_x, &offset_y
                            );

                            // @TODO: See about single channel texture format
                            packed_glyph->image.pixel_format = PixelFormat_BGRA8;
                            packed_glyph->image.w = w;
                            packed_glyph->image.h = h;
                            packed_glyph->image.align = vec2(offset_x, offset_y) / vec2(w, h);

                            u32 pitch = sizeof(u32)*packed_glyph->image.w;
                            u32 out_glyph_size = packed_glyph->image.h*pitch;
                            u8* out_glyph = cast(u8*) push_size(&global_arena, , no_clear());

                            u8* source = stb_glyph;
                            u8* dest_row = out_glyph + packed_glyph->image.w*(packed_glyph->image.h - 1)*pitch;
                            for (u32 y = 0; y < packed_glyph->image.h; y++) {
                                u32* dest_pixel = cast(u32*) dest_row;
                                for (u32 x = 0; x < packed_glyph->image.w; x++) {
                                    u8 alpha = *source++;
                                    u8 color = cast(u8) (255.0f*square_root((cast(f32) alpha) / 255.0f));
                                    *dest_pixel++ = (alpha << 24) | (color << 16) | (color << 8) | (color << 0);
                                }
                                dest_row -= pitch;
                            }

                            fwrite(out_glyph, out_glyph_size, 1, out);
                            stbtt_FreeBitmap(stb_glyph, 0);
                        }

                        fprintf(stderr, "Packed %d glyphs (font: '%s')\n", glyph_count, asset_desc->source_file);
                    } break;

                    default: {
                        *packed = {};
                        fprintf(stderr, "Encountered unhandled asset data type '%d'.\n", asset_desc->data_type);
                    } break;
                }
            } else {
                fprintf(stderr, "Could not open file '%s'.\n", asset_desc->source_file);
            }

            if (packed->type == AssetType_Unknown) {
                fprintf(stderr, "Encountered unknown asset type for asset %d.\n", asset_catalog_index - 1);
            }

            end_temporary_memory(temp_mem);
        }

        fseek(out, header.asset_catalog, SEEK_SET);
        fwrite(asset_catalog, sizeof(PackedAsset), packed_asset_count, out);
    }

    check_arena(&global_arena);
}

#if 0
    // @TODO: Big fat unjankification project.
    b32 parse_successful = asset_manifest.len > 0;
    if (parse_successful) {
        while (parse_successful && chars_left(&asset_manifest)) {
            if (expect_char(&asset_manifest, &parse_successful, '"', "to begin asset name")) {
                advance(&asset_manifest);
                String asset_name = advance_to(&asset_manifest, '"');
                advance(&asset_manifest);
                eat_whitespaces(&asset_manifest);
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
                                    font->size = 12;

                                    if (match_word(&asset_manifest, "size")) {
                                        char* end_ptr = 0;
                                        font->size = parse_integer(asset_manifest.data, &end_ptr);
                                        if (end_ptr && asset_manifest.data != end_ptr) {
                                            assert(advance_to_ptr(&asset_manifest, end_ptr));
                                        } else {
                                            parse_successful = false;
                                            fprintf(stderr, "Asset Manifest Parse Error: Expected a valid integer after 'size'.\n");
                                        }
                                    }
                                } break;

                                INVALID_DEFAULT_CASE;
                            }
                            eat_whitespaces(&asset_manifest);
                        }
                    }
                }
            }
        }
    } else {
        fprintf(stderr, "Failed to open asset manifest.\n");
    }
#endif

