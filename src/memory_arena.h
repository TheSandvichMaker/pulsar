#ifndef MEMORY_ARENA_H
#define MEMORY_ARENA_H

struct MemoryArena {
    size_t size;
    size_t used;
    u8* base_ptr;

    s32 temp_count;
};

struct TemporaryMemory {
    MemoryArena* arena;
    size_t used;
};

inline void initialize_arena(MemoryArena* arena, size_t size, void* base_ptr) {
    arena->size = size;
    arena->base_ptr = (u8*)base_ptr;
    arena->used = 0;
    arena->temp_count = 0;
}

enum ArenaPushFlags {
    ArenaFlag_ClearToZero = 0x1,
};

struct ArenaPushParams {
    u32 flags;
    u32 alignment;
};

inline ArenaPushParams default_arena_params() {
    ArenaPushParams params;
    params.flags = ArenaFlag_ClearToZero;
    params.alignment = 4;
    return params;
}

inline ArenaPushParams align(u32 align, b32 clear) {
    ArenaPushParams params = default_arena_params();
    params.flags = clear ? (params.flags | ArenaFlag_ClearToZero) : (params.flags & ~ArenaFlag_ClearToZero);
    params.alignment = align;
    return params;
}

inline ArenaPushParams no_clear() {
    ArenaPushParams params = default_arena_params();
    params.flags &= ~ArenaFlag_ClearToZero;
    return params;
}

inline ArenaPushParams align_no_clear(u32 align) {
    ArenaPushParams params = default_arena_params();
    params.flags &= ~ArenaFlag_ClearToZero;
    params.alignment = align;
    return params;
}

inline size_t get_alignment_offset(MemoryArena* arena, size_t align, size_t at) {
    size_t result_pointer = (size_t)arena->base_ptr + at;

    size_t align_offset = result_pointer & (align - 1);
    if (align_offset) {
        align_offset = align - align_offset;
    }

    return align_offset;
}

inline size_t get_alignment_offset(MemoryArena* arena, size_t align) {
    size_t align_offset = get_alignment_offset(arena, align, arena->used);
    return align_offset;
}

inline size_t get_arena_size_remaining(MemoryArena* arena, ArenaPushParams params = default_arena_params()) {
    size_t result = arena->size - (arena->used + get_alignment_offset(arena, params.alignment));
    return result;
}

#define ZERO_STRUCT(instance) zero_size(sizeof(instance), &(instance))
#define ZERO_ARRAY(count, pointer) zero_size(count*sizeof((pointer)[0]), pointer)
inline void* zero_size(size_t size, void* ptr) {
    // TODO: Check this for performance
    u8* byte = (u8*)ptr;
    while (size--) {
        *byte++ = 0;
    }
    return ptr;
}

inline size_t get_effective_size_for(MemoryArena* arena, size_t size, ArenaPushParams params) {
    size_t align_offset  = get_alignment_offset(arena, params.alignment);
    size_t adjusted_size = size + align_offset;
    return adjusted_size;
}

inline b32 arena_has_room_for(MemoryArena* arena, size_t size, ArenaPushParams params = default_arena_params()) {
    size_t effective_size = get_effective_size_for(arena, size, params);
    b32 result = (arena->used + effective_size) <= arena->size;
    return result;
}

inline void* get_next_allocation_location(MemoryArena* arena, size_t align = 4) {
    size_t align_offset = get_alignment_offset(arena, align);
    void* result = cast(void*) (arena->base_ptr + arena->used + align_offset);
    return result;
}

// TODO: Optional "clear" parameter
#define push_struct(arena, type, ...) (type*)push_size_(arena, sizeof(type), ##__VA_ARGS__)
#define push_array(arena, count, type, ...) (type*)push_size_(arena, sizeof(type) * (count), ##__VA_ARGS__)
#define push_size(arena, size, ...) push_size_(arena, size, ##__VA_ARGS__)
// #define ZERO_AND_push_struct(arena, type, ...) (type*)zero_size(sizeof(type), push_size_(arena, sizeof(type), ##__VA_ARGS__))
inline void* push_size_(MemoryArena* arena, size_t size_init, ArenaPushParams params = default_arena_params()) {
    size_t size = get_effective_size_for(arena, size_init, params);

    assert((arena->used + size) <= arena->size);
    size_t align_offset = get_alignment_offset(arena, params.alignment);
    void* result = (arena->base_ptr + arena->used + align_offset);
    arena->used += size;

    if (params.flags & ArenaFlag_ClearToZero) {
        zero_size(size_init, result);
    }

    return result;
}

inline char* push_string(MemoryArena* arena, size_t length, char* source) {
    assert((arena->used + length) <= arena->size);

    char* dest = (char*)push_size_(arena, length, align_no_clear(1));
    for (u32 char_index = 0; char_index < length; char_index++) {
        dest[char_index] = source[char_index];
    }

    return dest;
}

inline char* push_string_and_null_terminate(MemoryArena* arena, size_t length, char* source) {
    char* result = push_string(arena, length + 1, source);
    result[length] = '\0';

    return result;
}

inline void sub_arena(MemoryArena* result, MemoryArena* arena, size_t size, ArenaPushParams params = default_arena_params()) {
    result->size = size;
    result->base_ptr = (u8*)push_size_(arena, size, params);
    result->used = 0;
    result->temp_count = 0;
}

inline void copy(size_t size, void* source_init, void* dest_init) {
    u8* source = (u8*)source_init;
    u8* dest = (u8*)dest_init;
    while (size--) { *dest++= *source++; }
}

inline TemporaryMemory begin_temporary_memory(MemoryArena* arena) {
    TemporaryMemory result;

    result.arena = arena;
    result.used = arena->used;

    arena->temp_count++;

    return result;
}

inline void end_temporary_memory(TemporaryMemory temp_mem) {
    MemoryArena* arena = temp_mem.arena;
    arena->used = temp_mem.used;
    assert(arena->temp_count > 0);
    arena->temp_count--;
}

inline void clear_arena(MemoryArena* arena) {
    initialize_arena(arena, arena->size, arena->base_ptr);
}

inline void check_arena(MemoryArena* arena) {
    assert(arena->temp_count == 0);
}

#endif /* MEMORY_ARENA_H */
