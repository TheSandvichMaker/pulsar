#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "pulsar_common.h"
#include "intrinsics.h"
#include "math.h"
#include "string.h"
#include "file_io.h"

#include "pulsar_memory.h"
#include "pulsar_memory_arena.h"

#include "file_io.cpp"

#include "pulsar_asset_pack_file_format.h"
#include "pulsar_template_array.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "external/stb_truetype.h"

#define GAMMA_CORRECT_FONTS 1
#define FONT_OVERSAMPLING 2
#define AUDIO_SAMPLE_RATE 48000

#pragma pack(push, 1)
struct BitmapHeader {
    u16 file_type;
    u32 file_size;
    u16 reserved1;
    u16 reserved2;
    u32 bitmap_offset;
    u32 size;
    s32 width;
    s32 height;
    u16 planes;
    u16 bits_per_pixel;

    u32 compression;
    u32 size_of_bitmap;
    s32 horz_resolution;
    s32 vert_resolution;
    u32 colors_used;
    u32 colors_important;

    u32 red_mask;
    u32 green_mask;
    u32 blue_mask;
    u32 alpha_mask;
};
#pragma pack(pop)

internal void* load_bitmap(size_t size, void* memory, u32* out_width, u32* out_height) {
    void* result = 0;

    if (size != 0 && memory) {
        BitmapHeader* header = cast(BitmapHeader*) memory;

        assert(header->size >= sizeof(header));
        assert(header->height >= 0);
        assert(header->compression == 3);

        u32* pixels = cast(u32*) (cast(u8*) header + header->bitmap_offset);
        *out_width  = header->width;
        *out_height = header->height;

        u32 alpha_mask = header->alpha_mask;
        u32 red_mask   = header->red_mask;
        u32 green_mask = header->green_mask;
        u32 blue_mask  = header->blue_mask;

        // TODO: Would the alpha mask ever not be present?
        if (!alpha_mask) {
            alpha_mask = ~(red_mask | green_mask | blue_mask);
        }

        BitScanResult alpha_scan = find_least_significant_set_bit(alpha_mask);
        BitScanResult red_scan   = find_least_significant_set_bit(red_mask);
        BitScanResult green_scan = find_least_significant_set_bit(green_mask);
        BitScanResult blue_scan  = find_least_significant_set_bit(blue_mask);

        assert(alpha_scan.found && red_scan.found && green_scan.found && blue_scan.found);

        s32 alpha_shift_down = cast(s32) alpha_scan.index;
        s32 red_shift_down   = cast(s32) red_scan.index;
        s32 green_shift_down = cast(s32) green_scan.index;
        s32 blue_shift_down  = cast(s32) blue_scan.index;

        s32 alpha_shift_up = 24;
        s32 red_shift_up   = 16;
        s32 green_shift_up =  8;
        s32 blue_shift_up  =  0;

        u32* source_dest = pixels;
        for (s32 i = 0; i < header->width * header->height; i++) {
            u32 c = *source_dest;
            f32 texel_r = cast(f32) ((c & red_mask) >> red_shift_down);
            f32 texel_g = cast(f32) ((c & green_mask) >> green_shift_down);
            f32 texel_b = cast(f32) ((c & blue_mask) >> blue_shift_down);
            f32 texel_a = cast(f32) ((c & alpha_mask) >> alpha_shift_down);

            f32 rcp_255 = 1.0f/255.0f;
            texel_a = rcp_255*texel_a;
            texel_r = 255.0f*square_root(square(rcp_255*texel_r) * texel_a);
            texel_g = 255.0f*square_root(square(rcp_255*texel_g) * texel_a);
            texel_b = 255.0f*square_root(square(rcp_255*texel_b) * texel_a);
            texel_a *= 255.0f;

            *source_dest++ = cast(u32) (texel_a + 0.5f) << alpha_shift_up |
                             cast(u32) (texel_r + 0.5f) << red_shift_up   |
                             cast(u32) (texel_g + 0.5f) << green_shift_up |
                             cast(u32) (texel_b + 0.5f) << blue_shift_up;
        }

        result = pixels;
    }

    return result;
}

