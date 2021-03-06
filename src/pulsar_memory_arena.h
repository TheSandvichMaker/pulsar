#ifndef PULSAR_MEMORY_ARENA_H
#define PULSAR_MEMORY_ARENA_H

#define MEMORY_ARENA_DEFAULT_ALIGN 4

struct MemoryArena {
    size_t size;
    size_t used;
    u8* base_ptr;

    s32 temp_count;
    void* active_linear_buffer;
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
    arena->active_linear_buffer = 0;
}

inline size_t get_alignment_offset(MemoryArena* arena, size_t align, size_t at) {
    size_t result_pointer = cast(size_t) arena->base_ptr + at;

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

inline size_t get_arena_size_remaining(MemoryArena* arena, AllocateParams params = default_allocate_params()) {
    size_t result = arena->size - (arena->used + get_alignment_offset(arena, align_or(params, MEMORY_ARENA_DEFAULT_ALIGN)));
    return result;
}

// @TODO: Isn't zero_struct redundant given you can do *instance = {}? I guess if you want to intentionally not initialize to default values it's got a purpose.
#define zero_struct(instance) zero_size(sizeof(instance), &(instance))
#define zero_array(count, pointer) zero_size(count*sizeof((pointer)[0]), pointer)
inline void* zero_size(size_t size, void* ptr) {
    // TODO: Check this for performance
    u8* byte = (u8*)ptr;
    while (size--) {
        *byte++ = 0;
    }
    return ptr;
}

inline size_t get_effective_size_for(MemoryArena* arena, size_t size, AllocateParams params) {
    size_t align_offset  = get_alignment_offset(arena, align_or(params, MEMORY_ARENA_DEFAULT_ALIGN));
    size_t adjusted_size = size + align_offset;
    return adjusted_size;
}

inline b32 arena_has_room_for(MemoryArena* arena, size_t size, AllocateParams params = default_allocate_params()) {
    size_t effective_size = get_effective_size_for(arena, size, params);
    b32 result = (arena->used + effective_size) <= arena->size;
    return result;
}

inline void* get_next_allocation_location(MemoryArena* arena, size_t align = MEMORY_ARENA_DEFAULT_ALIGN) {
    size_t align_offset = get_alignment_offset(arena, align);
    void* result = cast(void*) (arena->base_ptr + arena->used + align_offset);
    return result;
}

// TODO: Optional "clear" parameter
#define push_struct(arena, type, ...) (type*)push_size_(arena, sizeof(type), ##__VA_ARGS__)
#define push_array(arena, count, type, ...) (type*)push_size_(arena, sizeof(type) * (count), ##__VA_ARGS__)
#define push_size(arena, size, ...) push_size_(arena, size, ##__VA_ARGS__)
inline void* push_size_(MemoryArena* arena, size_t size_init, AllocateParams params = default_allocate_params(), void* lb = 0) {
    assert(arena->active_linear_buffer == lb);

    s32 align = align_or(params, MEMORY_ARENA_DEFAULT_ALIGN);

    size_t size = get_effective_size_for(arena, size_init, params);

    assert((arena->used + size) <= arena->size);
    size_t align_offset = get_alignment_offset(arena, align);
    void* result = (arena->base_ptr + arena->used + align_offset);
    arena->used += size;

    if (params.flags & AllocateFlag_ClearToZero) {
        zero_size(size_init, result);
    }

    return result;
}

template <typename T>
struct LinearBuffer {
    MemoryArena* arena;
    u32 flags;
    size_t count;
    T* data;
};

template <typename T>
inline LinearBuffer<T>* begin_linear_buffer(MemoryArena* arena, AllocateParams params = no_clear()) {
    assert(!arena->active_linear_buffer);

    // @TODO: Make it so that header->data is aligned to the given params.align_
    LinearBuffer<T>* header = push_struct(arena, LinearBuffer<T>, align(params.align_, false));
    header->arena = arena;
    header->flags = params.flags;
    header->count = 0;
    header->data = cast(T*) (header + 1);

    arena->active_linear_buffer = header;

    return header;
}

template <typename T>
inline T* lb_push_n(LinearBuffer<T>* buf, size_t n) {
    T* result = push_array(buf->arena, n, T, align(1, buf->flags), buf);
    buf->count += n;
    return result;
}

template <typename T>
inline T* lb_push(LinearBuffer<T>* buf) {
    T* result = lb_push_n(buf, 1);
    return result;
}

template <typename T>
inline T* lb_add(LinearBuffer<T>* buf, T item) {
    T* result = lb_push(buf);
    *result = item;
    return result;
}

template <typename T>
inline void end_linear_buffer(LinearBuffer<T>* buf) {
    assert(buf->arena->active_linear_buffer == buf);

    buf->arena->active_linear_buffer = 0;
    buf->arena = 0;
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

inline void sub_arena(MemoryArena* result, MemoryArena* arena, size_t size, AllocateParams params = default_allocate_params()) {
    result->size = size;
    result->base_ptr = (u8*)push_size_(arena, size, params);
    result->used = 0;
    result->temp_count = 0;
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
    assert(arena->active_linear_buffer == 0);
}

// @Note: A good citizen would put this in a .cpp
internal ALLOCATOR(arena_allocator) {
    MemoryArena* arena = cast(MemoryArena*) user_data;

    void* result = 0;

    if (new_size) {
        if (old_size) {
            assert(old_ptr);
            assert(new_size >= old_size);
            if (cast(u8*) old_ptr + old_size == get_next_allocation_location(arena, align_or(params, MEMORY_ARENA_DEFAULT_ALIGN))) {
                // @Note: If we can grow in place, we just push the difference and return the old_ptr
                push_size(arena, new_size - old_size, params);
                result = old_ptr;
            } else {
                result = push_size(arena, new_size, params);
                copy(old_size, old_ptr, result);
            }
        } else {
            result = push_size(arena, new_size, params);
        }
    }

    return result;
}

#endif /* PULSAR_MEMORY_ARENA_H */
