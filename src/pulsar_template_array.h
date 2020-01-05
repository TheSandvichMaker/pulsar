#ifndef TEMPLATE_ARRAY_H
#define TEMPLATE_ARRAY_H

template <typename T>
struct Array {
    Allocator allocator;
    T* data;
    size_t capacity;
    size_t count;
};

#define FOR_ARRAY(array, body) for (size_t i = 0; i < array.count; i++) { auto it = array.data + i; body }
template <typename T>
inline Array<T> allocate_array(size_t capacity, Allocator allocator) {
    Array<T> array;
    array.allocator = allocator;
    array.data = cast(T*) allocator.alloc(sizeof(T)*capacity, 0, 0, allocator.user_data, align_no_clear(alignof(T)));
    array.capacity = capacity;
    array.count = 0;
    return array;
}

template <typename T>
inline void maybe_grow_array(Array<T>* array, size_t required_capacity) {
    if (required_capacity > array->capacity) {
        size_t old_capacity = array->capacity;
        size_t new_capacity = old_capacity;
        while (new_capacity < required_capacity) {
            new_capacity *= 2;
        }
        array->data = cast(T*) array->allocator.alloc(sizeof(T)*new_capacity, sizeof(T)*old_capacity, array->data, array->allocator.user_data, align_no_clear(alignof(T)));
        assert(array->data);
        array->capacity = new_capacity;
    }
}

template <typename T>
inline T* array_add(Array<T>* array) {
    maybe_grow_array(array, array->count + 1);
    T* result = array->data + array->count++;
    return result;
}

template <typename T>
inline T* array_add(Array<T>* array, T item) {
    T* slot = array_add(array);
    *slot = item;
    return slot;
}

template <typename T>
inline void array_remove_unordered(Array<T>* array, size_t index) {
    array->data[index] = array->data[--array->count];
}

template <typename T>
inline size_t array_count_remaining(Array<T>* array) {
    size_t result = (array->capacity - array->count);
    return result;
}

template <typename T>
inline void clear_array(Array<T>* array) {
    array->count = 0;
}

#endif /* TEMPLATE_ARRAY_H */