#pragma pack(push, 1)
struct WaveHeader {
    u32 riff_id;
    u32 size;
    u32 wave_id;
};

struct WaveFmt {
    u16 format_tag;
    u16 num_channels;
    u32 samples_per_sec;
    u32 avg_bytes_per_sec;
    u16 block_align;
    u16 bits_per_sample;
    u16 cb_size;
    u16 valid_bits_per_sample;
    u32 channel_mask;
    u8 sub_format[16];
};

struct WaveChunk {
    u32 id;
    u32 size;
};
#pragma pack(pop)

#define RIFF_CODE(a, b, c, d) (((u32)(a) << 0) | ((u32)(b) << 8) | ((u32)(c) << 16) | ((u32)(d) << 24))
enum WaveChunkID {
    WaveChunkID_fmt = RIFF_CODE('f', 'm', 't', ' '),
    WaveChunkID_RIFF = RIFF_CODE('R', 'I', 'F', 'F'),
    WaveChunkID_WAVE = RIFF_CODE('W', 'A', 'V', 'E'),
    WaveChunkID_data = RIFF_CODE('d', 'a', 't', 'a'),
};

struct RiffIterator {
    u8* at;
    u8* stop;
};

inline RiffIterator parse_chunk_at(void* at, void* stop) {
    RiffIterator iter;

    iter.at = (u8*)at;
    iter.stop = (u8*)stop;

    return iter;
}

inline RiffIterator next_chunk(RiffIterator iter) {
    WaveChunk* chunk = cast(WaveChunk*) iter.at;
    u32 size = (chunk->size + 1) & ~1;
    iter.at += sizeof(WaveChunk) + size;
    return iter;
}

inline b32 is_valid(RiffIterator iter) {
    b32 result = iter.at < iter.stop;
    return result;
}

inline void* get_chunk_data(RiffIterator iter) {
    void* result = iter.at + sizeof(WaveChunk);
    return result;
}

inline u32 get_type(RiffIterator iter) {
    WaveChunk* chunk = cast(WaveChunk*) iter.at;
    u32 result = chunk->id;
    return result;
}

inline u32 get_chunk_data_size(RiffIterator iter) {
    WaveChunk* chunk = cast(WaveChunk*) iter.at;
    u32 result = chunk->size;
    return result;
}

internal s16* load_wav(size_t size, void* memory, u32* out_channel_count, u32* out_sample_rate, u32* out_sample_count) {
    s16* result = 0;

    if (size != 0 && memory) {
        WaveHeader* header = cast(WaveHeader*) memory;
        assert(header->riff_id == WaveChunkID_RIFF);
        assert(header->wave_id == WaveChunkID_WAVE);

        u32 sample_rate = 0;
        u32 channel_count = 0;
        s16* sample_data = 0;
        u32 sample_data_size = 0;
        for (RiffIterator iter = parse_chunk_at(header + 1, cast(u8*) (header + 1) + header->size - 4);
             is_valid(iter);
             iter = next_chunk(iter)
        ) {
            switch (get_type(iter)) {
                case WaveChunkID_fmt: {
                    WaveFmt* fmt = cast(WaveFmt*) get_chunk_data(iter);
                    assert(fmt->format_tag == 1);
                    assert(fmt->num_channels == 2);
                    assert(fmt->samples_per_sec == AUDIO_SAMPLE_RATE);
                    assert(fmt->bits_per_sample == 16);
                    assert(fmt->block_align == sizeof(s16)*fmt->num_channels);
                    channel_count = fmt->num_channels;
                    sample_rate = fmt->samples_per_sec;
                } break;

                case WaveChunkID_data: {
                    sample_data = cast(s16*) get_chunk_data(iter);
                    sample_data_size = get_chunk_data_size(iter);
                } break;
            }
        }

        assert(channel_count && sample_data);
        assert(channel_count == 2);

        *out_channel_count = channel_count;
        *out_sample_count = sample_data_size / (sizeof(s16)*channel_count);
        *out_sample_rate = sample_rate;
        result = sample_data;
    }

    return result;
}

