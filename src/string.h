#ifndef SND_STRING_H
#define SND_STRING_H

// @Note: Exclusive of the null terminator
inline size_t cstr_length(char* cstr) {
    size_t length = 0;
    while (*cstr++ != 0) {
        length++;
    }
    return length;
}

inline b32 is_space(char c) {
    b32 result = false;
    if (c == ' ' || c == '\t') {
        result = true;
    }
    return result;
}

inline b32 is_newline(char c) {
    b32 result = false;
    if (c == '\r' || c == '\n') {
        result = true;
    }
    return result;
}

inline b32 is_whitespace(char c) {
    b32 result = is_space(c) || is_newline(c);
    return result;
}

inline b32 is_numeric(char c) {
    b32 result = (c >= '0' && c <= '9');
    return result;
}

inline b32 is_alphabetic(char c) {
    b32 result = ((c >= 'a' && c <= 'z') ||
                  (c >= 'A' && c <= 'Z'));
    return result;
}

inline b32 is_alphanumeric(char c) {
    b32 result = is_alphabetic(c) || is_numeric(c);
    return result;
}

inline s32 parse_integer(char* start, char** end) {
    s32 result = 0;

    b32 negate = false;

    *end = start;
    char* end_of_number = start;
    if (start[0] == '-') {
        negate = true;
        end_of_number++;
    } else if (start[0] == '+') {
        end_of_number++;
    }

    while (end_of_number[1] && (end_of_number[1] >= '0' && end_of_number[1] <= '9')) {
        end_of_number++;
    }

    b32 successfully_parsed_number = false;
    u32 exponent = 1;
    for (char* scan = end_of_number; (scan >= start) && (scan[0] >= '0' && scan[0] <= '9'); scan--) {
        successfully_parsed_number = true;
        result += (scan[0] - '0') * exponent;
        exponent *= 10;
    }

    if (successfully_parsed_number) {
        *end = end_of_number + 1;
    }

    if (negate) {
        result = -result;
    }

    return result;
}

#define PRINTF_STRING(string) cast(int) (string).len, (string).data
typedef struct String {
    size_t len;
    char* data;
} String;

inline String wrap_string(size_t length, char* chars) {
    String result;
    result.len = length;
    result.data = chars;
    return result;
}

inline String wrap_cstr(char* cstr) {
    String result;
    result.len  = cstr_length(cstr);
    result.data = cstr;
    return result;
}

inline String string_from_two_pointers(char* ptr1, char* ptr2) {
    // NOTE: This function is exclusive, so the character ptr2 is pointing at
    // is not included in the resulting string.
    String result;
    result.len  = (size_t)(ptr2 - ptr1);
    result.data = ptr1;
    return result;
}

inline b32 strings_are_equal(String a, String b) {
    b32 result = false;

    if (a.len == b.len) {
        result = true;
        for (u32 i = 0; i < a.len; i++) {
            if (*a.data++ != *b.data++) {
                result = false;
                break;
            }
        }
    }

    return result;
}

// @Speed? Getting the cstr length with wrap_cstr first and doing the
// comparison second is probably wasteful.
inline b32 strings_are_equal(String a, char* b) {
    b32 result = strings_are_equal(a, wrap_cstr(b));
    return result;
}

inline b32 strings_are_equal(char* a, String b) {
    b32 result = strings_are_equal(wrap_cstr(a), b);
    return result;
}

inline b32 strings_are_equal(char* a, char* b) {
    b32 result = strings_are_equal(wrap_cstr(a), wrap_cstr(b));
    return result;
}

inline b32 in_range(String* string, char* ptr) {
    b32 result = (ptr >= string->data && ptr <= (string->data + string->len));
    return result;
}

inline String substring(String* source, size_t start, size_t end) {
    assert(end >= start);
    assert(end < source->len);

    String result = wrap_string(end - start + 1, source->data + start);

    return result;
}

inline size_t chars_left(String* string) {
    size_t result = string->len > 0;
    return result;
}

inline char peek(String* string) {
    if (chars_left(string)) {
        return string->data[0];
    } else {
        return 0;
    }
}

inline char peek_by(String* string, u32 by) {
    if (by < string->len) {
        return string->data[by];
    } else {
        return 0;
    }
}

inline char advance(String* string) {
    if (chars_left(string)) {
        string->data++;
        string->len--;
        return string->data[-1];
    } else {
        return 0;
    }
}

inline String advance_by(String* string, u32 by) {
    if (by < string->len) {
        String result = wrap_string(by, string->data);
        string->data += by;
        string->len -= by;
        return result;
    } else {
        String result = {};
        return result;
    }
}

inline void eat_newline(String* string) {
    if (peek(string) == '\r') {
        advance(string);
    }

    if (peek(string) == '\n') {
        advance(string);
    }
}

inline b32 match_string(String* string, char* substring) {
    b32 match = true;
    u32 string_length = 0;
    for (u32 char_index = 0; *substring; char_index++) {
        if ((s32)string->len - char_index > 0 && string->data[char_index] == *substring++) {
            string_length++;
        } else {
            match = false;
            break;
        }
    }

    if (match) {
        advance_by(string, string_length);
    }

    return match;
}

inline void eat_whitespaces(String* string) {
    while (is_whitespace(peek(string))) {
        advance(string);
    }
}

typedef enum StringAdvanceFlag {
    StringAdvance_StopAtNewline = 0x1,
    StringAdvance_OnlyAdvanceIfFound = 0x2,
} StringAdvanceFlag;

inline b32 advance_to_ptr(String* string, char* target) {
    b32 result = true;
    if (in_range(string, target)) {
        string->len -= target - string->data;
        string->data = target;
    } else {
        result = false;
    }
    return result;
}

inline String advance_to(String* string, char target, u32 flags = 0) {
    String start_string = *string;

    b32 found_target = false;
    char* start = string->data;
    while (chars_left(string)) {
        if (peek(string) == target) {
            found_target = true;
            break;
        } else if (flags & StringAdvance_StopAtNewline && is_newline(peek(string))) {
            break;
        } else {
            advance(string);
        }
    }
    char* end = string->data;

    if (flags & StringAdvance_OnlyAdvanceIfFound) {
        *string = start_string;
    }

    String result = string_from_two_pointers(start, end);
    return result;
}

inline String advance_word(String* string) {
    char* start = 0;
    char* end = 0;
    if (chars_left(string)) {
        while (chars_left(string) && is_whitespace(peek(string))) {
            advance(string);
        }

        start = string->data;
        while (chars_left(string) && !is_whitespace(peek(string))) {
            advance(string);
        }
        end = string->data;

        while (chars_left(string) && is_whitespace(peek(string))) {
            advance(string);
        }
    }

    String result = string_from_two_pointers(start, end);
    return result;
}

inline b32 match_word(String* source_string, char* target, String* out_word = 0) {
    String string = *source_string;
    String word = advance_word(&string);
    b32 result = false;
    if (strings_are_equal(word, target)) {
        result = true;
        *source_string = string;
    }
    if (out_word) {
        *out_word = word;
    }
    return result;
}

inline String advance_line(String* string) {
    char* start = string->data;
    while (!is_newline(peek(string))) {
        advance(string);
    }
    char* end = string->data;

    eat_newline(string);

    String result = string_from_two_pointers(start, end);
    return result;
}

#endif
