#include <windows.h>

#define TokenType TokenType_

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <stdarg.h>

#include "common.h"
#include "string.h"

struct FoundString {
    u32 start_line;
    u32 end_line;
    u32 start_column;
    u32 end_column;

    String containing_lines;
};

internal FoundString find_substring(String source, String substr) {
    FoundString result = {};
    result.start_line = 1;

    result.containing_lines = wrap_string(0, source.data);
    size_t distance_to_substring = (size_t)(substr.data - source.data);
    size_t distance_to_line_start = 0;
    for (size_t i = 0; i < distance_to_substring;) {
        if (is_newline(source.data[i])) {
            result.start_line++;
            result.start_column = 0;
            if (source.data[i] == '\r') {
                i++;
            }
            if (source.data[i] == '\n') {
                i++;
            }
            distance_to_line_start = i;
        } else {
            result.start_column++;
            i++;
        }
    }

    result.end_line   = result.start_line;
    result.end_column = result.start_column;

    result.containing_lines = substring(&source, distance_to_line_start, distance_to_line_start);
    size_t source_remaining = source.len - distance_to_line_start;
    while (source_remaining > 0) {
        if (is_newline(result.containing_lines.data[result.containing_lines.len - 1])) {
            // TODO: Fix the searching for the end of the last _containing_
            // line of the substring. Right now, it cuts off the moment it hits
            // a newline, even if the substring hasn't been fully matched yet.
#if 0
            while (is_newline(result.containing_lines.data[result.containing_lines.len - 1])) {
                result.containing_lines.len++;
                source_remaining--;
            }

            result.end_line++;
            result.end_column = 0;
#else
            break;
#endif
        } else {
            result.end_column++;
            result.containing_lines.len++;
            source_remaining--;
        }
    }

    assert(result.containing_lines.len >= substr.len);

    return result;
}

enum DiagnosticSeverity {
    Diagnostic_Error,
    Diagnostic_Warning,
    Diagnostic_Note,
};

#define PRINTF_STRING(str) (int)str.len, str.data
internal void va_print_diagnostic(
    String source_file_name,
    String source_code,
    String relevant_code,
    DiagnosticSeverity severity,
    char* format_string,
    va_list format_args
) {
    FoundString line_info = find_substring(source_code, relevant_code);

    b32 bold_white = (severity != Diagnostic_Note);
    if (relevant_code.len > 1) {
        fprintf(stderr, "[%.*s:%d:%d:%d] ", PRINTF_STRING(source_file_name), line_info.start_line, line_info.start_column, line_info.end_column);
    } else {
        fprintf(stderr, "[%.*s:%d:%d] ", PRINTF_STRING(source_file_name), line_info.start_line, line_info.start_column);
    }

    switch (severity) {
        case Diagnostic_Error: {
            fprintf(stderr, "error: ");
        } break;

        case Diagnostic_Warning: {
            fprintf(stderr, "warning: ");
        } break;

        case Diagnostic_Note: {
            fprintf(stderr, "note: ");
        } break;
    }

    vfprintf(stderr, format_string, format_args);

    fputs("\n", stderr);

    char* ptr = line_info.containing_lines.data;
    u32 column_offset = 0;
    while (is_space(*ptr)) {
        if (*ptr == ' ' || *ptr == '\t') {
            column_offset++;
        } else {
            INVALID_CODE_PATH;
        }
        ptr++;
    }

    fprintf(stderr, "    %.*s\n", (int)line_info.containing_lines.len - column_offset - 1, line_info.containing_lines.data + column_offset);
    fprintf(stderr, "    %*s", (int)line_info.start_column - column_offset, (char*)"");
    s32 underline_length = (s32)relevant_code.len; // (line_info.end_column - line_info.start_column);
    if (underline_length > 1) {
        for (s32 i = 0; i < underline_length; i++) {
            fprintf(stderr, "~");
        }
    } else {
        fprintf(stderr, "^");
    }
    fprintf(stderr, "\n");
}

