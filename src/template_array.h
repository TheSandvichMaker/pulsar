#ifndef TEMPLATE_ARRAY_H
#define TEMPLATE_ARRAY_H

template <typename T>
struct Array {
    MemoryArena* arena;
    T* data;
    size_t capacity;
    size_t count;
};

#define FOR_ARRAY(array, body) for (size_t i = 0; i < array.count; i++) { auto it = array.data + i; body }
template <typename T>
inline Array<T> allocate_array(size_t capacity, MemoryArena* arena) {
    Array<T> array;
    array.arena = arena;
    array.data = push_array(arena, capacity, T, align_no_clear(alignof(T)));
    array.capacity = capacity;
    array.count = 0;
    return array;
}

template <typename T>
inline T* grow_array(Array<T>* array, size_t size) {
    T* next_allocation = (T*)get_next_allocation_location(array->arena, alignof(T));
    if ((array->data + array->capacity) != next_allocation) {
        // NOTE: There's been an allocation that's not us since we last
        // added something. Time to "realloc"!
        T* new_array = push_array(array->arena, array->capacity + size, T, align_no_clear(alignof(T)));
        for (size_t i = 0; i < array->count; i++) {
            new_array[i] = array->data[i];
        }
        array->data = new_array;
    } else {
        push_array(array->arena, size, T, align_no_clear(1)); // NOTE: Grow the array in place.
    }
    array->capacity = array->capacity + size;
    return array->data + array->count;
}

template <typename T>
inline T* array_push_size(Array<T>* array, size_t size) {
    T* result = grow_array(array, size);
    array->count += size;
    return result;
}

template <typename T>
inline T* array_add(Array<T>* array) {
    if ((array->count + 1) > array->capacity) {
        grow_array(array, array->capacity);
    }

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

#endif /* TEMPLATE_ARRAY_H */
