/*
 * @TODO:
 * Handle audio more flexibly, don't enforce stereo or 48khz.
 */

internal Bitmap load_bitmap(char* file_name) {
    Bitmap result = {};

    EntireFile read_result = read_entire_file(file_name);
    if (read_result.size != 0) {
        BitmapHeader* header = (BitmapHeader*)read_result.data;

        assert(header->size >= sizeof(header));
        assert(header->height >= 0);
        assert(header->compression == 3);

        u32* pixels = (u32*)((u8*)read_result.data + header->bitmap_offset);
        result.pixels = pixels;
        result.w  = header->width;
        result.h = header->height;

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

        s32 alpha_shift_down = (s32)alpha_scan.index;
        s32 red_shift_down   = (s32)red_scan.index;
        s32 green_shift_down = (s32)green_scan.index;
        s32 blue_shift_down  = (s32)blue_scan.index;

        s32 alpha_shift_up = 24;
        s32 red_shift_up   = 16;
        s32 green_shift_up =  8;
        s32 blue_shift_up  =  0;

        u32* source_dest = pixels;
        for (s32 i = 0; i < header->width * header->height; i++) {
            u32 c = *source_dest;
            f32 texel_r = (f32)((c & red_mask) >> red_shift_down);
            f32 texel_g = (f32)((c & green_mask) >> green_shift_down);
            f32 texel_b = (f32)((c & blue_mask) >> blue_shift_down);
            f32 texel_a = (f32)((c & alpha_mask) >> alpha_shift_down);

            f32 rcp_255 = 1.0f/255.0f;
            texel_a = rcp_255*texel_a;
            texel_r = 255.0f*square_root(square(rcp_255*texel_r) * texel_a);
            texel_g = 255.0f*square_root(square(rcp_255*texel_g) * texel_a);
            texel_b = 255.0f*square_root(square(rcp_255*texel_b) * texel_a);
            texel_a *= 255.0f;

            *source_dest++ = (u32)(texel_a + 0.5f) << alpha_shift_up |
                             (u32)(texel_r + 0.5f) << red_shift_up   |
                             (u32)(texel_g + 0.5f) << green_shift_up |
                             (u32)(texel_b + 0.5f) << blue_shift_up;
        }

    }

    s32 bytes_per_pixel = 4;
    result.pitch = result.w * bytes_per_pixel;

#if 0
    result.pitch = -result.width * bytes_per_pixel;
    result.memory = (u8*)result.memory - result.pitch*(result.height - 1);
#endif

    return result;
}

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
    WaveChunk* chunk = (WaveChunk*)iter.at;
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
    WaveChunk* chunk = (WaveChunk*)iter.at;
    u32 result = chunk->id;
    return result;
}

inline u32 get_chunk_data_size(RiffIterator iter) {
    WaveChunk* chunk = (WaveChunk*)iter.at;
    u32 result = chunk->size;
    return result;
}

internal Sound load_wav(char* file_name) {
    Sound result = {};

    EntireFile read_result = read_entire_file(file_name);
    if (read_result.size != 0) {
        WaveHeader* header = (WaveHeader*)read_result.data;
        assert(header->riff_id == WaveChunkID_RIFF);
        assert(header->wave_id == WaveChunkID_WAVE);

        u32 channel_count = 0;
        s16* sample_data = 0;
        u32 sample_data_size = 0;
        for (RiffIterator iter = parse_chunk_at(header + 1, (u8*)(header + 1) + header->size - 4);
             is_valid(iter);
             iter = next_chunk(iter)
        ) {
            switch (get_type(iter)) {
                case WaveChunkID_fmt: {
                    WaveFmt* fmt = (WaveFmt*)get_chunk_data(iter);
                    assert(fmt->format_tag == 1); // NOTE: We only support PCM
                    assert(fmt->num_channels == 2);
                    assert(fmt->samples_per_sec == 48000); // hmm!!
                    assert(fmt->bits_per_sample == 16);
                    assert(fmt->block_align == sizeof(u16)*fmt->num_channels);
                    channel_count = fmt->num_channels;
                } break;

                case WaveChunkID_data: {
                    sample_data = (s16*)get_chunk_data(iter);
                    sample_data_size = get_chunk_data_size(iter);
                } break;
            }
        }

        assert(channel_count && sample_data);
        assert(channel_count == 2);

        result.channel_count = channel_count;
        result.sample_count = sample_data_size / (channel_count*sizeof(u16));
        // result.samples[0] = sample_data;

        // @TODO: Handle deinterlacing offline or at least with user provided
        // temporary memory?
        result.samples[0] = cast(s16*) malloc(sample_data_size);
        result.samples[1] = result.samples[0] + result.sample_count;
        for (u32 sample_index = 0; sample_index < result.sample_count; sample_index++) {
            result.samples[0][sample_index] = sample_data[sample_index*2];
            result.samples[1][sample_index] = sample_data[sample_index*2 + 1];
        }

        free(read_result.data);
    }

    return result;
}