inline void print_diagnostic(
    String source_file_name,
    String source_code,
    String relevant_code,
    DiagnosticSeverity severity,
    char* format_string,
    ...
) {
    va_list args;
    va_start(args, format_string);
    va_print_diagnostic(source_file_name, source_code, relevant_code, severity, format_string, args);
    va_end(args);
}

#define MEMORY_ARENA_ALLOCATOR(name) void* name(size_t size)
typedef MEMORY_ARENA_ALLOCATOR(MemoryArenaAllocator);

struct MemoryArena {
    size_t size;
    size_t used;
    u8* base_ptr;

    MemoryArenaAllocator* allocator;

    s32 temp_count;
};

struct TemporaryMemory {
    MemoryArena* arena;
    size_t used;
};

inline void initialize_arena(MemoryArena* arena, size_t size, void* base_ptr, MemoryArenaAllocator* allocator = 0) {
    arena->size = size;
    arena->base_ptr = (u8*)base_ptr;
    arena->used = 0;
    arena->allocator = allocator;
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

inline void check_arena(MemoryArena* arena) {
    assert(arena->temp_count == 0);
}

inline size_t get_alignment_offset(MemoryArena* arena, size_t align) {
    size_t result_pointer = (size_t)arena->base_ptr + arena->used;

    size_t align_offset = result_pointer & (align - 1);
    if (align_offset) {
        align_offset = align - align_offset;
    }

    return align_offset;
}

inline size_t get_arena_size_remaining(MemoryArena* arena, size_t align = 4) {
    size_t result = arena->size - (arena->used + get_alignment_offset(arena, align));
    return result;
}

// TODO: Optional "clear" parameter
#define push_struct(arena, type, ...) (type*)push_size_(arena, sizeof(type), ##__VA_ARGS__)
#define push_array(arena, count, type, ...) (type*)push_size_(arena, sizeof(type) * (count), ##__VA_ARGS__)
#define push_size(arena, size, ...) push_size_(arena, size, ##__VA_ARGS__)
inline void* push_size_(MemoryArena* arena, size_t size, size_t align = 4) {
    void* result = 0;

    size_t align_offset = get_alignment_offset(arena, align);
    size += align_offset;

    if ((arena->used + size) > arena->size) {
        if (arena->allocator) {
            void* new_memory_block = arena->allocator(arena->size);
            initialize_arena(arena, arena->size, new_memory_block, arena->allocator);
            return push_size_(arena, size, align);
        } else {
            assert(!"Out of memory.");
        }
    }

    result = (void*)(arena->base_ptr + arena->used + align_offset);
    arena->used += size;

    return result;
}

inline void* get_next_allocation_location(MemoryArena* arena, size_t align = 4) {
    size_t align_offset = get_alignment_offset(arena, align);
    void* result = (void*)(arena->base_ptr + arena->used + align_offset);
    return result;
}

inline char* push_string(MemoryArena* arena, char* source, bool include_null = true) {
    u32 size = (u32)include_null;
    for (char* at = source; *at; at++) {
        size++;
    }

    assert((arena->used + size) <= arena->size);

    char* source_ptr = source;
    char* dest = (char*)push_size_(arena, size, 1);
    for (u32 char_index = 0; char_index < size; char_index++) {
        if (include_null || source[char_index]) {
            *dest++ = *source_ptr++;
        }
    }

    return source;
}

inline void sub_arena(MemoryArena* result, MemoryArena* arena, size_t size, size_t align = 4) {
    result->size = size;
    result->base_ptr = (u8*)push_size_(arena, size, align);
    result->used = 0;
    result->temp_count = 0;
}

#define ZERO_STRUCT(instance) zero_size(sizeof(instance), &(instance))
inline void zero_size(size_t size, void* ptr) {
    // TODO: Check this for performance
    u8* byte = (u8*)ptr;
    while (size--) {
        *byte++ = 0;
    }
}

inline void copy(size_t size, void* source_init, void* dest_init) {
    u8* source = (u8*)source_init;
    u8* dest = (u8*)dest_init;
    while (size--) { *dest++= *source++; }
}

#define ALLOCATOR(name) void* name(size_t size, void* realloc_ptr, size_t realloc_size, void* user_data)
typedef ALLOCATOR(Allocator);

internal ALLOCATOR(default_allocator) {
    return realloc(realloc_ptr, size);
}

internal ALLOCATOR(arena_allocator) {
    MemoryArena* arena = (MemoryArena*)user_data;
    void* result = push_size(arena, size, 1);
    if (realloc_ptr) {
        memcpy(result, realloc_ptr, realloc_size);
    }
    return result;
}

struct ArrayHeader {
    Allocator* allocator;
    void* allocator_data;
    size_t capacity;
    size_t count;
};

#define Array(TYPE) TYPE*
#define allocate_array(TYPE, ...) (TYPE*)allocate_array_(sizeof(TYPE), __VA_ARGS__)
inline void* allocate_array_(size_t type_size, size_t capacity = 8, Allocator* allocator = default_allocator, void* allocator_data = 0) {
    ArrayHeader* array = (ArrayHeader*)allocator(sizeof(ArrayHeader) + type_size*capacity, 0, 0, allocator_data);
    assert(array);

    array->allocator = allocator;
    array->allocator_data = allocator_data;
    array->capacity = capacity;
    array->count = 0;

    void* array_data = (void*)((u8*)array + sizeof(ArrayHeader));
    return array_data;
}

inline ArrayHeader* get_array_header(void* array) {
    ArrayHeader* header = (ArrayHeader*)((u8*)array - sizeof(ArrayHeader));
    return header;
}

inline void set_array_allocator(void* array, Allocator* allocator, void* allocator_data = 0) {
    ArrayHeader* header = get_array_header(array);
    header->allocator = allocator;
    header->allocator_data = allocator_data;
}

inline size_t array_count(void* array) {
    return get_array_header(array)->count;
}

inline size_t array_capacity(void* array) {
    return get_array_header(array)->capacity;
}

#define ARRAY_GROWTH_RATE 2
inline void* array_add_(void** array, size_t item_size, void* item) {
    ArrayHeader* header = get_array_header(*array);
    if ((header->count + 1) > header->capacity) {
        size_t old_size = sizeof(ArrayHeader) + item_size*header->capacity;
        size_t new_size = sizeof(ArrayHeader) + item_size*header->capacity * ARRAY_GROWTH_RATE;
        header->capacity *= ARRAY_GROWTH_RATE;

        void* new_address = header->allocator(new_size, header, old_size, header->allocator_data);
        assert(new_address);

        *array = (u8*)new_address + sizeof(ArrayHeader);
        header = get_array_header(*array);
    }
    void* result = (u8*)(*array) + item_size*header->count++;
    if (item) {
        memcpy(result, item, item_size);
    }
    return result;
}

#define array_add(ARRAY_PTR, ITEM) array_add_((void**)(ARRAY_PTR), sizeof(**ARRAY_PTR), &(ITEM))

#define array_get(ARRAY, INDEX) (ARRAY)[INDEX]
#define array_get_ptr(ARRAY, INDEX) &array_get(ARRAY, INDEX)

struct EntireFile {
    u32 contents_size;
    void* contents;
};

internal EntireFile read_entire_file_and_null_terminate(char* file_name) {
    EntireFile result = {};

    FILE* in = fopen(file_name, "rb");
    if (in) {
        fseek(in, 0, SEEK_END);
        result.contents_size = ftell(in);
        fseek(in, 0, SEEK_SET);

        result.contents = malloc(result.contents_size + 1);
        fread(result.contents, result.contents_size, 1, in);
        ((u8*)result.contents)[result.contents_size] = 0;
        fclose(in);
    }

    return result;
}

enum TokenType {
    Token_Unknown,

    // Token_OpenParen,
    // Token_CloseParen,
    // Token_OpenBracket,
    // Token_CloseBracket,
    // Token_OpenBrace,
    // Token_CloseBrace,
    // Token_Colon,
    Token_Semicolon,
    // Token_Asterisk,
    // Token_Comma,
    // Token_Pound,
    // Token_Equals,
    Token_Operator,
    Token_DotDotDot,

    Token_String,
    Token_Identifier,

    Token_EndOfStream,
};

#define PRINTF_TOKEN(token) (int)(token).length, (token).text
struct Token {
    TokenType type;
    size_t length;
    char* text;
};

struct Tokenizer {
    String source_file;
    String source_code;
    char* at;

    Token last_token;
    b32 had_error;
};

inline void error_at_token(Tokenizer* tokenizer, Token token, char* format_string, ...) {
    va_list args;
    va_start(args, format_string);
    va_print_diagnostic(tokenizer->source_file, tokenizer->source_code, wrap_string(token.length, token.text), Diagnostic_Error, format_string, args);
    va_end(args);
    tokenizer->had_error = true;
}

inline void error(Tokenizer* tokenizer, char* format_string, ...) {
    va_list args;
    va_start(args, format_string);
    Token token = tokenizer->last_token;
    va_print_diagnostic(tokenizer->source_file, tokenizer->source_code, wrap_string(token.length, token.text), Diagnostic_Error, format_string, args);
    va_end(args);
    tokenizer->had_error = true;
}

internal void eat_whitespaces(Tokenizer* tokenizer) {
    b32 eating = true;
    while (eating && tokenizer->at[0]) {
        switch (tokenizer->at[0]) {
            case '\r':
            case '\n':
            case '\t':
            case '\v':
            case '\f':
            case ' ': {
                tokenizer->at++;
            } break;

            case '/': {
                if (tokenizer->at[1] == '/') {
                    tokenizer->at += 2;
                    while (tokenizer->at[0] && !is_newline(tokenizer->at[0])) {
                        tokenizer->at++;
                    }
                } else if (tokenizer->at[1] == '*') {
                    tokenizer->at += 2;
                    while (tokenizer->at[0] && !(tokenizer->at[0] == '*' && tokenizer->at[1] == '/')) {
                        tokenizer->at++;
                    }
                    if (tokenizer->at[0] == '*') {
                        tokenizer->at += 2;
                    }
                } else {
                    eating = false;
                }
            } break;

            default: {
                eating = false;
            } break;
        }
    }
}

internal Token get_token(Tokenizer* tokenizer) {
    eat_whitespaces(tokenizer);

    Token token = {};
    token.length = 1;
    token.text = tokenizer->at;

    char c = tokenizer->at[0];
    tokenizer->at++;

    switch (c) {
        case '\0': { token.type = Token_EndOfStream; } break;

        // case '(': { token.type = Token_OpenParen; } break;
        // case ')': { token.type = Token_CloseParen; } break;
        // case '[': { token.type = Token_OpenBracket; } break;
        // case ']': { token.type = Token_CloseBracket; } break;
        // case '{': { token.type = Token_OpenBrace; } break;
        // case '}': { token.type = Token_CloseBrace; } break;
        // case ':': { token.type = Token_Colon; } break;
        case ';': { token.type = Token_Semicolon; } break;
        // case '*': { token.type = Token_Asterisk; } break;
        // case ',': { token.type = Token_Comma; } break;
        // case '#': { token.type = Token_Pound; } break;
        // case '=': { token.type = Token_Equals; } break;

        case '.': {
            if (tokenizer->at[0] == '.' && tokenizer->at[1] == '.') {
                token.type = Token_DotDotDot;
                token.length = 3;
                tokenizer->at += 2;
            }
        } break;

        case '"': {
            token.type = Token_String;

            token.text = tokenizer->at;
            while (tokenizer->at[0] && tokenizer->at[0] != '"') {
                if ((tokenizer->at[0] == '\\' && tokenizer->at[1])) {
                    tokenizer->at++;
                }
                tokenizer->at++;
            }

            token.length = tokenizer->at - token.text;

            if (tokenizer->at[0] == '"') {
                tokenizer->at++;
            }
        } break;

        default: {
            if (is_alphabetic(c)) {
                token.type = Token_Identifier;

                token.text = tokenizer->at - 1;
                while (tokenizer->at[0] && (is_alphanumeric(tokenizer->at[0]) || tokenizer->at[0] == '_')) {
                    tokenizer->at++;
                }

                token.length = tokenizer->at - token.text;
            } else if (is_numeric(c)) {
                // parse_number(tokenizer);
                token.type = Token_Unknown;
            } else {
                // NOTE: If it's not alphabetic, and we don't know what it is,
                // then I guess it's a single character operator!
                // (Multi-character operators ought to be handled on a
                // case-by-case basis up above. Not sure if it's a good idea,
                // but I feel like it's an effective simplification.
                token.type = Token_Operator;
            }
        } break;
    }

    tokenizer->last_token = token;

    return token;
}

inline b32 token_equals(Token token, char match) {
    b32 result = (token.length == 1) && (token.text[0] == match);
    return result;
}

inline b32 token_equals(Token token, char* match) {
    char* at = match;
    for (size_t i = 0; i < token.length; i++, at++) {
        if (*at == 0 || token.text[i] != *at) {
            return false;
        }
    }

    b32 result = (*at == 0);
    return result;
}

inline b32 token_equals(Token a, Token b) {
    b32 result = false;

    if (a.length == b.length) {
        result = true;
        for (u32 i = 0; i < a.length; i++) {
            if (*a.text++ != *b.text++) {
                result = false;
                break;
            }
        }
    }

    return result;
}

internal b32 match(Tokenizer* tokenizer, char desired_character) {
    Token token = get_token(tokenizer);
    b32 result = token_equals(token, desired_character);
    return result;
}

internal b32 match(Tokenizer* tokenizer, char* desired_string) {
    Token token = get_token(tokenizer);
    b32 result = token_equals(token, desired_string);
    return result;
}

internal b32 match(Tokenizer* tokenizer, TokenType desired_type) {
    Token token = get_token(tokenizer);
    b32 result = token.type == desired_type;
    return result;
}

struct IntrospectionParams {
    b32 should_parse;
    b32 is_flags;
};

internal IntrospectionParams parse_introspection_params(Tokenizer* tokenizer) {
    IntrospectionParams result = {};
    result.should_parse = true;

    for (;;) {
        Token token = get_token(tokenizer);
        if (token_equals(token, ')') || token.type == Token_EndOfStream) {
            break;
        }
        if (token_equals(token, "flags")) {
            if (match(tokenizer, ':')) {
                Token value = get_token(tokenizer);
                if (token_equals(value, "true")) {
                    result.is_flags = true;
                } else if (token_equals(value, "false")) {
                    result.is_flags = false;
                } else {
                    error(tokenizer, "Unexpected value for 'flags'");
                }
            } else {
                error(tokenizer, "Expected ':' after 'flags'");
            }
        }
        if (token_equals(token, "...")) {
            result.should_parse = false;
            break;
        }
    }
    return result;
}

struct MetaType {
    Token type;
};

struct StructMember {
    b32 is_pointer;
    Token type;
    Token name;
};

struct MetaStruct {
    Token name;
    Array(StructMember) members;
};

struct MetaEnum {
    Token name;
    Array(Token) members;
    b32 is_flags;
};

global Array(MetaType) meta_type_array;
global Array(MetaStruct) meta_struct_array;
global Array(MetaEnum) meta_enum_array;

inline void add_meta_type_if_unique(Array(MetaType)* type_array, Token type_token) {
    b32 already_exists = false;
    for (size_t i = 0; i < array_count(*type_array); i++) {
        MetaType test = array_get(*type_array, i);
        if (token_equals(test.type, type_token)) {
            already_exists = true;
            break;
        }
    }

    if (!already_exists) {
        MetaType meta_type;
        meta_type.type = type_token;
        array_add(type_array, meta_type);
    }
}

internal StructMember parse_member(Tokenizer* tokenizer, Token struct_type_token, Token member_type_token) {
    StructMember result;
    result.is_pointer = false;
    result.type = member_type_token;

    add_meta_type_if_unique(&meta_type_array, result.type);

    b32 parsing = true;
    while (parsing) {
        Token token = get_token(tokenizer);

        switch (token.type) {
            case Token_Operator: {
                if (token_equals(token, '*')) {
                    result.is_pointer = true;
                }
            } break;

            case Token_Identifier: {
                result.name = token;
            } break;

            case Token_Semicolon: {
                parsing = false;
            } break;

            case Token_EndOfStream: {
                error(tokenizer, "Unexpected end of file.");
                parsing = false;
            } break;
        }
    }
    return result;
}

internal void parse_struct(Tokenizer* tokenizer, IntrospectionParams params) {
    Token name_token = get_token(tokenizer);

    MetaStruct meta;
    meta.name = name_token;
    meta.members = allocate_array(StructMember);

    if (token_equals(name_token, "Entity")) {
        int y = 5;
    }

    add_meta_type_if_unique(&meta_type_array, meta.name);

    if (match(tokenizer, '{')) {
        for (;;) {
            Token member_token = get_token(tokenizer);
            if (member_token.type == Token_EndOfStream || token_equals(member_token, '}')) {
                break;
            } else {
                StructMember member = parse_member(tokenizer, name_token, member_token);
                array_add(&meta.members, member);
            }
        }
    } else {
        error(tokenizer, "Expected '{' after struct declaration.");
    }

    array_add(&meta_struct_array, meta);
}

internal void parse_enum(Tokenizer* tokenizer, IntrospectionParams params) {
    Token name_token = get_token(tokenizer);

    MetaEnum meta;
    meta.name = name_token;
    meta.members = allocate_array(Token);
    meta.is_flags = params.is_flags;

    add_meta_type_if_unique(&meta_type_array, meta.name);

    if (match(tokenizer, '{')) {
        for (;;) {
            Token member_token = get_token(tokenizer);
            if (member_token.type == Token_EndOfStream || token_equals(member_token, '}')) {
                break;
            } else if (token_equals(member_token, '=')) {
                while (!match(tokenizer, ',')) {}
            } else if (token_equals(member_token, ',')) {
                continue;
            } else {
                array_add(&meta.members, member_token);
            }
        }
    } else {
        error(tokenizer, "Expected '{' after enum declaration.");
    }

    array_add(&meta_enum_array, meta);
};

internal void parse_introspectable(Tokenizer* tokenizer) {
    if (match(tokenizer, '(')) {
        IntrospectionParams params = parse_introspection_params(tokenizer);

        if (params.should_parse) {
            Token type_token = get_token(tokenizer);
            if (token_equals(type_token, "struct")) {
                parse_struct(tokenizer, params);
            } else if (token_equals(type_token, "enum")) {
                parse_enum(tokenizer, params);
            } else {
                error(tokenizer, "Introspection is only supported for structs and enums right now :(");
            }
        }
    }
}

internal MEMORY_ARENA_ALLOCATOR(malloc_allocator) {
    return malloc(size);
}

global s64 perf_count_frequency;
inline LARGE_INTEGER win32_get_clock() {
    LARGE_INTEGER result;
    QueryPerformanceCounter(&result);
    return result;
}

inline f32 win32_get_seconds_elapsed(LARGE_INTEGER start, LARGE_INTEGER end) {
    f32 result = cast(f32) (end.QuadPart - start.QuadPart) / cast(f32) perf_count_frequency;
    return result;
}

inline void win32_initialize_perf_counter() {
    LARGE_INTEGER perf_count_frequency_result;
    QueryPerformanceFrequency(&perf_count_frequency_result);
    perf_count_frequency = perf_count_frequency_result.QuadPart;
}

int main(int argument_count, char** arguments) {
    win32_initialize_perf_counter();
    LARGE_INTEGER start_clock = win32_get_clock();

    MemoryArena general_arena;
    initialize_arena(&general_arena, MEGABYTES(128), malloc(MEGABYTES(128)), malloc_allocator);

    meta_type_array = allocate_array(MetaType, 8, arena_allocator, &general_arena);
    meta_enum_array = allocate_array(MetaEnum, 8, arena_allocator, &general_arena);
    meta_struct_array = allocate_array(MetaStruct, 8, arena_allocator, &general_arena);

    WIN32_FIND_DATAA find_data;
    HANDLE find_handle = FindFirstFileA("*.h", &find_data);

    if (find_handle == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "No .h files found...\n");
        return -1;
    }

    char* file_name = find_data.cFileName;

    u32 file_count = 1;
    b32 had_error = false;
    while (file_name && !had_error) {
        if (!strings_are_equal(file_name, "pulsar_code_generator.h")) {
            EntireFile file = read_entire_file_and_null_terminate(file_name);
            if (file.contents_size > 0) {
                Tokenizer tokenizer = {};
                tokenizer.source_file = wrap_cstr(file_name);
                tokenizer.source_code = wrap_string(file.contents_size, (char*)file.contents);
                tokenizer.at = (char*)file.contents;

                b32 parsing = true;
                while (parsing) {
                    Token token = get_token(&tokenizer);
                    switch (token.type) {
                        case Token_Unknown: {
                        } break;

                        case Token_Identifier: {
                            if (token_equals(token, "introspect")) {
                                parse_introspectable(&tokenizer);
                            }
                        } break;

                        case Token_Operator: {
                            if (token_equals(token, '#')) {
                                // TODO: Handle multiple levels of #if 0
                                if (match(&tokenizer, "if") && match(&tokenizer, '0')) {
                                    Token next_token = get_token(&tokenizer);
                                    while (next_token.type != Token_EndOfStream) {
                                        if (token_equals(next_token, '#') && match(&tokenizer, "endif")) {
                                            break;
                                        }
                                        next_token = get_token(&tokenizer);
                                    }
                                    if (next_token.type == Token_EndOfStream) {
                                        error(&tokenizer, "Unmatched #if 0");
                                    }
                                }
                            }
                        } break;

                        case Token_EndOfStream: {
                            parsing = false;
                        } break;

                        default: {
                        } break;
                    }
                    if (tokenizer.had_error) {
                        had_error = true;
                    }
                }
            } else {
                fprintf(stderr, "ERROR: Failed to open file '%s'", file_name);
            }
        }

        if (FindNextFileA(find_handle, &find_data)) {
            file_count++;
            file_name = find_data.cFileName;
        } else {
            file_name = 0;
        }
    }

    FindClose(find_handle);

    FILE* pre_headers = fopen("pulsar_generated_pre_headers.h", "w");

    fprintf(pre_headers, "#ifndef PULSAR_GENERATED_PRE_HEADERS_H\n");
    fprintf(pre_headers, "#define PULSAR_GENERATED_PRE_HEADERS_H\n\n");

    fprintf(pre_headers, "#define PULSAR_CODE_GENERATION_SUCCEEDED 1\n\n");

    fprintf(pre_headers, "enum MetaType {\n");
    for (size_t type_index = 0; type_index < array_count(meta_type_array); type_index++) {
        MetaType type = array_get(meta_type_array, type_index);
        fprintf(pre_headers, "    MetaType_%.*s,\n", PRINTF_TOKEN(type.type));
    }
    fprintf(pre_headers, "};\n\n");

    for (size_t struct_index = 0; struct_index < array_count(meta_struct_array); struct_index++) {
        MetaStruct meta = array_get(meta_struct_array, struct_index);
        fprintf(pre_headers, "#define BodyOf_%.*s \\\n", PRINTF_TOKEN(meta.name));
        for (size_t member_index = 0; member_index < array_count(meta.members); member_index++) {
            auto member = array_get(meta.members, member_index);
            fprintf(pre_headers, "    %.*s%s %.*s;%s\n",
                PRINTF_TOKEN(member.type),
                member.is_pointer ? "*" : "",
                PRINTF_TOKEN(member.name),
                (member_index + 1 < array_count(meta.members)) ? " \\" : ""
            );
        }
        fprintf(pre_headers, "\n");
    }

    fprintf(pre_headers, "#endif\n");

    FILE* post_headers = fopen("pulsar_generated_post_headers.h", "w");

    fprintf(post_headers, "#ifndef PULSAR_GENERATED_POST_HEADERS_H\n");
    fprintf(post_headers, "#define PULSAR_GENERATED_POST_HEADERS_H\n\n");

    for (size_t enum_index = 0; enum_index < array_count(meta_enum_array); enum_index++) {
        MetaEnum meta = array_get(meta_enum_array, enum_index);
        if (meta.is_flags) {
            fprintf(post_headers, "int GetNextEnumFlagNameOf_%.*s(unsigned int* value, char** name) {\n", PRINTF_TOKEN(meta.name));
            for (size_t member_index = 0; member_index < array_count(meta.members); member_index++) {
                Token member = array_get(meta.members, member_index);
                fprintf(post_headers, "    %s (*value & %.*s) { *name = \"%.*s\"; *value &= ~%.*s; return 1; }\n",
                    member_index == 0 ? "if" : "else if",
                    PRINTF_TOKEN(member), PRINTF_TOKEN(member), PRINTF_TOKEN(member)
                );
            }
            fprintf(post_headers, "    else { *name = 0; return 0; }\n}\n\n");
        } else {
            fprintf(post_headers, "char* GetEnumNameOf_%.*s(int value) {\n", PRINTF_TOKEN(meta.name));
            fprintf(post_headers, "    switch (value) {\n");
            for (size_t member_index = 0; member_index < array_count(meta.members); member_index++) {
                Token member = array_get(meta.members, member_index);
                fprintf(post_headers, "        case %.*s: return \"%.*s\";\n", PRINTF_TOKEN(member), PRINTF_TOKEN(member));
            }
            fprintf(post_headers, "        default: return \"Unknown value for %.*s\";\n", PRINTF_TOKEN(meta.name));
            fprintf(post_headers, "    }\n");
            fprintf(post_headers, "}\n\n");
        }
    }

    for (size_t struct_index = 0; struct_index < array_count(meta_struct_array); struct_index++) {
        MetaStruct meta = array_get(meta_struct_array, struct_index);
        fprintf(post_headers, "static MemberDefinition MembersOf_%.*s[] = {\n", PRINTF_TOKEN(meta.name));
        for (size_t member_index = 0; member_index < array_count(meta.members); member_index++) {
            auto member = array_get(meta.members, member_index);
            fprintf(post_headers, "    { %s, MetaType_%.*s, \"%.*s\", (u32)&((%.*s*)0)->%.*s },\n",
                member.is_pointer ? "MetaMemberFlag_IsPointer" : "0",
                PRINTF_TOKEN(member.type),
                PRINTF_TOKEN(member.name),
                PRINTF_TOKEN(meta.name),
                PRINTF_TOKEN(member.name)
            );
        }
        fprintf(post_headers, "};\n\n");
    }

    fprintf(post_headers, "#endif\n");

    LARGE_INTEGER end_clock = win32_get_clock();
    f32 seconds_elapsed = win32_get_seconds_elapsed(start_clock, end_clock);

    fprintf(stderr, "Ran code generator in %g seconds\n", seconds_elapsed);
}
