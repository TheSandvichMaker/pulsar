// @TODO: Figure out whether or not I like this style of "templatizing" data structures

#ifndef PULSAR_ARRAY_TYPE
#error PULSAR_ARRAY_TYPE was undefined
#endif

#ifndef PULSAR_ARRAY
#define PULSAR_ARRAY_(type) Array_##type
#define PULSAR_ARRAY(type) PULSAR_ARRAY_(type)
#endif

struct PULSAR_ARRAY(PULSAR_ARRAY_TYPE) {
    Allocator allocator;
    PULSAR_ARRAY_TYPE* data;
    size_t capacity;
    size_t count;
};

inline void allocate_array(PULSAR_ARRAY(PULSAR_ARRAY_TYPE)* array, size_t initial_capacity, Allocator allocator) {
    array->allocator = allocator;
    array->capacity = initial_capacity;
    array->data = cast(PULSAR_ARRAY_TYPE*) allocate(array->allocator, sizeof(PULSAR_ARRAY_TYPE)*array->capacity, align_no_clear(alignof(PULSAR_ARRAY_TYPE)));
    array->count = 0;
}

inline void maybe_grow_array(PULSAR_ARRAY(PULSAR_ARRAY_TYPE)* array, size_t required_capacity) {
    if (required_capacity > array->capacity) {
        size_t old_capacity = array->capacity;
        size_t new_capacity = old_capacity;
        while (new_capacity < required_capacity) {
            new_capacity *= 2;
        }
        array->data = cast(PULSAR_ARRAY_TYPE*) reallocate(array->allocator, sizeof(PULSAR_ARRAY_TYPE)*new_capacity, sizeof(PULSAR_ARRAY_TYPE)*old_capacity, array->data, align_no_clear(alignof(PULSAR_ARRAY_TYPE)));
        assert(array->data);
        array->capacity = new_capacity;
    }
}

inline PULSAR_ARRAY_TYPE* array_add(PULSAR_ARRAY(PULSAR_ARRAY_TYPE)* array) {
    maybe_grow_array(array, array->count + 1);
    PULSAR_ARRAY_TYPE* result = array->data + array->count++;
    return result;
}

inline PULSAR_ARRAY_TYPE* array_add(PULSAR_ARRAY(PULSAR_ARRAY_TYPE)* array, PULSAR_ARRAY_TYPE item) {
    PULSAR_ARRAY_TYPE* slot = array_add(array);
    *slot = item;
    return slot;
}

inline void array_remove_unordered(PULSAR_ARRAY(PULSAR_ARRAY_TYPE)* array, size_t index) {
    array->data[index] = array->data[--array->count];
}

inline size_t array_count_remaining(PULSAR_ARRAY(PULSAR_ARRAY_TYPE)* array) {
    size_t result = (array->capacity - array->count);
    return result;
}

inline void clear_array(PULSAR_ARRAY(PULSAR_ARRAY_TYPE)* array) {
    array->count = 0;
}

#undef PULSAR_ARRAY_TYPE
