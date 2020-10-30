// @Note: Copied straight from Handmade Hero

struct SortEntry {
    f32 sort_key;
    u32 index;
};

inline u32 sort_key_to_u32(f32 sort_key) {
    u32 result = *(u32*)&sort_key;
    if (result & 0x80000000) {
        result = ~result;
    } else {
        result |= 0x80000000;
    }
    return result;
}

inline void swap(SortEntry* a, SortEntry* b) {
    SortEntry swap = *b;
    *b = *a;
    *a = swap;
}

inline void bubble_sort(u32 count, SortEntry* first) {
    for (u32 outer = 0; outer < count; outer++) {
        b32 list_is_sorted = true;
        for (u32 inner = 0; inner < (count - 1); inner++) {
            SortEntry* entry_a = first + inner;
            SortEntry* entry_b = first + inner + 1;
            if (entry_a->sort_key > entry_b->sort_key) {
                list_is_sorted = false;
                swap(entry_a, entry_b);
            }
        }

        if (list_is_sorted) {
            break;
        }
    }
}

internal void merge_sort(u32 count, SortEntry* first, SortEntry* temp) {
    if (count == 1) {
        // NOTE: No work to do.
    } else if (count == 2) {
        SortEntry* entry_a = first;
        SortEntry* entry_b = first + 1;
        if (entry_a->sort_key > entry_b->sort_key) {
            swap(entry_a, entry_b);
        }
    } else {
        u32 half0 = count / 2;
        u32 half1 = count - half0;

        assert(half0 >= 1);
        assert(half1 >= 1);

        SortEntry* in_half0 = first;
        SortEntry* in_half1 = first + half0;
        SortEntry* end = first + count;

        merge_sort(half0, in_half0, temp);
        merge_sort(half1, in_half1, temp);

        SortEntry* read_half0 = in_half0;
        SortEntry* read_half1 = in_half1;

        SortEntry* out = temp;
        for (u32 index = 0; index < count; index++) {
            if (read_half0 == in_half1) {
                *out++ = *read_half1++;
            } else if (read_half1 == end) {
                *out++ = *read_half0++;
            } else if (read_half0->sort_key < read_half1->sort_key) {
                *out++ = *read_half0++;
            } else {
                *out++ = *read_half1++;
            }
        }
        assert(out == (temp + count));
        assert(read_half0 == in_half1);
        assert(read_half1 == end);

        // TODO: Not really necessary if we ping-pong
        for (u32 index = 0; index < count; index++) {
            first[index] = temp[index];
        }
    }
}

internal void radix_sort(u32 count, SortEntry* first, SortEntry* temp) {
    SortEntry* source = first;
    SortEntry* dest = temp;
    for (u32 byte_index = 0; byte_index < 32; byte_index += 8) {
        u32 sort_key_offsets[256] = {};

        // NOTE: First pass - count how many of each key
        for (u32 index = 0; index < count; index++) {
            u32 radix_value = sort_key_to_u32(source[index].sort_key);
            u32 radix_piece = (radix_value >> byte_index) & 0xFF;
            sort_key_offsets[radix_piece]++;
        }

        // NOTE: Convert counts to offsets
        u32 total = 0;
        for (u32 sort_key_index = 0; sort_key_index < ARRAY_COUNT(sort_key_offsets); sort_key_index++) {
            u32 piece_count = sort_key_offsets[sort_key_index];
            sort_key_offsets[sort_key_index] = total;
            total += piece_count;
        }

        // NOTE: Second pass - place elements into the right location
        for (u32 index = 0; index < count; index++) {
            u32 radix_value = sort_key_to_u32(source[index].sort_key);
            u32 radix_piece = (radix_value >> byte_index) & 0xFF;
            dest[sort_key_offsets[radix_piece]++] = source[index];
        }

        SortEntry* swap_temp = dest;
        dest = source;
        source = swap_temp;
    }
}
