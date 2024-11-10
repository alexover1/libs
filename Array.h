#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <cstddef>
#include <cstring>
#include <iterator>

template<typename T>
struct Iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    Iterator(pointer ptr) : ptr(ptr) {}

    reference operator*() const { return *ptr; }
    pointer operator->() { return ptr; }
    Iterator& operator++() { ptr++; return *this; }
    Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
    friend bool operator== (const Iterator& a, const Iterator& b) { return a.ptr == b.ptr; }
    friend bool operator!= (const Iterator& a, const Iterator& b) { return a.ptr != b.ptr; }

protected:
    T *ptr;
};

template<typename T>
struct Slice {
    size_t count;
    T *data;

    T& operator[](size_t index)
    {
        assert(index < count);
        return data[index];
    }

    Iterator<T> begin() { return Iterator<T>(data); }
    Iterator<T> end() { return Iterator<T>(&data[count]); }
};

template<typename T>
struct Array {
    T *data;
    size_t count;
    size_t capacity;

    T& operator[](size_t index)
    {
        assert(index < count);
        return data[index];
    }

    Iterator<T> begin() { return Iterator<T>(data); }
    Iterator<T> end() { return Iterator<T>(&data[count]); }
};

template<typename T>
inline void array_maybe_grow(Array<T>& array, size_t amount)
{
    if (array.count + amount <= array.capacity) return;

    size_t new_capacity = array.capacity > 0 ? array.capacity * 2 : 4;
    T *new_data = new T[new_capacity];
    memcpy(new_data, array.data, array.count * sizeof(T));
    if (array.data) delete[] array.data;
    array.data = new_data;
    array.capacity = new_capacity;
}

template<typename T>
inline void array_add(Array<T>& array, T item)
{
    array_maybe_grow(array, 1);
    array.data[array.count] = item;
    array.count += 1;
}

template<typename T>
inline int array_find(Array<T>& array, T item)
{
    for (size_t i = 0; i < array.count; i++) {
        if (array.data[i] == item) {
            return (int)(i);
        }
    }
    return -1;
}

template<typename T>
inline Slice<T> array_slice(Array<T>& array, size_t start, size_t end)
{
    size_t count = start - end;
    assert(array.count <= count);
    return {count, array.data + start};
}

template<typename T>
inline Slice<T> array_slice(Array<T>& array, size_t start = 0)
{
    return array_slice(array, start, array.count);
}

#endif // ARRAY_H
