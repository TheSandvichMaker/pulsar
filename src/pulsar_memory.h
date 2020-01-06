#ifndef PULSAR_MEMORY_H
#define PULSAR_MEMORY_H

enum AllocateFlag {
    AllocateFlag_ClearToZero = 0x1,
};

#define ALLOCATOR_ALIGN_DONT_CARE -1
struct AllocateParams {
    s32 align_;
    u32 flags;
};

#define ALLOCATOR(name) void* name(size_t new_size, size_t old_size, void* old_ptr, void* user_data, AllocateParams params)
typedef ALLOCATOR(AllocatorFunction);

struct Allocator {
    AllocatorFunction* alloc;
    void* user_data;
};

inline Allocator allocator(AllocatorFunction* function, void* user_data) {
    Allocator result;
    result.alloc = function;
    result.user_data = user_data;
    return result;
}

inline AllocateParams default_allocate_params() {
    AllocateParams params;
    params.flags = AllocateFlag_ClearToZero;
    params.align_ = ALLOCATOR_ALIGN_DONT_CARE;
    return params;
}

inline AllocateParams align_flags(u32 align, u32 flags) {
    AllocateParams params = default_allocate_params();
    params.flags = flags;
    params.align_ = align;
    return params;
}

inline AllocateParams align(u32 align, b32 clear) {
    AllocateParams params = default_allocate_params();
    params.flags = clear ? (params.flags | AllocateFlag_ClearToZero) : (params.flags & ~AllocateFlag_ClearToZero);
    params.align_ = align;
    return params;
}

inline AllocateParams no_clear() {
    AllocateParams params = default_allocate_params();
    params.flags &= ~AllocateFlag_ClearToZero;
    return params;
}

inline AllocateParams align_no_clear(u32 align) {
    AllocateParams params = default_allocate_params();
    params.flags &= ~AllocateFlag_ClearToZero;
    params.align_ = align;
    return params;
}

inline s32 align_or(AllocateParams params, s32 fallback) {
    s32 result = (params.align_ == ALLOCATOR_ALIGN_DONT_CARE) ? fallback : params.align_;
    return result;
}

inline void* allocate(Allocator allocator, size_t new_size, AllocateParams params = default_allocate_params()) {
    void* result = allocator.alloc(new_size, 0, 0, allocator.user_data, params);
    return result;
}

inline void* reallocate(Allocator allocator, size_t new_size, size_t old_size, void* old_ptr, AllocateParams params = default_allocate_params()) {
    void* result = allocator.alloc(new_size, old_size, old_ptr, allocator.user_data, params);
    return result;
}

inline void deallocate(Allocator allocator, size_t size, void* memory, AllocateParams params = default_allocate_params()) {
    allocator.alloc(0, size, memory, allocator.user_data, params);
}

#endif /* PULSAR_MEMORY_H */
