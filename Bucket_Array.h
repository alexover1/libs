#ifndef BUCKET_ARRAY_H
#define BUCKET_ARRAY_H

#include <iterator>

template<typename T>
struct Bucket {
    Bucket<T> *next;
    T *data;
    size_t count;
    size_t capacity;

    T& operator[](size_t index)
    {
        assert(index < count);
        return data[index];
    }
};

template<typename T>
struct Bucket_Array {
    Bucket<T> *first;
    Bucket<T> *last;

    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(Bucket<T> *bucket) : bucket(bucket) {}

        reference operator*() const { return bucket->data[slot_index]; }
        pointer operator->() { return &bucket->data[slot_index]; }
        Iterator& operator++()
        {
            if (++slot_index >= bucket->count) {
                bucket = bucket->next;
                slot_index = 0;
            }
            return *this;
        }
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
        friend bool operator== (const Iterator& a, const Iterator& b) { return a.bucket == b.bucket && a.slot_index == b.slot_index; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.bucket != b.bucket || a.slot_index != b.slot_index; };

    protected:
        Bucket<T> *bucket;
        size_t slot_index = 0;
    };

    Iterator begin() { return Iterator(first); }
    Iterator end() { return Iterator(nullptr); }
};

template<typename T>
inline Bucket<T> *bucket_create(size_t capacity)
{
    char *data = new char[sizeof(Bucket<T>) + capacity * sizeof(char)];

    Bucket<T> *new_bucket = (Bucket<T> *)(data);

    new_bucket->data = (T *)(data + sizeof(Bucket<T>));
    new_bucket->count = 0;
    new_bucket->capacity = capacity;
    new_bucket->next = 0;

    return new_bucket;
}

template<typename T>
inline void array_ensure_unfull_bucket(Bucket_Array<T>& array)
{
    if (array.last) {
        if (array.last->count < array.last->capacity) return;
        Bucket<T> *bucket = bucket_create<T>(array.last->capacity * 2);
        array.last->next = bucket;
        array.last = bucket;
    } else {
        Bucket<T> *bucket = bucket_create<T>(4);
        array.first = bucket;
        array.last = bucket;
    }
}

template<typename T>
inline T *array_add(Bucket_Array<T>& array)
{
    array_ensure_unfull_bucket(array);
    Bucket<T> *bucket = array.last;
    T *result = &bucket->data[bucket->count];
    bucket->count += 1;
    return result;
}

#endif // BUCKET_ARRAY_H
