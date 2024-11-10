#ifndef PRINT_H
#define PRINT_H

#include <iostream>
#include <thread>

#ifndef SNPRINTF
#ifndef STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
#endif
#define SNPRINTF stbsp_snprintf
#endif

struct String_Buffer {
    char *data;      // Pointer to start of data
    size_t length;   // How much we have used
    size_t capacity; // How much we have total
};

template<typename T>
inline T min(T x, T y)
{
    return x < y ? x : y;
}

inline void sprint1(String_Buffer *buffer, const char *cstr)
{
    int n = SNPRINTF(
                buffer->data + buffer->length,
                buffer->capacity - buffer->length,
                "%s", cstr);
    buffer->length = min(buffer->length + n, buffer->capacity - 1);
}

inline void sprint1(String_Buffer *buffer, char c)
{
    int n = SNPRINTF(
                buffer->data + buffer->length,
                buffer->capacity - buffer->length,
                "%c", c);
    buffer->length = min(buffer->length + n, buffer->capacity - 1);
}

inline void sprint1(String_Buffer *buffer, int x)
{
    int n = SNPRINTF(
                buffer->data + buffer->length,
                buffer->capacity - buffer->length,
                "%d", x);
    buffer->length = min(buffer->length + n, buffer->capacity - 1);
}

inline void sprint1(String_Buffer *buffer, long int x)
{
    int n = SNPRINTF(
                buffer->data + buffer->length,
                buffer->capacity - buffer->length,
                "%ld", x);
    buffer->length = min(buffer->length + n, buffer->capacity - 1);
}

inline void sprint1(String_Buffer *buffer, long long int x)
{
    int n = SNPRINTF(
                buffer->data + buffer->length,
                buffer->capacity - buffer->length,
                "%lld", x);
    buffer->length = min(buffer->length + n, buffer->capacity - 1);
}

inline void sprint1(String_Buffer *buffer, unsigned x)
{
    int n = SNPRINTF(
                buffer->data + buffer->length,
                buffer->capacity - buffer->length,
                "%u", x);
    buffer->length = min(buffer->length + n, buffer->capacity - 1);
}

inline void sprint1(String_Buffer *buffer, long unsigned x)
{
    int n = SNPRINTF(
                buffer->data + buffer->length,
                buffer->capacity - buffer->length,
                "%lu", x);
    buffer->length = min(buffer->length + n, buffer->capacity - 1);
}

inline void sprint1(String_Buffer *buffer, long long unsigned x)
{
    int n = SNPRINTF(
                buffer->data + buffer->length,
                buffer->capacity - buffer->length,
                "%llu", x);
    buffer->length = min(buffer->length + n, buffer->capacity - 1);
}

inline void sprint1(String_Buffer *buffer, float f)
{
    int n = SNPRINTF(
                buffer->data + buffer->length,
                buffer->capacity - buffer->length,
                "%f", f);
    buffer->length = min(buffer->length + n, buffer->capacity - 1);
}

inline void sprint1(String_Buffer *buffer, double f)
{
    int n = SNPRINTF(
                buffer->data + buffer->length,
                buffer->capacity - buffer->length,
                "%f", f);
    buffer->length = min(buffer->length + n, buffer->capacity - 1);
}

inline void sprint1(String_Buffer *buffer, bool b)
{
    int n = SNPRINTF(
                buffer->data + buffer->length,
                buffer->capacity - buffer->length,
                b ? "true" : "false");
    buffer->length = min(buffer->length + n, buffer->capacity - 1);
}

template<typename ...Types>
inline void sprint(String_Buffer *buffer, Types... args)
{
    (sprint1(buffer, args), ...);
}

template<typename ...Types>
inline void print(Types... args)
{
    thread_local char data[1024 * 4];

    String_Buffer buffer = {};
    buffer.data = data;
    buffer.capacity = sizeof(data);

    (sprint1(&buffer, args), ...);

    std::cout.write(buffer.data, buffer.length);
    std::cout.flush(); // TODO: Is there a better way?
}

#endif // PRINT_H