#pragma pack(push, 1)
struct MidiChunk {
    u32 type;
    u32 length;
};

struct MidiHeader {
    u16 format;
    u16 ntrcks;
    u16 division;
};
#pragma pack(pop)

#define MIDI_TYPE(type) ((type[0] << 0) | (type[1] << 8) | (type[2] << 16) | (type[3] << 24))

struct MidiStream {
    u8* at;
    u8* stop;
};

inline u32 flip_endianness_u32(u32 value) {
    u32 result = ((value & 0x000000FF) << 24) |
                 ((value & 0x0000FF00) <<  8) |
                 ((value & 0x00FF0000) >>  8) |
                 ((value & 0xFF000000) >> 24);
    return result;
}

inline u32 midi_read_variable_length_quantity(MidiStream* stream) {
    u32 result = (*stream->at++);

    if (result & 0x80) {
        u8 c;
        result &= 0x7F;
        do {
            c = (*stream->at++);
            result = (result << 7) + (c & 0x7F);
        } while (c & 0x80);
    }

    return result;
}

inline u8 midi_read_u8(MidiStream* stream) {
    u8 result = *stream->at++;
    return result;
}

inline u32 midi_read_u32(MidiStream* stream) {
    u32 result  = midi_read_u8(stream) << 24;
        result |= midi_read_u8(stream) << 16;
        result |= midi_read_u8(stream) <<  8;
        result |= midi_read_u8(stream) <<  0;
    return result;
}

inline u32 midi_read_u24(MidiStream* stream) {
    u32 result  = midi_read_u8(stream) << 16;
        result |= midi_read_u8(stream) <<  8;
        result |= midi_read_u8(stream) <<  0;
    return result;
}

inline u16 midi_read_u16(MidiStream* stream) {
    u16 result  = midi_read_u8(stream) << 8;
        result |= midi_read_u8(stream) << 0;
    return result;
}

inline MidiChunk midi_read_chunk(MidiStream* stream) {
    MidiChunk result;
    result.type = *(cast(u32*) stream->at);
    stream->at += 4;
    result.length = midi_read_u32(stream);
    return result;
}

enum AssetDataType {
    AssetDataType_Unknown,

    AssetDataType_Wav,
    AssetDataType_Bmp,
    AssetDataType_Ttf,
    AssetDataType_Midi,

    AssetDataType_Soundtrack,
};

struct AssetDescription {
    char* asset_name;
    char* source_file;
    AssetDataType data_type;

    u32 asset_index;
    PackedAsset packed;

    f32 bpm;
    MidiID* midi_tracks;
};

global MemoryArena global_arena;
global Array<AssetDescription> asset_descriptions;
global u32 packed_asset_count = 1; // @Note: Reserving the 0th index for the null asset

