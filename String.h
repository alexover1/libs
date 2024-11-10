#ifndef STRING_H
#define STRING_H

#include <cmath>
#include <cctype>
#include <cstddef>
#include <climits>
#include <cfloat>
#include <cassert>
#include <cstring>

#ifndef STRING_NO_PRINT
#include "Print.h"
#endif

struct string {
    size_t count;
    const char *data;

    char operator[](size_t index) const
    {
        assert(index < count);
        return data[index];
    }

    bool operator==(string b)
    {
        if (count != b.count) return false;
        return memcmp(b.data, data, count) == 0;
    }
    bool operator!=(string b)
    {
        if (count != b.count) return true;
        return memcmp(b.data, data, count) != 0;
    }
};

inline string operator ""_sv(const char *data, size_t count)
{
    return {count, data};
}

inline string to_string(const char *cstr)
{
    return {strlen(cstr), cstr};
}

inline string string_trim_left(string s)
{
    size_t i = 0;
    while (i < s.count && isspace(s.data[i])) {
        i += 1;
    }
    return {s.count - i, s.data + i};
}

inline string string_trim_right(string s)
{
    size_t i = 0;
    while (i < s.count && isspace(s.data[s.count - 1 - i])) {
        i += 1;
    }
    return {s.count - i, s.data};
}

inline string string_trim(string s)
{
    return string_trim_right(string_trim_left(s));
}

inline string string_chop_left(string& s, size_t n)
{
    if (n > s.count) n = s.count;

    string result = {n, s.data};

    s.count -= n;
    s.data += n;

    return result;
}

inline string string_chop_right(string& s, size_t n)
{
    if (n > s.count) n = s.count;

    string result = {n, s.data + s.count - n};

    s.count -= n;

    return result;
}

inline bool string_index_of(string s, char c, size_t& index)
{
    size_t i = 0;
    while (i < s.count && s.data[i] != c) {
        i += 1;
    }

    if (i < s.count) {
        index = i;
        return true;
    }

    return false;
}

inline bool string_index_of_from(string s, size_t start, char c, size_t& index)
{
    size_t i = start;
    while (i < s.count && s.data[i] != c) {
        i += 1;
    }

    if (i < s.count) {
        index = i;
        return true;
    }

    return false;
}

inline bool string_try_chop_by_delim(string& s, char delim, string& chunk)
{
    size_t i = 0;
    while (i < s.count && s.data[i] != delim) {
        i += 1;
    }

    string result = {i, s.data};

    if (i < s.count) {
        s.count -= i + 1;
        s.data  += i + 1;

        chunk = result;
        return true;
    }

    return false;
}

inline string string_chop_by_delim(string& s, char delim)
{
    size_t i = 0;
    while (i < s.count && s.data[i] != delim) {
        i += 1;
    }

    string result = {i, s.data};

    if (i < s.count) {
        s.count -= i + 1;
        s.data  += i + 1;
    } else {
        s.count -= i;
        s.data  += i;
    }

    return result;
}

enum Parse_Error {
    PARSE_ERROR_NONE = 0,
    PARSE_ERROR_INVALID_CHAR,
    PARSE_ERROR_OVERFLOW,
};

inline Parse_Error parse_u64(string s, unsigned long long& value)
{
    value = 0;

    for (size_t i = 0; i < s.count; i++) {
        unsigned char c = (unsigned char)(s.data[i]);
        if (c < '0' || c > '9') return PARSE_ERROR_INVALID_CHAR;

        if (value > (ULLONG_MAX / 10) || (value == (ULLONG_MAX / 10) && (c - '0') > (ULLONG_MAX % 10))) {
            return PARSE_ERROR_OVERFLOW;
        }

        value = value * 10 + (c - '0');
    }

    return PARSE_ERROR_NONE;
}

inline Parse_Error parse_f64(string s, double& value)
{
    value = 0.0;

    size_t i = 0;

    // Parse the integer part before the decimal point
    bool has_digits = false;
    while (i < s.count && isdigit(s.data[i])) {
        value = value * 10 + (s.data[i] - '0');
        i += 1;
        has_digits = true;
    }

    if (!has_digits && (i == s.count || (s.data[i] != '.' && s.data[i] != 'e' && s.data[i] != 'E'))) {
        return PARSE_ERROR_INVALID_CHAR;
    }

    if (i < s.count && s.data[i] == '.') {
        i += 1;
        double fractional_part = 0.1;
        bool fractional_digits = false;

        while (i < s.count && isdigit(s.data[i])) {
            value += (s.data[i] - '0') * fractional_part;
            fractional_part /= 10;
            i += 1;
            fractional_digits = true;
        }

        if (!fractional_digits && (i == s.count || (s.data[i] != 'e' && s.data[i] != 'E'))) {
            return PARSE_ERROR_INVALID_CHAR;
        }
    }

    if (i < s.count && (s.data[i] == 'e' || s.data[i] == 'E')) {
        i += 1;

        bool exp_negative = false;

        if (i < s.count && (s.data[i] == '-' || s.data[i] == '+')) {
            exp_negative = (s.data[i] == '-');
            i += 1;
        }

        int exponent = 0;
        while (i < s.count && isdigit(s.data[i])) {
            exponent = exponent * 10 + (s.data[i] - '0');
            i += 1;
        }

        if (exponent > 308) {
            return PARSE_ERROR_OVERFLOW;
        }

        value *= pow(10, exp_negative ? -exponent : exponent);
    }

    if (i != s.count) {
        return PARSE_ERROR_INVALID_CHAR;
    }

    if (value > DBL_MAX || value < -DBL_MAX) {
        return PARSE_ERROR_OVERFLOW;
    }

    if (value != 0.0 && (value < DBL_MIN && value > -DBL_MIN)) {
        return PARSE_ERROR_OVERFLOW;
    }

    return PARSE_ERROR_NONE;
}

#ifndef STRING_NO_PRINT

inline void sprint1(String_Buffer *buffer, string s)
{
    int n = snprintf(
                buffer->data + buffer->length,
                buffer->capacity - buffer->length,
                "%.*s", (int)(s.count), s.data);
    buffer->length = min(buffer->length + n, buffer->capacity - 1);
}

#endif // STRING_NO_PRINT

#endif // STRING_H
