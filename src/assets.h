#ifndef ASSETS_H
#define ASSETS_H

struct Bitmap {
    u32 w, h;
    u32 pitch;
    void* pixels;
};

struct Sound {
    u32 channel_count;
    u32 sample_count;
    u32 sample_rate;
    s16* samples[2];
};

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

#endif /* ASSET_LOADING_H */