internal AssetDescription* add_asset(char* asset_name = 0, char* file_name = 0) {
    AssetDataType data_type = AssetDataType_Unknown;
    if (file_name) {
        char* extension = file_name;
        for (char* at = file_name; at[0]; at++) {
            if (at[0] && at[-1] == '.') {
                extension = at;
            }
        }

        if (strings_are_equal(extension, "wav")) {
            data_type = AssetDataType_Wav;
        } else if (strings_are_equal(extension, "bmp")) {
            data_type = AssetDataType_Bmp;
        } else if (strings_are_equal(extension, "ttf")) {
            data_type = AssetDataType_Ttf;
        } else if (strings_are_equal(extension, "mid")) {
            data_type = AssetDataType_Midi;
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
    image->scale = vec2(1.0f, 1.0f);
    return desc;
}

internal AssetDescription* add_font(char* asset_name, char* file_name, u32 size) {
    AssetDescription* desc = add_asset(asset_name, file_name);

    PackedFont* font = &desc->packed.font;
    // @TODO: Actual unicode support, instead of just a hardcoded ascii range.
    font->first_codepoint = ' ';
    font->one_past_last_codepoint = '~' + 1;
    font->size = size;

    // @Note: We will pack an image asset for every individual glyph for now.
    packed_asset_count += font->one_past_last_codepoint - font->first_codepoint;

    return desc;
}

internal AssetDescription* add_midi_track(char* asset_name, char* file_name, f32 bpm) {
    AssetDescription* desc = add_asset(asset_name, file_name);
    desc->bpm = bpm;
    return desc;
}

internal AssetDescription* add_soundtrack(char* asset_name, char* audio_file, u32 midi_file_count, char** midi_files, f32 bpm) {
    AssetDescription* desc = add_asset(asset_name, 0);
    desc->data_type = AssetDataType_Soundtrack;
    desc->bpm = bpm;

    desc->packed.soundtrack.sound = { add_asset(0, audio_file)->asset_index };
    desc->packed.soundtrack.midi_track_count = midi_file_count;
    desc->midi_tracks = push_array(&global_arena, midi_file_count, MidiID, no_clear());
    for (u32 midi_index = 0; midi_index < midi_file_count; midi_index++) {
        desc->midi_tracks[midi_index] = { add_midi_track(0, midi_files[midi_index], bpm)->asset_index };
    }
    return desc;
}

int main(int argument_count, char** arguments) {
#define MEMORY_POOL_SIZE GIGABYTES(2ul)
    void* memory_pool = malloc(MEMORY_POOL_SIZE);
    // @Note: Zero the memory pool so everything allocated from the global
    // arena is automatically initialized to zero.
    memset(memory_pool, 0, MEMORY_POOL_SIZE);
    initialize_arena(&global_arena, MEMORY_POOL_SIZE, memory_pool);

    allocate_array(&asset_descriptions, 64, allocator(arena_allocator, &global_arena));

    {
        char* midi_files[] = { "assets/ugly_loop.mid" };
        add_soundtrack("ugly_loop", "assets/ugly_loop.wav", ARRAY_COUNT(midi_files), midi_files, 120);
    }

    {
        char* midi_files[] = { "assets/test_soundtrack.mid" };
        add_soundtrack("test_soundtrack", "assets/test_soundtrack.wav", ARRAY_COUNT(midi_files), midi_files, 120);
    }

    {
        char* midi_files[] = { "assets/track1_1.mid" };
        add_soundtrack("track1_1", "assets/track1_1.wav", ARRAY_COUNT(midi_files), midi_files, 118);
    }

    {
        char* midi_files[] = { "assets/pulsar_kicktrack_1.mid" };
        add_soundtrack("pulsar_kicktrack_1", "assets/pulsar_kicktrack_1.wav", ARRAY_COUNT(midi_files), midi_files, 95);
    }

    {
        char* midi_files[] = { "assets/demo_level_loop_1.mid" };
        add_soundtrack("demo_level_loop_1", "assets/demo_level_loop_1_drums.wav", ARRAY_COUNT(midi_files), midi_files, 95);
        add_soundtrack("demo_level_loop_1_pad", "assets/demo_level_loop_1_pad.wav", 0, 0, 95);
        char* bells_midi_files[] = { "assets/demo_level_loop_1_bells.mid" };
        add_soundtrack("demo_level_loop_1_bells", "assets/demo_level_loop_1_bells.wav", ARRAY_COUNT(bells_midi_files), bells_midi_files, 95);
    }

    add_sound("test_sound", "assets/test_sound.wav");
    add_sound("test_music", "assets/test_music.wav");

    add_sound("menu_ambient", "assets/menu_ambient.wav");
    add_sound("menu_select", "assets/menu_select.wav");
    add_sound("menu_confirm", "assets/menu_confirm.wav");

    add_image("camera_icon", "assets/camera_icon.bmp");
    add_image("speaker_icon", "assets/speaker_icon.bmp");
    add_image("checkpoint_icon", "assets/checkpoint_icon.bmp");

    add_font("console_font", "C:/Windows/Fonts/consola.ttf", 16);
    add_font("editor_font", "C:/Windows/Fonts/SourceSerifPro-Regular.ttf", 24);
    add_font("menu_font", "C:/Windows/Fonts/SourceSerifPro-Regular.ttf", 72);
    add_font("big_menu_font", "C:/Windows/Fonts/SourceSerifPro-Regular.ttf", 128);

    AssetPackHeader header;
    header.magic_value = ASSET_PACK_CODE('p', 'l', 'a', 'f');
    header.version = ASSET_PACK_VERSION;
    header.asset_count = packed_asset_count;

    u32 asset_catalog_size = sizeof(PackedAsset)*header.asset_count;

    header.asset_catalog = sizeof(AssetPackHeader);
    header.asset_name_store = header.asset_catalog + asset_catalog_size;

    FILE* out = fopen("assets.pla", "wb");
    if (out) {
        fseek(out, header.asset_name_store, SEEK_SET);
        fputc(0, out); // @Note: The name store starts with a null byte for assets without a name.
        for (u32 asset_index = 0; asset_index < asset_descriptions.count; asset_index++) {
            AssetDescription* asset_desc = asset_descriptions.data + asset_index;

            if (asset_desc->asset_name) {
                PackedAsset* packed = &asset_desc->packed;

                packed->name_offset = ftell(out) - header.asset_name_store;
                u32 name_length = cast(u32) cstr_length(asset_desc->asset_name) + 1; // @Note: + 1 to include the null byte.

                fwrite(asset_desc->asset_name, name_length, 1, out);
            }
        }

        header.asset_data = ftell(out);

        fseek(out, 0, SEEK_SET);
        fprintf(stderr, "Writing header (ver: %d, assets: %d)\n", header.version, header.asset_count);
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

            EntireFile file = {};
            if (asset_desc->source_file) {
                file = read_entire_file(asset_desc->source_file, allocator(arena_allocator, &global_arena));
            }

            if (file.size || !asset_desc->source_file) {
                switch (asset_desc->data_type) {
                    case AssetDataType_Wav: {
                        packed->type = AssetType_Sound;

                        u32 channel_count, sample_rate, sample_count;
                        s16* samples = load_wav(file.size, file.data, &channel_count, &sample_rate, &sample_count);
                        assert(sample_rate == AUDIO_SAMPLE_RATE);

                        u32 sound_size = sizeof(s16)*channel_count*sample_count;

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

                        fprintf(stderr, "Packed sound '%s' from '%s' (ch: %d, s: %d)\n", asset_desc->asset_name, asset_desc->source_file, packed->sound.channel_count, packed->sound.sample_count);
                    } break;

                    case AssetDataType_Bmp: {
                        packed->type = AssetType_Image;

                        u32 w, h;
                        void* pixels = load_bitmap(file.size, file.data, &w, &h);

                        packed->image.w = w;
                        packed->image.h = h;

                        u32 bitmap_size = sizeof(u32)*w*h;
                        fwrite(pixels, bitmap_size, 1, out);

                        fprintf(stderr, "Packed image '%s' from '%s' (w: %d, h: %d, align: { %f, %f })\n",
                            asset_desc->asset_name,
                            asset_desc->source_file,
                            packed->image.w, packed->image.h,
                            packed->image.align.x, packed->image.align.y
                        );
                    } break;

                    case AssetDataType_Ttf: {
                        packed->type = AssetType_Font;

                        const u8* ttf_source = cast(const u8*) file.data;

                        stbtt_fontinfo font_info;
                        stbtt_InitFont(&font_info, ttf_source, stbtt_GetFontOffsetForIndex(ttf_source, 0));

                        u32 oversample_amount = FONT_OVERSAMPLING;
                        f32 rcp_oversample_amount = 1.0f / cast(f32) oversample_amount;
                        f32 font_scale = stbtt_ScaleForPixelHeight(&font_info, cast(f32) (oversample_amount*packed->font.size));

                        s32 ascent, descent, line_gap;
                        stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);

                        f32 scaled_ascent = (rcp_oversample_amount*font_scale) * cast(f32) ascent;
                        f32 scaled_descent = (rcp_oversample_amount*font_scale) * cast(f32) descent;
                        f32 scaled_line_gap = (rcp_oversample_amount*font_scale) * cast(f32) line_gap;

                        packed->font.ascent = scaled_ascent;
                        packed->font.descent = scaled_descent;
                        packed->font.line_gap = scaled_line_gap;
                        packed->font.oversample_amount = oversample_amount;

                        // @Note: The glyphs are stored in the asset catalog immediately after the font.
                        u32 glyph_count = packed->font.one_past_last_codepoint - packed->font.first_codepoint;
                        ImageID* glyph_table = push_array(&global_arena, glyph_count, ImageID);
                        for (u32 glyph_index = 0; glyph_index < glyph_count; glyph_index++) {
                            glyph_table[glyph_index] = { asset_catalog_index++ };
                        }

                        fwrite(glyph_table, sizeof(ImageID), glyph_count, out);
                        fprintf(stderr, "Packed font '%s' from '%s' (glyph count: %d)\n", asset_desc->asset_name, asset_desc->source_file, glyph_count);

                        u32 kerning_table_size = sizeof(f32)*glyph_count*glyph_count;
                        f32* kerning_table = push_array(&global_arena, glyph_count*glyph_count, f32, no_clear());

                        u32 kerning_table_position = ftell(out);
                        fseek(out, kerning_table_size, SEEK_CUR);

                        s32 whitespace_advance_width;
                        stbtt_GetCodepointHMetrics(&font_info, ' ', &whitespace_advance_width, 0);

                        packed->font.whitespace_width = rcp_oversample_amount*font_scale*cast(f32) whitespace_advance_width;

                        for (u32 glyph_index = 0; glyph_index < glyph_count; glyph_index++) {
                            TemporaryMemory glyph_temp = begin_temporary_memory(&global_arena);

                            PackedAsset* packed_glyph = asset_catalog + glyph_table[glyph_index].value;

                            packed_glyph->type = AssetType_Image;
                            packed_glyph->data_offset = ftell(out) - header.asset_data;

                            u32 codepoint = packed->font.first_codepoint + glyph_index;

                            s32 ix0, iy0, ix1, iy1;
                            stbtt_GetCodepointBitmapBox(&font_info, codepoint, font_scale, font_scale, &ix0, &iy0, &ix1, &iy1);

                            s32 w = ix1 - ix0;
                            s32 h = iy1 - iy0;
                            u8* stb_glyph = cast(u8*) push_size(&global_arena, w*h);
                            stbtt_MakeCodepointBitmap(&font_info, stb_glyph, w, h, w, font_scale, font_scale, codepoint);

                            s32 advance_width, left_side_bearing;
                            stbtt_GetCodepointHMetrics(&font_info, codepoint, &advance_width, &left_side_bearing);

                            f32 scaled_left_side_bearing = rcp_oversample_amount*font_scale*cast(f32) left_side_bearing;

                            for (u32 paired_glyph_index = 0; paired_glyph_index < glyph_count; paired_glyph_index++) {
                                u32 paired_codepoint = packed->font.first_codepoint + paired_glyph_index;
                                s32 kern_width = stbtt_GetCodepointKernAdvance(&font_info, codepoint, paired_codepoint);
                                kerning_table[glyph_count*paired_glyph_index + glyph_index] = rcp_oversample_amount*font_scale*cast(f32) (advance_width + kern_width);
                            }

                            // @TODO: See about single channel texture format
                            packed_glyph->image.pixel_format = PixelFormat_BGRA8;
                            packed_glyph->image.w = w;
                            packed_glyph->image.h = h;
                            packed_glyph->image.align = vec2(-scaled_left_side_bearing, cast(f32) iy1) / vec2(w, h);
                            packed_glyph->image.scale = vec2(rcp_oversample_amount, rcp_oversample_amount);

                            u32 pitch = sizeof(u32)*packed_glyph->image.w;
                            u32 out_glyph_size = packed_glyph->image.h*pitch;
                            u8* out_glyph = cast(u8*) push_size(&global_arena, out_glyph_size);

                            u8* source = stb_glyph;
                            u8* dest_row = out_glyph + (packed_glyph->image.h - 1)*pitch;
                            for (u32 y = 0; y < packed_glyph->image.h; y++) {
                                u32* dest_pixel = cast(u32*) dest_row;
                                for (u32 x = 0; x < packed_glyph->image.w; x++) {
                                    u8 alpha = *source++;
#if GAMMA_CORRECT_FONTS
                                    u8 color = cast(u8) (255.0f*square_root(cast(f32) alpha / 255.0f));
#else
                                    u8 color = alpha;
#endif
                                    *dest_pixel++ = (alpha << 24) | (color << 16) | (color << 8) | (color << 0);
                                }
                                dest_row -= pitch;
                            }
                            fwrite(out_glyph, out_glyph_size, 1, out);

                            end_temporary_memory(glyph_temp);
                        }

                        u32 end_position = ftell(out);
                        fseek(out, kerning_table_position, SEEK_SET);
                        fwrite(kerning_table, kerning_table_size, 1, out);
                        fseek(out, end_position, SEEK_SET);

                        fprintf(stderr, "Packed %d glyphs for '%s'\n", glyph_count, asset_desc->asset_name);
                    } break;

                    case AssetDataType_Midi: {
                        packed->type = AssetType_Midi;

                        assert(asset_desc->bpm);

                        f64 running_sample_index = 0.0f;

                        packed->midi.ticks_per_second = AUDIO_SAMPLE_RATE;
                        packed->midi.time_signature_numerator = 4;
                        packed->midi.time_signature_denominator = 4;

                        f64 samples_per_microsecond = cast(f32) packed->midi.ticks_per_second / 1000000.0f;
                        u32 ticks_per_quarter_note = 0;
                        u32 microseconds_per_quarter_note = round_f32_to_u32(1000000.0f*(60.0f / asset_desc->bpm));

                        f64 samples_per_delta_time = 0.0f;

                        MidiStream stream;
                        stream.at = cast(u8*) file.data;
                        stream.stop = stream.at + file.size;

                        LinearBuffer<MidiEvent>* events = begin_linear_buffer<MidiEvent>(&global_arena);

                        while (stream.at < stream.stop) {
                            MidiChunk chunk = midi_read_chunk(&stream);

                            MidiStream data;
                            data.at = stream.at;
                            data.stop = data.at + chunk.length;

                            switch (chunk.type) {
                                case MIDI_TYPE("MThd"): {
                                    MidiHeader midi_header;
                                    midi_header.format = midi_read_u16(&data);
                                    midi_header.ntrcks = midi_read_u16(&data);
                                    midi_header.division = midi_read_u16(&data);

                                    // @TODO: Handle invalid formats mildly more elegantly.
                                    assert(midi_header.format == 0);
                                    assert(midi_header.ntrcks == 1);

                                    if ((midi_header.division >> 15) & 0x1) {
                                        NOT_IMPLEMENTED;
                                    } else {
                                        ticks_per_quarter_note = midi_header.division;
                                        samples_per_delta_time = samples_per_microsecond*(cast(f64) microseconds_per_quarter_note / cast(f64) ticks_per_quarter_note);
                                    }
                                } break;

                                case MIDI_TYPE("MTrk"): {
                                    u32 channel = 0;
                                    u32 running_status = 0;

                                    while (data.at < data.stop) {
                                        u32 delta_time = midi_read_variable_length_quantity(&data);

                                        u8 byte = midi_read_u8(&data);
                                        if (byte & 0x80) {
                                            // @Note: Status Byte
                                            if (byte == 0xFF) {
                                                // @Note: MIDI meta event.
                                                u8 event = midi_read_u8(&data);
                                                u32 length = midi_read_variable_length_quantity(&data);
                                                u8* expected_end_position = data.at + length;
                                                switch (event) {
                                                    case 0x51: {
                                                        // @Note: Tempo Change
                                                        microseconds_per_quarter_note = midi_read_u24(&data);
                                                        samples_per_delta_time = samples_per_microsecond*(cast(f64) microseconds_per_quarter_note / cast(f64) ticks_per_quarter_note);
                                                    } break;

                                                    case 0x58: {
                                                        // @Note: Time Signature Change
                                                        u8 numerator = midi_read_u8(&data);
                                                        u8 negative_power_for_denominator = midi_read_u8(&data);
                                                        u8 midi_clocks_per_metronome_click = midi_read_u8(&data);
                                                        u8 notated_32nd_notes_per_quarter_note = midi_read_u8(&data);

                                                        packed->midi.time_signature_numerator = numerator;
                                                        packed->midi.time_signature_denominator = 1 << negative_power_for_denominator;
                                                    } break;

                                                    case 0x2F: {
                                                        // @Note: End of Track
                                                        goto done_parsing_track;
                                                    } break;

                                                    default: {
                                                        // @Note: Skipping unhandled messages
                                                        data.at += length;
                                                    } break;
                                                }
                                                assert(data.at == expected_end_position);
                                            } else {
                                                // @Note: Midi Channel Message
                                                channel = byte & 0x0F;
                                                running_status = byte & 0xF0;

                                                byte = midi_read_u8(&data);
                                                goto parse_data_bytes;
                                            }
                                        } else {
                                            // @Note: Data Byte
parse_data_bytes:
                                            MidiEvent event = {};

                                            f64 delta_time_in_samples = samples_per_delta_time * cast(f64) delta_time;
                                            running_sample_index += delta_time_in_samples;

                                            b32 handled = true;
                                            switch (running_status) {
                                                case 0x80: {
                                                    // @Note: Note-Off
                                                    event.type = cast(u8) MidiEvent_NoteOff;
                                                    event.note_value = byte;
                                                    event.velocity = midi_read_u8(&data);
                                                } break;

                                                case 0x90: {
                                                    // @Note: Note-On
                                                    event.type = cast(u8) MidiEvent_NoteOn;
                                                    event.note_value = byte;
                                                    event.velocity = midi_read_u8(&data);

                                                    if (event.velocity == 0) {
                                                        event.type = cast(u8) MidiEvent_NoteOff;
                                                    }
                                                } break;

                                                default: {
                                                    handled = false;
                                                } break;
                                            }

                                            if (handled) {
                                                event.absolute_time_in_ticks = safe_truncate_u64_u32(round_f64_to_u64(running_sample_index));
                                                event.channel = cast(u8) channel;

                                                lb_add(events, event);
                                            }
                                        }
                                    }
done_parsing_track:
                                    // @Note: Since we only support single track midi files, we'll jump out right here and now.
                                    goto done_parsing_midi;
                                } break;

                                default: {
                                    // @Note: Skipping unknown chunks
                                    data.at = data.stop;
                                } break;
                            }

                            assert(data.at == data.stop);
                            stream.at = data.at;
                        }
done_parsing_midi:
                        packed->midi.beats_per_minute = asset_desc->bpm;

                        end_linear_buffer(events);
                        packed->midi.event_count = cast(u32) events->count;

                        u32 midi_events_size = sizeof(MidiEvent)*packed->midi.event_count;
                        fwrite(events->data, midi_events_size, 1, out);

                        fprintf(stderr, "Packed midi track '%s' from '%s' (events: %d)\n", asset_desc->asset_name, asset_desc->source_file, packed->midi.event_count);
                    } break;

                    case AssetDataType_Soundtrack: {
                        packed->type = AssetType_Soundtrack;
                        fwrite(asset_desc->midi_tracks, sizeof(MidiID)*packed->soundtrack.midi_track_count, 1, out);
                        fprintf(stderr, "Packed soundtrack track '%s' (midi tracks: %d)\n", asset_desc->asset_name, packed->soundtrack.midi_track_count);
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
                INVALID_CODE_PATH;
            }

            end_temporary_memory(temp_mem);
        }

        fseek(out, header.asset_catalog, SEEK_SET);
        fwrite(asset_catalog, sizeof(PackedAsset), packed_asset_count, out);
    }

    check_arena(&global_arena);
}

#if 0
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

