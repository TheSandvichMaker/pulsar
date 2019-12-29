# 1 "pulsar_main.cpp"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 375 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "pulsar_main.cpp" 2
# 1 "./pulsar_main.h" 1
# 17 "./pulsar_main.h"
# 1 "./common.h" 1



# 1 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\stdint.h" 1 3
# 63 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\stdint.h" 3
# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\stdint.h" 1 3
# 13 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\stdint.h" 3
# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vcruntime.h" 1 3
# 45 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vcruntime.h" 3
# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\sal.h" 1 3
# 2361 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\sal.h" 3
extern "C" {
# 2967 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\sal.h" 3
}



# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\ConcurrencySal.h" 1 3
# 22 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\ConcurrencySal.h" 3
extern "C" {
# 354 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\ConcurrencySal.h" 3
}
# 2971 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\sal.h" 2 3
# 46 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vcruntime.h" 2 3
# 1 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\vadefs.h" 1 3
# 32 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\vadefs.h" 3
# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vadefs.h" 1 3
# 15 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vadefs.h" 3
#pragma pack(push, 8)


extern "C" {
# 28 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vadefs.h" 3
        typedef unsigned __int64 uintptr_t;
# 39 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vadefs.h" 3
        typedef char* va_list;
# 106 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vadefs.h" 3
    void __cdecl __va_start(va_list* , ...);
# 118 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vadefs.h" 3
}



    extern "C++"
    {
        template <typename _Ty>
        struct __vcrt_va_list_is_reference
        {
            enum : bool { __the_value = false };
        };

        template <typename _Ty>
        struct __vcrt_va_list_is_reference<_Ty&>
        {
            enum : bool { __the_value = true };
        };

        template <typename _Ty>
        struct __vcrt_va_list_is_reference<_Ty&&>
        {
            enum : bool { __the_value = true };
        };

        template <typename _Ty>
        void __vcrt_va_start_verify_argument_type() throw()
        {
            static_assert(!__vcrt_va_list_is_reference<_Ty>::__the_value, "va_start argument must not have reference type and must not be parenthesized");
        }
    }
# 157 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vadefs.h" 3
#pragma pack(pop)
# 33 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\vadefs.h" 2 3
# 47 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vcruntime.h" 2 3
# 81 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vcruntime.h" 3
#pragma pack(push, 8)
# 81 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vcruntime.h" 3
 extern "C" {
# 194 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vcruntime.h" 3
    typedef unsigned __int64 size_t;
    typedef __int64 ptrdiff_t;
    typedef __int64 intptr_t;







    typedef bool __vcrt_bool;
# 247 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vcruntime.h" 3
    extern "C++"
    {
        template <typename _CountofType, size_t _SizeOfArray>
        char (*__countof_helper(__unaligned _CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];


    }
# 298 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vcruntime.h" 3
    void __cdecl __security_init_cookie(void);





        void __cdecl __security_check_cookie( uintptr_t _StackCookie);
        __declspec(noreturn) void __cdecl __report_gsfailure( uintptr_t _StackCookie);



extern uintptr_t __security_cookie;







}
# 317 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vcruntime.h" 3
#pragma pack(pop)
# 14 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\stdint.h" 2 3



typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int_least8_t;
typedef short int_least16_t;
typedef int int_least32_t;
typedef long long int_least64_t;
typedef unsigned char uint_least8_t;
typedef unsigned short uint_least16_t;
typedef unsigned int uint_least32_t;
typedef unsigned long long uint_least64_t;

typedef signed char int_fast8_t;
typedef int int_fast16_t;
typedef int int_fast32_t;
typedef long long int_fast64_t;
typedef unsigned char uint_fast8_t;
typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;
typedef unsigned long long uint_fast64_t;

typedef long long intmax_t;
typedef unsigned long long uintmax_t;
# 64 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\stdint.h" 2 3
# 5 "./common.h" 2
# 1 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\stddef.h" 1 3
# 51 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\stddef.h" 3
typedef long long int ptrdiff_t;
# 62 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\stddef.h" 3
typedef long long unsigned int size_t;
# 109 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\stddef.h" 3
namespace std { typedef decltype(nullptr) nullptr_t; }
using ::std::nullptr_t;








# 1 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include/__stddef_max_align_t.h" 1 3
# 30 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include/__stddef_max_align_t.h" 3
typedef double max_align_t;
# 119 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\stddef.h" 2 3
# 6 "./common.h" 2
# 1 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\float.h" 1 3
# 45 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\float.h" 3
# 1 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\float.h" 1 3
# 13 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\float.h" 3
# 1 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt.h" 1 3
# 10 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt.h" 3
# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vcruntime.h" 1 3
# 11 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt.h" 2 3

#pragma pack(push, 8)
# 12 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt.h" 3
 extern "C" {
# 136 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt.h" 3
extern "C++"
{
    template<bool _Enable, typename _Ty>
    struct _CrtEnableIf;

    template<typename _Ty>
    struct _CrtEnableIf<true, _Ty>
    {
        typedef _Ty _Type;
    };
}



    typedef bool __crt_bool;
# 278 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt.h" 3
             void __cdecl _invalid_parameter_noinfo(void);
         __declspec(noreturn) void __cdecl _invalid_parameter_noinfo_noreturn(void);

__declspec(noreturn)
         void __cdecl _invoke_watson(
               wchar_t const* _Expression,
               wchar_t const* _FunctionName,
               wchar_t const* _FileName,
               unsigned int _LineNo,
               uintptr_t _Reserved);
# 510 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt.h" 3
typedef int errno_t;
typedef unsigned short wint_t;
typedef unsigned short wctype_t;
typedef long __time32_t;
typedef __int64 __time64_t;

typedef struct __crt_locale_data_public
{
      unsigned short const* _locale_pctype;
                        int _locale_mb_cur_max;
               unsigned int _locale_lc_codepage;
} __crt_locale_data_public;

typedef struct __crt_locale_pointers
{
    struct __crt_locale_data* locinfo;
    struct __crt_multibyte_data* mbcinfo;
} __crt_locale_pointers;

typedef __crt_locale_pointers* _locale_t;

typedef struct _Mbstatet
{
    unsigned long _Wchar;
    unsigned short _Byte, _State;
} _Mbstatet;

typedef _Mbstatet mbstate_t;
# 551 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt.h" 3
        typedef __time64_t time_t;
# 561 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt.h" 3
    typedef size_t rsize_t;
# 2010 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt.h" 3
}
# 2010 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt.h" 3
#pragma pack(pop)
# 14 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\float.h" 2 3

#pragma pack(push, 8)
# 15 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\float.h" 3
 extern "C" {
# 227 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\float.h" 3
         unsigned int __cdecl _clearfp(void);

#pragma warning(push)
#pragma warning(disable: 4141)

 __declspec(deprecated("This function or variable may be unsafe. Consider using " "_controlfp_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
         unsigned int __cdecl _controlfp(
         unsigned int _NewValue,
         unsigned int _Mask
    );

#pragma warning(pop)


 void __cdecl _set_controlfp(
         unsigned int _NewValue,
         unsigned int _Mask
    );


         errno_t __cdecl _controlfp_s(
              unsigned int* _CurrentState,
              unsigned int _NewValue,
              unsigned int _Mask
    );


         unsigned int __cdecl _statusfp(void);


         void __cdecl _fpreset(void);
# 273 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\float.h" 3
         unsigned int __cdecl _control87(
         unsigned int _NewValue,
         unsigned int _Mask
    );
# 290 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\float.h" 3
         int* __cdecl __fpecode(void);




         int __cdecl __fpe_flt_rounds(void);
# 308 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\float.h" 3
                        double __cdecl _copysign( double _Number, double _Sign);
                        double __cdecl _chgsign( double _X);
                        double __cdecl _scalb( double _X, long _Y);
                        double __cdecl _logb( double _X);
                        double __cdecl _nextafter( double _X, double _Y);
                        int __cdecl _finite( double _X);
                        int __cdecl _isnan( double _X);
                        int __cdecl _fpclass( double _X);


                            float __cdecl _scalbf( float _X, long _Y);
# 335 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\float.h" 3
             void __cdecl fpreset(void);
# 402 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\float.h" 3
}
# 402 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\float.h" 3
#pragma pack(pop)
# 46 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\float.h" 2 3
# 7 "./common.h" 2
# 1 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\assert.h" 1 3
# 14 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\assert.h" 3
#pragma pack(push, 8)
# 14 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\assert.h" 3
 extern "C" {
# 26 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\assert.h" 3
             void __cdecl _wassert(
               wchar_t const* _Message,
               wchar_t const* _File,
               unsigned _Line
        );
# 41 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\assert.h" 3
}
# 41 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\assert.h" 3
#pragma pack(pop)
# 8 "./common.h" 2







typedef int32_t b32;

typedef float f32;
typedef double f64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
# 59 "./common.h"
inline u32 safe_truncate_u64u32(u64 source) {
    (void)( (!!(source <= 0xffffffffui32)) || (_wassert(L"source <= UINT32_MAX", L"./common.h", (unsigned)(60)), 0) );
    u32 result = (u32) source;
    return result;
}
# 18 "./pulsar_main.h" 2

# 1 "./memory_arena.h" 1



struct LinearBuffer;
struct MemoryArena {
    size_t size;
    size_t used;
    u8* base_ptr;

    s32 temp_count;
    LinearBuffer* active_linear_buffer;
};

struct TemporaryMemory {
    MemoryArena* arena;
    size_t used;
};

inline void initialize_arena(MemoryArena* arena, size_t size, void* base_ptr) {
    arena->size = size;
    arena->base_ptr = (u8*)base_ptr;
    arena->used = 0;
    arena->temp_count = 0;
    arena->active_linear_buffer = 0;
}

enum ArenaPushFlags {
    ArenaFlag_ClearToZero = 0x1,
};

struct ArenaPushParams {
    u32 flags;
    u32 alignment;
};

inline ArenaPushParams default_arena_params() {
    ArenaPushParams params;
    params.flags = ArenaFlag_ClearToZero;
    params.alignment = 4;
    return params;
}

inline ArenaPushParams align_flags(u32 align, u32 flags) {
    ArenaPushParams params = default_arena_params();
    params.flags = flags;
    params.alignment = align;
    return params;
}

inline ArenaPushParams align(u32 align, b32 clear) {
    ArenaPushParams params = default_arena_params();
    params.flags = clear ? (params.flags | ArenaFlag_ClearToZero) : (params.flags & ~ArenaFlag_ClearToZero);
    params.alignment = align;
    return params;
}

inline ArenaPushParams no_clear() {
    ArenaPushParams params = default_arena_params();
    params.flags &= ~ArenaFlag_ClearToZero;
    return params;
}

inline ArenaPushParams align_no_clear(u32 align) {
    ArenaPushParams params = default_arena_params();
    params.flags &= ~ArenaFlag_ClearToZero;
    params.alignment = align;
    return params;
}

inline size_t get_alignment_offset(MemoryArena* arena, size_t align, size_t at) {
    size_t result_pointer = (size_t)arena->base_ptr + at;

    size_t align_offset = result_pointer & (align - 1);
    if (align_offset) {
        align_offset = align - align_offset;
    }

    return align_offset;
}

inline size_t get_alignment_offset(MemoryArena* arena, size_t align) {
    size_t align_offset = get_alignment_offset(arena, align, arena->used);
    return align_offset;
}

inline size_t get_arena_size_remaining(MemoryArena* arena, ArenaPushParams params = default_arena_params()) {
    size_t result = arena->size - (arena->used + get_alignment_offset(arena, params.alignment));
    return result;
}



inline void* zero_size(size_t size, void* ptr) {

    u8* byte = (u8*)ptr;
    while (size--) {
        *byte++ = 0;
    }
    return ptr;
}

inline size_t get_effective_size_for(MemoryArena* arena, size_t size, ArenaPushParams params) {
    size_t align_offset = get_alignment_offset(arena, params.alignment);
    size_t adjusted_size = size + align_offset;
    return adjusted_size;
}

inline b32 arena_has_room_for(MemoryArena* arena, size_t size, ArenaPushParams params = default_arena_params()) {
    size_t effective_size = get_effective_size_for(arena, size, params);
    b32 result = (arena->used + effective_size) <= arena->size;
    return result;
}

inline void* get_next_allocation_location(MemoryArena* arena, size_t align = 4) {
    size_t align_offset = get_alignment_offset(arena, align);
    void* result = (void*) (arena->base_ptr + arena->used + align_offset);
    return result;
}






inline void* push_size_(MemoryArena* arena, size_t size_init, ArenaPushParams params = default_arena_params(), LinearBuffer* lb = 0) {
    (void)( (!!(arena->active_linear_buffer == lb)) || (_wassert(L"arena->active_linear_buffer == lb", L"./memory_arena.h", (unsigned)(126)), 0) );

    size_t size = get_effective_size_for(arena, size_init, params);

    (void)( (!!((arena->used + size) <= arena->size)) || (_wassert(L"(arena->used + size) <= arena->size", L"./memory_arena.h", (unsigned)(130)), 0) );
    size_t align_offset = get_alignment_offset(arena, params.alignment);
    void* result = (arena->base_ptr + arena->used + align_offset);
    arena->used += size;

    if (params.flags & ArenaFlag_ClearToZero) {
        zero_size(size_init, result);
    }

    return result;
}

struct LinearBuffer {
    MemoryArena* arena;
    u32 flags;
    size_t count;
    void* data;
};


inline void* begin_linear_buffer_(MemoryArena* arena, ArenaPushParams params = default_arena_params()) {
    (void)( (!!(!arena->active_linear_buffer)) || (_wassert(L"!arena->active_linear_buffer", L"./memory_arena.h", (unsigned)(151)), 0) );

    LinearBuffer* header = (LinearBuffer*)push_size_(arena, sizeof(LinearBuffer), align(params.alignment, false));
    header->arena = arena;
    header->flags = params.flags;
    header->count = 0;
    header->data = header + 1;

    arena->active_linear_buffer = header;

    return header->data;
}







inline size_t end_linear_buffer(void* buffer) {
    LinearBuffer* header = ((LinearBuffer*) ((u8*) buffer - sizeof(LinearBuffer)));
    (void)( (!!(header->arena->active_linear_buffer == header)) || (_wassert(L"header->arena->active_linear_buffer == header", L"./memory_arena.h", (unsigned)(172)), 0) );

    header->arena->active_linear_buffer = 0;
    header->arena = 0;

    return header->count;
}



inline char* push_string(MemoryArena* arena, size_t length, char* source) {
    (void)( (!!((arena->used + length) <= arena->size)) || (_wassert(L"(arena->used + length) <= arena->size", L"./memory_arena.h", (unsigned)(183)), 0) );

    char* dest = (char*)push_size_(arena, length, align_no_clear(1));
    for (u32 char_index = 0; char_index < length; char_index++) {
        dest[char_index] = source[char_index];
    }

    return dest;
}

inline char* push_string_and_null_terminate(MemoryArena* arena, size_t length, char* source) {
    char* result = push_string(arena, length + 1, source);
    result[length] = '\0';

    return result;
}

inline void sub_arena(MemoryArena* result, MemoryArena* arena, size_t size, ArenaPushParams params = default_arena_params()) {
    result->size = size;
    result->base_ptr = (u8*)push_size_(arena, size, params);
    result->used = 0;
    result->temp_count = 0;
}

inline void copy(size_t size, void* source_init, void* dest_init) {
    u8* source = (u8*)source_init;
    u8* dest = (u8*)dest_init;
    while (size--) { *dest++= *source++; }
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
    (void)( (!!(arena->temp_count > 0)) || (_wassert(L"arena->temp_count > 0", L"./memory_arena.h", (unsigned)(227)), 0) );
    arena->temp_count--;
}

inline void clear_arena(MemoryArena* arena) {
    initialize_arena(arena, arena->size, arena->base_ptr);
}

inline void check_arena(MemoryArena* arena) {
    (void)( (!!(arena->temp_count == 0)) || (_wassert(L"arena->temp_count == 0", L"./memory_arena.h", (unsigned)(236)), 0) );
    (void)( (!!(arena->active_linear_buffer == 0)) || (_wassert(L"arena->active_linear_buffer == 0", L"./memory_arena.h", (unsigned)(237)), 0) );
}
# 20 "./pulsar_main.h" 2
# 1 "./pulsar_platform_bridge.h" 1



# 1 "./math.h" 1



# 1 "./intrinsics.h" 1



# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\cmath" 1 3




# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\yvals.h" 1 3






# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\xkeycheck.h" 1 3
# 8 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\yvals.h" 2 3
# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\crtdefs.h" 1 3








# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vcruntime.h" 1 3
# 10 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\crtdefs.h" 2 3
# 9 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\yvals.h" 2 3

#pragma pack(push,8)
# 253 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\yvals.h" 3
#pragma detect_mismatch("_MSC_VER", "1900")



#pragma detect_mismatch("_ITERATOR_DEBUG_LEVEL", "0")




#pragma detect_mismatch("RuntimeLibrary", "MT_StaticRelease")
# 412 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\yvals.h" 3
# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\use_ansi.h" 1 3
# 57 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\use_ansi.h" 3
#pragma comment(lib, "libcpmt" "" "")
# 413 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\yvals.h" 2 3
# 592 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\yvals.h" 3
typedef long long _Longlong;
typedef unsigned long long _ULonglong;
# 611 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\yvals.h" 3
namespace std {
enum _Uninitialized
 {
 _Noinit
 };



#pragma warning(push)
#pragma warning(disable: 4412)
class _Lockit
 {
public:
# 644 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\yvals.h" 3
 __thiscall _Lockit();
 explicit __thiscall _Lockit(int);
 __thiscall ~_Lockit() noexcept;


 static void __cdecl _Lockit_ctor(int);
 static void __cdecl _Lockit_dtor(int);

private:
 static void __cdecl _Lockit_ctor(_Lockit *);
 static void __cdecl _Lockit_ctor(_Lockit *, int);
 static void __cdecl _Lockit_dtor(_Lockit *);

public:
                    _Lockit(const _Lockit&) = delete;
 _Lockit& operator=(const _Lockit&) = delete;

private:
 int _Locktype;
# 677 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\yvals.h" 3
 };
# 764 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\yvals.h" 3
class _Init_locks
 {
public:
# 780 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\yvals.h" 3
 __thiscall _Init_locks();
 __thiscall ~_Init_locks() noexcept;


private:
 static void __cdecl _Init_locks_ctor(_Init_locks *);
 static void __cdecl _Init_locks_dtor(_Init_locks *);
# 797 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\yvals.h" 3
 };

#pragma warning(pop)
}
# 809 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\yvals.h" 3
         void __cdecl _Atexit(void (__cdecl *)(void));

typedef unsigned long _Uint32t;





#pragma pack(pop)
# 6 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\cmath" 2 3






# 1 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\math.h" 1 3
# 11 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\math.h" 3
# 1 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_math.h" 1 3
# 14 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_math.h" 3
#pragma pack(push, 8)
# 14 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_math.h" 3
 extern "C" {

#pragma warning(push)
#pragma warning(disable: 4738)
#pragma warning(disable: 4820)






 struct _exception
    {
        int type;
        char* name;
        double arg1;
        double arg2;
        double retval;
    };






        struct _complex
        {
            double x, y;
        };
# 61 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_math.h" 3
    typedef float float_t;
    typedef double double_t;
# 80 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_math.h" 3
        extern double const _HUGE;
# 171 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_math.h" 3
void __cdecl _fperrraise( int _Except);

                        short __cdecl _dclass( double _X);
                        short __cdecl _ldclass( long double _X);
                        short __cdecl _fdclass( float _X);

                        int __cdecl _dsign( double _X);
                        int __cdecl _ldsign( long double _X);
                        int __cdecl _fdsign( float _X);

                        int __cdecl _dpcomp( double _X, double _Y);
                        int __cdecl _ldpcomp( long double _X, long double _Y);
                        int __cdecl _fdpcomp( float _X, float _Y);

                        short __cdecl _dtest( double* _Px);
                        short __cdecl _ldtest( long double* _Px);
                        short __cdecl _fdtest( float* _Px);

         short __cdecl _d_int( double* _Px, short _Xexp);
         short __cdecl _ld_int( long double* _Px, short _Xexp);
         short __cdecl _fd_int( float* _Px, short _Xexp);

         short __cdecl _dscale( double* _Px, long _Lexp);
         short __cdecl _ldscale( long double* _Px, long _Lexp);
         short __cdecl _fdscale( float* _Px, long _Lexp);

         short __cdecl _dunscale( short* _Pex, double* _Px);
         short __cdecl _ldunscale( short* _Pex, long double* _Px);
         short __cdecl _fdunscale( short* _Pex, float* _Px);

                        short __cdecl _dexp( double* _Px, double _Y, long _Eoff);
                        short __cdecl _ldexp( long double* _Px, long double _Y, long _Eoff);
                        short __cdecl _fdexp( float* _Px, float _Y, long _Eoff);

                        short __cdecl _dnorm( unsigned short* _Ps);
                        short __cdecl _fdnorm( unsigned short* _Ps);

                        double __cdecl _dpoly( double _X, double const* _Tab, int _N);
                        long double __cdecl _ldpoly( long double _X, long double const* _Tab, int _N);
                        float __cdecl _fdpoly( float _X, float const* _Tab, int _N);

                        double __cdecl _dlog( double _X, int _Baseflag);
                        long double __cdecl _ldlog( long double _X, int _Baseflag);
                        float __cdecl _fdlog( float _X, int _Baseflag);

                        double __cdecl _dsin( double _X, unsigned int _Qoff);
                        long double __cdecl _ldsin( long double _X, unsigned int _Qoff);
                        float __cdecl _fdsin( float _X, unsigned int _Qoff);


typedef union
{
    unsigned short _Sh[4];
    double _Val;
} _double_val;


typedef union
{
    unsigned short _Sh[2];
    float _Val;
} _float_val;


typedef union
{
    unsigned short _Sh[4];
    long double _Val;
} _ldouble_val;

typedef union
{
    unsigned short _Word[4];
    float _Float;
    double _Double;
    long double _Long_double;
} _float_const;

extern const _float_const _Denorm_C, _Inf_C, _Nan_C, _Snan_C, _Hugeval_C;
extern const _float_const _FDenorm_C, _FInf_C, _FNan_C, _FSnan_C;
extern const _float_const _LDenorm_C, _LInf_C, _LNan_C, _LSnan_C;

extern const _float_const _Eps_C, _Rteps_C;
extern const _float_const _FEps_C, _FRteps_C;
extern const _float_const _LEps_C, _LRteps_C;

extern const double _Zero_C, _Xbig_C;
extern const float _FZero_C, _FXbig_C;
extern const long double _LZero_C, _LXbig_C;
# 288 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_math.h" 3
extern "C++"
{
                   inline int fpclassify( float _X) throw()
    {
        return _fdtest(&_X);
    }

                   inline int fpclassify( double _X) throw()
    {
        return _dtest(&_X);
    }

                   inline int fpclassify( long double _X) throw()
    {
        return _ldtest(&_X);
    }

                   inline bool signbit( float _X) throw()
    {
        return _fdsign(_X) != 0;
    }

                   inline bool signbit( double _X) throw()
    {
        return _dsign(_X) != 0;
    }

                   inline bool signbit( long double _X) throw()
    {
        return _ldsign(_X) != 0;
    }

                   inline int _fpcomp( float _X, float _Y) throw()
    {
        return _fdpcomp(_X, _Y);
    }

                   inline int _fpcomp( double _X, double _Y) throw()
    {
        return _dpcomp(_X, _Y);
    }

                   inline int _fpcomp( long double _X, long double _Y) throw()
    {
        return _ldpcomp(_X, _Y);
    }

    template <class _Trc, class _Tre> struct _Combined_type
    {
        typedef float _Type;
    };

    template <> struct _Combined_type<float, double>
    {
        typedef double _Type;
    };

    template <> struct _Combined_type<float, long double>
    {
        typedef long double _Type;
    };

    template <class _Ty, class _T2> struct _Real_widened
    {
        typedef long double _Type;
    };

    template <> struct _Real_widened<float, float>
    {
        typedef float _Type;
    };

    template <> struct _Real_widened<float, double>
    {
        typedef double _Type;
    };

    template <> struct _Real_widened<double, float>
    {
        typedef double _Type;
    };

    template <> struct _Real_widened<double, double>
    {
        typedef double _Type;
    };

    template <class _Ty> struct _Real_type
    {
        typedef double _Type;
    };

    template <> struct _Real_type<float>
    {
        typedef float _Type;
    };

    template <> struct _Real_type<long double>
    {
        typedef long double _Type;
    };

    template <class _T1, class _T2>
                   inline int _fpcomp( _T1 _X, _T2 _Y) throw()
    {
        typedef typename _Combined_type<float,
            typename _Real_widened<
            typename _Real_type<_T1>::_Type,
            typename _Real_type<_T2>::_Type>::_Type>::_Type _Tw;
        return _fpcomp((_Tw)_X, (_Tw)_Y);
    }

    template <class _Ty>
                   inline bool isfinite( _Ty _X) throw()
    {
        return fpclassify(_X) <= 0;
    }

    template <class _Ty>
                   inline bool isinf( _Ty _X) throw()
    {
        return fpclassify(_X) == 1;
    }

    template <class _Ty>
                   inline bool isnan( _Ty _X) throw()
    {
        return fpclassify(_X) == 2;
    }

    template <class _Ty>
                   inline bool isnormal( _Ty _X) throw()
    {
        return fpclassify(_X) == (-1);
    }

    template <class _Ty1, class _Ty2>
                   inline bool isgreater( _Ty1 _X, _Ty2 _Y) throw()
    {
        return (_fpcomp(_X, _Y) & 4) != 0;
    }

    template <class _Ty1, class _Ty2>
                   inline bool isgreaterequal( _Ty1 _X, _Ty2 _Y) throw()
    {
        return (_fpcomp(_X, _Y) & (2 | 4)) != 0;
    }

    template <class _Ty1, class _Ty2>
                   inline bool isless( _Ty1 _X, _Ty2 _Y) throw()
    {
        return (_fpcomp(_X, _Y) & 1) != 0;
    }

    template <class _Ty1, class _Ty2>
                   inline bool islessequal( _Ty1 _X, _Ty2 _Y) throw()
    {
        return (_fpcomp(_X, _Y) & (1 | 2)) != 0;
    }

    template <class _Ty1, class _Ty2>
                   inline bool islessgreater( _Ty1 _X, _Ty2 _Y) throw()
    {
        return (_fpcomp(_X, _Y) & (1 | 4)) != 0;
    }

    template <class _Ty1, class _Ty2>
                   inline bool isunordered( _Ty1 _X, _Ty2 _Y) throw()
    {
        return _fpcomp(_X, _Y) == 0;
    }
}






                   int __cdecl abs( int _X);
                   long __cdecl labs( long _X);
                   long long __cdecl llabs( long long _X);

                   double __cdecl acos( double _X);
                   double __cdecl asin( double _X);
                   double __cdecl atan( double _X);
                   double __cdecl atan2( double _Y, double _X);

                   double __cdecl cos( double _X);
                   double __cdecl cosh( double _X);
                   double __cdecl exp( double _X);
                                      double __cdecl fabs( double _X);
                   double __cdecl fmod( double _X, double _Y);
                   double __cdecl log( double _X);
                   double __cdecl log10( double _X);
                   double __cdecl pow( double _X, double _Y);
                   double __cdecl sin( double _X);
                   double __cdecl sinh( double _X);
                                      double __cdecl sqrt( double _X);
                   double __cdecl tan( double _X);
                   double __cdecl tanh( double _X);

                            double __cdecl acosh( double _X);
                            double __cdecl asinh( double _X);
                            double __cdecl atanh( double _X);
                             double __cdecl atof( char const* _String);
                             double __cdecl _atof_l( char const* _String, _locale_t _Locale);
                            double __cdecl _cabs( struct _complex _Complex_value);
                            double __cdecl cbrt( double _X);
                            double __cdecl ceil( double _X);
                            double __cdecl _chgsign( double _X);
                            double __cdecl copysign( double _Number, double _Sign);
                            double __cdecl _copysign( double _Number, double _Sign);
                            double __cdecl erf( double _X);
                            double __cdecl erfc( double _X);
                            double __cdecl exp2( double _X);
                            double __cdecl expm1( double _X);
                            double __cdecl fdim( double _X, double _Y);
                            double __cdecl floor( double _X);
                            double __cdecl fma( double _X, double _Y, double _Z);
                            double __cdecl fmax( double _X, double _Y);
                            double __cdecl fmin( double _X, double _Y);
                            double __cdecl frexp( double _X, int* _Y);
                            double __cdecl hypot( double _X, double _Y);
                            double __cdecl _hypot( double _X, double _Y);
                            int __cdecl ilogb( double _X);
                            double __cdecl ldexp( double _X, int _Y);
                            double __cdecl lgamma( double _X);
                            long long __cdecl llrint( double _X);
                            long long __cdecl llround( double _X);
                            double __cdecl log1p( double _X);
                            double __cdecl log2( double _X);
                            double __cdecl logb( double _X);
                            long __cdecl lrint( double _X);
                            long __cdecl lround( double _X);

    int __cdecl _matherr( struct _exception* _Except);

                            double __cdecl modf( double _X, double* _Y);
                            double __cdecl nan( char const* _X);
                            double __cdecl nearbyint( double _X);
                            double __cdecl nextafter( double _X, double _Y);
                            double __cdecl nexttoward( double _X, long double _Y);
                            double __cdecl remainder( double _X, double _Y);
                            double __cdecl remquo( double _X, double _Y, int* _Z);
                            double __cdecl rint( double _X);
                            double __cdecl round( double _X);
                            double __cdecl scalbln( double _X, long _Y);
                            double __cdecl scalbn( double _X, int _Y);
                            double __cdecl tgamma( double _X);
                            double __cdecl trunc( double _X);
                            double __cdecl _j0( double _X );
                            double __cdecl _j1( double _X );
                            double __cdecl _jn(int _X, double _Y);
                            double __cdecl _y0( double _X);
                            double __cdecl _y1( double _X);
                            double __cdecl _yn( int _X, double _Y);

                            float __cdecl acoshf( float _X);
                            float __cdecl asinhf( float _X);
                            float __cdecl atanhf( float _X);
                            float __cdecl cbrtf( float _X);
                            float __cdecl _chgsignf( float _X);
                            float __cdecl copysignf( float _Number, float _Sign);
                            float __cdecl _copysignf( float _Number, float _Sign);
                            float __cdecl erff( float _X);
                            float __cdecl erfcf( float _X);
                            float __cdecl expm1f( float _X);
                            float __cdecl exp2f( float _X);
                            float __cdecl fdimf( float _X, float _Y);
                            float __cdecl fmaf( float _X, float _Y, float _Z);
                            float __cdecl fmaxf( float _X, float _Y);
                            float __cdecl fminf( float _X, float _Y);
                            float __cdecl _hypotf( float _X, float _Y);
                            int __cdecl ilogbf( float _X);
                            float __cdecl lgammaf( float _X);
                            long long __cdecl llrintf( float _X);
                            long long __cdecl llroundf( float _X);
                            float __cdecl log1pf( float _X);
                            float __cdecl log2f( float _X);
                            float __cdecl logbf( float _X);
                            long __cdecl lrintf( float _X);
                            long __cdecl lroundf( float _X);
                            float __cdecl nanf( char const* _X);
                            float __cdecl nearbyintf( float _X);
                            float __cdecl nextafterf( float _X, float _Y);
                            float __cdecl nexttowardf( float _X, long double _Y);
                            float __cdecl remainderf( float _X, float _Y);
                            float __cdecl remquof( float _X, float _Y, int* _Z);
                            float __cdecl rintf( float _X);
                            float __cdecl roundf( float _X);
                            float __cdecl scalblnf( float _X, long _Y);
                            float __cdecl scalbnf( float _X, int _Y);
                            float __cdecl tgammaf( float _X);
                            float __cdecl truncf( float _X);
# 591 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_math.h" 3
                                float __cdecl _logbf( float _X);
                                float __cdecl _nextafterf( float _X, float _Y);
                                int __cdecl _finitef( float _X);
                                int __cdecl _isnanf( float _X);
                                int __cdecl _fpclassf( float _X);

                                int __cdecl _set_FMA3_enable( int _Flag);
                                int __cdecl _get_FMA3_enable(void);
# 611 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_math.h" 3
                                float __cdecl acosf( float _X);
                                float __cdecl asinf( float _X);
                                float __cdecl atan2f( float _Y, float _X);
                                float __cdecl atanf( float _X);
                                float __cdecl ceilf( float _X);
                                float __cdecl cosf( float _X);
                                float __cdecl coshf( float _X);
                                float __cdecl expf( float _X);
# 670 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_math.h" 3
                       __inline float __cdecl fabsf( float _X)
        {
            return (float)fabs(_X);
        }





                                float __cdecl floorf( float _X);
                                float __cdecl fmodf( float _X, float _Y);
# 696 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_math.h" 3
                   __inline float __cdecl frexpf( float _X, int *_Y)
    {
        return (float)frexp(_X, _Y);
    }

                   __inline float __cdecl hypotf( float _X, float _Y)
    {
        return _hypotf(_X, _Y);
    }

                   __inline float __cdecl ldexpf( float _X, int _Y)
    {
        return (float)ldexp(_X, _Y);
    }



                                float __cdecl log10f( float _X);
                                float __cdecl logf( float _X);
                                float __cdecl modff( float _X, float *_Y);
                                float __cdecl powf( float _X, float _Y);
                                float __cdecl sinf( float _X);
                                float __cdecl sinhf( float _X);
                                float __cdecl sqrtf( float _X);
                                float __cdecl tanf( float _X);
                                float __cdecl tanhf( float _X);
# 775 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_math.h" 3
                            long double __cdecl acoshl( long double _X);

                   __inline long double __cdecl acosl( long double _X)
    {
        return acos((double)_X);
    }

                            long double __cdecl asinhl( long double _X);

                   __inline long double __cdecl asinl( long double _X)
    {
        return asin((double)_X);
    }

                   __inline long double __cdecl atan2l( long double _Y, long double _X)
    {
        return atan2((double)_Y, (double)_X);
    }

                            long double __cdecl atanhl( long double _X);

                   __inline long double __cdecl atanl( long double _X)
    {
        return atan((double)_X);
    }

                            long double __cdecl cbrtl( long double _X);

                   __inline long double __cdecl ceill( long double _X)
    {
        return ceil((double)_X);
    }

                   __inline long double __cdecl _chgsignl( long double _X)
    {
        return _chgsign((double)_X);
    }

                            long double __cdecl copysignl( long double _Number, long double _Sign);

                   __inline long double __cdecl _copysignl( long double _Number, long double _Sign)
    {
        return _copysign((double)_Number, (double)_Sign);
    }

                   __inline long double __cdecl coshl( long double _X)
    {
        return cosh((double)_X);
    }

                   __inline long double __cdecl cosl( long double _X)
    {
        return cos((double)_X);
    }

                            long double __cdecl erfl( long double _X);
                            long double __cdecl erfcl( long double _X);

                   __inline long double __cdecl expl( long double _X)
    {
        return exp((double)_X);
    }

                            long double __cdecl exp2l( long double _X);
                            long double __cdecl expm1l( long double _X);

                   __inline long double __cdecl fabsl( long double _X)
    {
        return fabs((double)_X);
    }

                            long double __cdecl fdiml( long double _X, long double _Y);

                   __inline long double __cdecl floorl( long double _X)
    {
        return floor((double)_X);
    }

                            long double __cdecl fmal( long double _X, long double _Y, long double _Z);
                            long double __cdecl fmaxl( long double _X, long double _Y);
                            long double __cdecl fminl( long double _X, long double _Y);

                   __inline long double __cdecl fmodl( long double _X, long double _Y)
    {
        return fmod((double)_X, (double)_Y);
    }

                   __inline long double __cdecl frexpl( long double _X, int *_Y)
    {
        return frexp((double)_X, _Y);
    }

                            int __cdecl ilogbl( long double _X);

                   __inline long double __cdecl _hypotl( long double _X, long double _Y)
    {
        return _hypot((double)_X, (double)_Y);
    }

                   __inline long double __cdecl hypotl( long double _X, long double _Y)
    {
        return _hypot((double)_X, (double)_Y);
    }

                   __inline long double __cdecl ldexpl( long double _X, int _Y)
    {
        return ldexp((double)_X, _Y);
    }

                            long double __cdecl lgammal( long double _X);
                            long long __cdecl llrintl( long double _X);
                            long long __cdecl llroundl( long double _X);

                   __inline long double __cdecl logl( long double _X)
    {
        return log((double)_X);
    }

                   __inline long double __cdecl log10l( long double _X)
    {
        return log10((double)_X);
    }

                            long double __cdecl log1pl( long double _X);
                            long double __cdecl log2l( long double _X);
                            long double __cdecl logbl( long double _X);
                            long __cdecl lrintl( long double _X);
                            long __cdecl lroundl( long double _X);

                   __inline long double __cdecl modfl( long double _X, long double* _Y)
    {
        double _F, _I;
        _F = modf((double)_X, &_I);
        *_Y = _I;
        return _F;
    }

                            long double __cdecl nanl( char const* _X);
                            long double __cdecl nearbyintl( long double _X);
                            long double __cdecl nextafterl( long double _X, long double _Y);
                            long double __cdecl nexttowardl( long double _X, long double _Y);

                   __inline long double __cdecl powl( long double _X, long double _Y)
    {
        return pow((double)_X, (double)_Y);
    }

                            long double __cdecl remainderl( long double _X, long double _Y);
                            long double __cdecl remquol( long double _X, long double _Y, int* _Z);
                            long double __cdecl rintl( long double _X);
                            long double __cdecl roundl( long double _X);
                            long double __cdecl scalblnl( long double _X, long _Y);
                            long double __cdecl scalbnl( long double _X, int _Y);

                   __inline long double __cdecl sinhl( long double _X)
    {
        return sinh((double)_X);
    }

                   __inline long double __cdecl sinl( long double _X)
    {
        return sin((double)_X);
    }

                   __inline long double __cdecl sqrtl( long double _X)
    {
        return sqrt((double)_X);
    }

                   __inline long double __cdecl tanhl( long double _X)
    {
        return tanh((double)_X);
    }

                   __inline long double __cdecl tanl( long double _X)
    {
        return tan((double)_X);
    }

                            long double __cdecl tgammal( long double _X);
                            long double __cdecl truncl( long double _X);
# 976 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_math.h" 3
            extern double HUGE;




        __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_j0" ". See online help for details.")) double __cdecl j0( double _X);
        __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_j1" ". See online help for details.")) double __cdecl j1( double _X);
        __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_jn" ". See online help for details.")) double __cdecl jn( int _X, double _Y);
        __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_y0" ". See online help for details.")) double __cdecl y0( double _X);
        __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_y1" ". See online help for details.")) double __cdecl y1( double _X);
        __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_yn" ". See online help for details.")) double __cdecl yn( int _X, double _Y);





#pragma warning(pop)

}
# 994 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_math.h" 3
#pragma pack(pop)
# 12 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\math.h" 2 3
# 13 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\cmath" 2 3




               inline double abs( double _Xx) noexcept
 {
 return (:: fabs(_Xx));
 }

               inline double pow( double _Xx, int _Yx) noexcept
 {
 if (_Yx == 2)
  return (_Xx * _Xx);

 return (:: pow(_Xx, static_cast<double>(_Yx)));
 }

               inline float abs( float _Xx) noexcept
 {
 return (:: fabsf(_Xx));
 }

               inline float acos( float _Xx) noexcept
 {
 return (:: acosf(_Xx));
 }

               inline float acosh( float _Xx) noexcept
 {
 return (:: acoshf(_Xx));
 }

               inline float asin( float _Xx) noexcept
 {
 return (:: asinf(_Xx));
 }

               inline float asinh( float _Xx) noexcept
 {
 return (:: asinhf(_Xx));
 }

               inline float atan( float _Xx) noexcept
 {
 return (:: atanf(_Xx));
 }

               inline float atanh( float _Xx) noexcept
 {
 return (:: atanhf(_Xx));
 }

               inline float atan2( float _Yx, float _Xx) noexcept
 {
 return (:: atan2f(_Yx, _Xx));
 }

               inline float cbrt( float _Xx) noexcept
 {
 return (:: cbrtf(_Xx));
 }

               inline float ceil( float _Xx) noexcept
 {
 return (:: ceilf(_Xx));
 }

               inline float copysign( float _Number,
      float _Sign) noexcept
 {
 return (:: copysignf(_Number, _Sign));
 }

               inline float cos( float _Xx) noexcept
 {
 return (:: cosf(_Xx));
 }

               inline float cosh( float _Xx) noexcept
 {
 return (:: coshf(_Xx));
 }

               inline float erf( float _Xx) noexcept
 {
 return (:: erff(_Xx));
 }

               inline float erfc( float _Xx) noexcept
 {
 return (:: erfcf(_Xx));
 }

               inline float exp( float _Xx) noexcept
 {
 return (:: expf(_Xx));
 }

               inline float exp2( float _Xx) noexcept
 {
 return (:: exp2f(_Xx));
 }

               inline float expm1( float _Xx) noexcept
 {
 return (:: expm1f(_Xx));
 }

               inline float fabs( float _Xx) noexcept
 {
 return (:: fabsf(_Xx));
 }

               inline float fdim( float _Xx, float _Yx) noexcept
 {
 return (:: fdimf(_Xx, _Yx));
 }

               inline float floor( float _Xx) noexcept
 {
 return (:: floorf(_Xx));
 }

               inline float fma( float _Xx, float _Yx,
      float _Zx) noexcept
 {
 return (:: fmaf(_Xx, _Yx, _Zx));
 }

               inline float fmax( float _Xx, float _Yx) noexcept
 {
 return (:: fmaxf(_Xx, _Yx));
 }

               inline float fmin( float _Xx, float _Yx) noexcept
 {
 return (:: fminf(_Xx, _Yx));
 }

               inline float fmod( float _Xx, float _Yx) noexcept
 {
 return (:: fmodf(_Xx, _Yx));
 }

               inline float frexp( float _Xx, int* _Yx) noexcept
 {
 return (:: frexpf(_Xx, _Yx));
 }

               inline float hypot( float _Xx, float _Yx) noexcept
 {
 return (:: hypotf(_Xx, _Yx));
 }

               inline int ilogb( float _Xx) noexcept
 {
 return (:: ilogbf(_Xx));
 }

               inline float ldexp( float _Xx, int _Yx) noexcept
 {
 return (:: ldexpf(_Xx, _Yx));
 }

               inline float lgamma( float _Xx) noexcept
 {
 return (:: lgammaf(_Xx));
 }

               inline long long llrint( float _Xx) noexcept
 {
 return (:: llrintf(_Xx));
 }

               inline long long llround( float _Xx) noexcept
 {
 return (:: llroundf(_Xx));
 }

               inline float log( float _Xx) noexcept
 {
 return (:: logf(_Xx));
 }

               inline float log10( float _Xx) noexcept
 {
 return (:: log10f(_Xx));
 }

               inline float log1p( float _Xx) noexcept
 {
 return (:: log1pf(_Xx));
 }

               inline float log2( float _Xx) noexcept
 {
 return (:: log2f(_Xx));
 }

               inline float logb( float _Xx) noexcept
 {
 return (:: logbf(_Xx));
 }

               inline long lrint( float _Xx) noexcept
 {
 return (:: lrintf(_Xx));
 }

               inline long lround( float _Xx) noexcept
 {
 return (:: lroundf(_Xx));
 }

               inline float modf( float _Xx, float* _Yx) noexcept
 {
 return (:: modff(_Xx, _Yx));
 }

               inline float nearbyint( float _Xx) noexcept
 {
 return (:: nearbyintf(_Xx));
 }

               inline float nextafter( float _Xx, float _Yx) noexcept
 {
 return (:: nextafterf(_Xx, _Yx));
 }

               inline float nexttoward( float _Xx,
      long double _Yx) noexcept
 {
 return (:: nexttowardf(_Xx, _Yx));
 }

               inline float pow( float _Xx,
      float _Yx) noexcept
 {
 return (:: powf(_Xx, _Yx));
 }

               inline float pow( float _Xx, int _Yx) noexcept
 {
 if (_Yx == 2)
  return (_Xx * _Xx);

 return (:: powf(_Xx, static_cast<float>(_Yx)));
 }

               inline float remainder( float _Xx, float _Yx) noexcept
 {
 return (:: remainderf(_Xx, _Yx));
 }

               inline float remquo( float _Xx, float _Yx,
       int *_Zx) noexcept
 {
 return (:: remquof(_Xx, _Yx, _Zx));
 }

               inline float rint( float _Xx) noexcept
 {
 return (:: rintf(_Xx));
 }

               inline float round( float _Xx) noexcept
 {
 return (:: roundf(_Xx));
 }

               inline float scalbln( float _Xx, long _Yx) noexcept
 {
 return (:: scalblnf(_Xx, _Yx));
 }

               inline float scalbn( float _Xx, int _Yx) noexcept
 {
 return (:: scalbnf(_Xx, _Yx));
 }

               inline float sin( float _Xx) noexcept
 {
 return (:: sinf(_Xx));
 }

               inline float sinh( float _Xx) noexcept
 {
 return (:: sinhf(_Xx));
 }

               inline float sqrt( float _Xx) noexcept
 {
 return (:: sqrtf(_Xx));
 }

               inline float tan( float _Xx) noexcept
 {
 return (:: tanf(_Xx));
 }

               inline float tanh( float _Xx) noexcept
 {
 return (:: tanhf(_Xx));
 }

               inline float tgamma( float _Xx) noexcept
 {
 return (:: tgammaf(_Xx));
 }

               inline float trunc( float _Xx) noexcept
 {
 return (:: truncf(_Xx));
 }

               inline long double abs( long double _Xx) noexcept
 {
 return (:: fabsl(_Xx));
 }

               inline long double acos( long double _Xx) noexcept
 {
 return (:: acosl(_Xx));
 }

               inline long double acosh( long double _Xx) noexcept
 {
 return (:: acoshl(_Xx));
 }

               inline long double asin( long double _Xx) noexcept
 {
 return (:: asinl(_Xx));
 }

               inline long double asinh( long double _Xx) noexcept
 {
 return (:: asinhl(_Xx));
 }

               inline long double atan( long double _Xx) noexcept
 {
 return (:: atanl(_Xx));
 }

               inline long double atanh( long double _Xx) noexcept
 {
 return (:: atanhl(_Xx));
 }

               inline long double atan2( long double _Yx,
      long double _Xx) noexcept
 {
 return (:: atan2l(_Yx, _Xx));
 }

               inline long double cbrt( long double _Xx) noexcept
 {
 return (:: cbrtl(_Xx));
 }

               inline long double ceil( long double _Xx) noexcept
 {
 return (:: ceill(_Xx));
 }

               inline long double copysign( long double _Number,
      long double _Sign) noexcept
 {
 return (:: copysignl(_Number, _Sign));
 }

               inline long double cos( long double _Xx) noexcept
 {
 return (:: cosl(_Xx));
 }

               inline long double cosh( long double _Xx) noexcept
 {
 return (:: coshl(_Xx));
 }

               inline long double erf( long double _Xx) noexcept
 {
 return (:: erfl(_Xx));
 }

               inline long double erfc( long double _Xx) noexcept
 {
 return (:: erfcl(_Xx));
 }

               inline long double exp( long double _Xx) noexcept
 {
 return (:: expl(_Xx));
 }

               inline long double exp2( long double _Xx) noexcept
 {
 return (:: exp2l(_Xx));
 }

               inline long double expm1( long double _Xx) noexcept
 {
 return (:: expm1l(_Xx));
 }

               inline long double fabs( long double _Xx) noexcept
 {
 return (:: fabsl(_Xx));
 }

               inline long double fdim( long double _Xx,
      long double _Yx) noexcept
 {
 return (:: fdiml(_Xx, _Yx));
 }

               inline long double floor( long double _Xx) noexcept
 {
 return (:: floorl(_Xx));
 }

               inline long double fma( long double _Xx,
      long double _Yx, long double _Zx) noexcept
 {
 return (:: fmal(_Xx, _Yx, _Zx));
 }

               inline long double fmax( long double _Xx,
      long double _Yx) noexcept
 {
 return (:: fmaxl(_Xx, _Yx));
 }

               inline long double fmin( long double _Xx,
      long double _Yx) noexcept
 {
 return (:: fminl(_Xx, _Yx));
 }

               inline long double fmod( long double _Xx,
      long double _Yx) noexcept
 {
 return (:: fmodl(_Xx, _Yx));
 }

               inline long double frexp( long double _Xx,
       int* _Yx) noexcept
 {
 return (:: frexpl(_Xx, _Yx));
 }

               inline long double hypot( long double _Xx,
      long double _Yx) noexcept
 {
 return (:: hypotl(_Xx, _Yx));
 }

               inline int ilogb( long double _Xx) noexcept
 {
 return (:: ilogbl(_Xx));
 }

               inline long double ldexp( long double _Xx,
      int _Yx) noexcept
 {
 return (:: ldexpl(_Xx, _Yx));
 }

               inline long double lgamma( long double _Xx) noexcept
 {
 return (:: lgammal(_Xx));
 }

               inline long long llrint( long double _Xx) noexcept
 {
 return (:: llrintl(_Xx));
 }

               inline long long llround( long double _Xx) noexcept
 {
 return (:: llroundl(_Xx));
 }

               inline long double log( long double _Xx) noexcept
 {
 return (:: logl(_Xx));
 }

               inline long double log10( long double _Xx) noexcept
 {
 return (:: log10l(_Xx));
 }

               inline long double log1p( long double _Xx) noexcept
 {
 return (:: log1pl(_Xx));
 }

               inline long double log2( long double _Xx) noexcept
 {
 return (:: log2l(_Xx));
 }

               inline long double logb( long double _Xx) noexcept
 {
 return (:: logbl(_Xx));
 }

               inline long lrint( long double _Xx) noexcept
 {
 return (:: lrintl(_Xx));
 }

               inline long lround( long double _Xx) noexcept
 {
 return (:: lroundl(_Xx));
 }

               inline long double modf( long double _Xx,
       long double* _Yx) noexcept
 {
 return (:: modfl(_Xx, _Yx));
 }

               inline long double nearbyint( long double _Xx) noexcept
 {
 return (:: nearbyintl(_Xx));
 }

               inline long double nextafter( long double _Xx,
      long double _Yx) noexcept
 {
 return (:: nextafterl(_Xx, _Yx));
 }

               inline long double nexttoward( long double _Xx,
      long double _Yx) noexcept
 {
 return (:: nexttowardl(_Xx, _Yx));
 }

               inline long double pow( long double _Xx,
      long double _Yx) noexcept
 {
 return (:: powl(_Xx, _Yx));
 }

               inline long double pow( long double _Xx,
      int _Yx) noexcept
 {
 if (_Yx == 2)
  return (_Xx * _Xx);

 return (:: powl(_Xx, static_cast<long double>(_Yx)));
 }

               inline long double remainder( long double _Xx,
      long double _Yx) noexcept
 {
 return (:: remainderl(_Xx, _Yx));
 }

               inline long double remquo( long double _Xx,
      long double _Yx, int *_Zx) noexcept
 {
 return (:: remquol(_Xx, _Yx, _Zx));
 }

               inline long double rint( long double _Xx) noexcept
 {
 return (:: rintl(_Xx));
 }

               inline long double round( long double _Xx) noexcept
 {
 return (:: roundl(_Xx));
 }

               inline long double scalbln( long double _Xx,
      long _Yx) noexcept
 {
 return (:: scalblnl(_Xx, _Yx));
 }

               inline long double scalbn( long double _Xx,
      int _Yx) noexcept
 {
 return (:: scalbnl(_Xx, _Yx));
 }

               inline long double sin( long double _Xx) noexcept
 {
 return (:: sinl(_Xx));
 }

               inline long double sinh( long double _Xx) noexcept
 {
 return (:: sinhl(_Xx));
 }

               inline long double sqrt( long double _Xx) noexcept
 {
 return (:: sqrtl(_Xx));
 }

               inline long double tan( long double _Xx) noexcept
 {
 return (:: tanl(_Xx));
 }

               inline long double tanh( long double _Xx) noexcept
 {
 return (:: tanhl(_Xx));
 }

               inline long double tgamma( long double _Xx) noexcept
 {
 return (:: tgammal(_Xx));
 }

               inline long double trunc( long double _Xx) noexcept
 {
 return (:: truncl(_Xx));
 }
# 647 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\cmath" 3
# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\xtgmath.h" 1 3







# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\xtr1common" 1 3





# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\cstdlib" 1 3
# 13 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\cstdlib" 3
# 1 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 1 3
# 13 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
# 1 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_malloc.h" 1 3
# 13 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_malloc.h" 3
#pragma pack(push, 8)
# 13 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_malloc.h" 3
 extern "C" {
# 54 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_malloc.h" 3
         __declspec(allocator) __declspec(restrict)
void* __cdecl _calloc_base(
         size_t _Count,
         size_t _Size
    );


                            __declspec(allocator) __declspec(restrict)
void* __cdecl calloc(
                            size_t _Count,
                            size_t _Size
    );


         int __cdecl _callnewh(
         size_t _Size
    );


         __declspec(allocator)
void* __cdecl _expand(
                            void* _Block,
                            size_t _Size
    );


void __cdecl _free_base(
                                   void* _Block
    );


void __cdecl free(
                                   void* _Block
    );


         __declspec(allocator) __declspec(restrict)
void* __cdecl _malloc_base(
         size_t _Size
    );


         __declspec(allocator) __declspec(restrict)
void* __cdecl malloc(
                            size_t _Size
    );



size_t __cdecl _msize_base(
                  void* _Block
    );



size_t __cdecl _msize(
                  void* _Block
    );


         __declspec(allocator) __declspec(restrict)
void* __cdecl _realloc_base(
                                    void* _Block,
                                    size_t _Size
    );


         __declspec(allocator) __declspec(restrict)
void* __cdecl realloc(
                                   void* _Block,
                                   size_t _Size
    );


         __declspec(allocator) __declspec(restrict)
void* __cdecl _recalloc_base(
                                   void* _Block,
                                   size_t _Count,
                                   size_t _Size
    );


         __declspec(allocator) __declspec(restrict)
void* __cdecl _recalloc(
                                   void* _Block,
                                   size_t _Count,
                                   size_t _Size
    );


void __cdecl _aligned_free(
                                   void* _Block
    );


         __declspec(allocator) __declspec(restrict)
void* __cdecl _aligned_malloc(
                            size_t _Size,
                            size_t _Alignment
    );


         __declspec(allocator) __declspec(restrict)
void* __cdecl _aligned_offset_malloc(
                            size_t _Size,
                            size_t _Alignment,
                            size_t _Offset
    );



size_t __cdecl _aligned_msize(
                  void* _Block,
                  size_t _Alignment,
                  size_t _Offset
    );


         __declspec(allocator) __declspec(restrict)
void* __cdecl _aligned_offset_realloc(
                                   void* _Block,
                                   size_t _Size,
                                   size_t _Alignment,
                                   size_t _Offset
    );


         __declspec(allocator) __declspec(restrict)
void* __cdecl _aligned_offset_recalloc(
                                   void* _Block,
                                   size_t _Count,
                                   size_t _Size,
                                   size_t _Alignment,
                                   size_t _Offset
    );


         __declspec(allocator) __declspec(restrict)
void* __cdecl _aligned_realloc(
                                   void* _Block,
                                   size_t _Size,
                                   size_t _Alignment
    );


         __declspec(allocator) __declspec(restrict)
void* __cdecl _aligned_recalloc(
                                   void* _Block,
                                   size_t _Count,
                                   size_t _Size,
                                   size_t _Alignment
    );
# 228 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_malloc.h" 3
}
# 228 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_malloc.h" 3
#pragma pack(pop)
# 14 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 2 3
# 1 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_search.h" 1 3
# 14 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_search.h" 3
# 1 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\stddef.h" 1 3
# 15 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_search.h" 2 3

#pragma pack(push, 8)
# 16 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_search.h" 3
 extern "C" {


    typedef int (__cdecl* _CoreCrtSecureSearchSortCompareFunction)(void*, void const*, void const*);
    typedef int (__cdecl* _CoreCrtNonSecureSearchSortCompareFunction)(void const*, void const*);





             void* __cdecl bsearch_s(
                                                           void const* _Key,
                                                           void const* _Base,
                                                           rsize_t _NumOfElements,
                                                           rsize_t _SizeOfElements,
                               _CoreCrtSecureSearchSortCompareFunction _CompareFunction,
                                                           void* _Context
        );

             void __cdecl qsort_s(
                                                                void* _Base,
                                                                rsize_t _NumOfElements,
                                                                rsize_t _SizeOfElements,
                                _CoreCrtSecureSearchSortCompareFunction _CompareFunction,
                                                                void* _Context
        );






         void* __cdecl bsearch(
                                                       void const* _Key,
                                                       void const* _Base,
                                                       size_t _NumOfElements,
                                                       size_t _SizeOfElements,
                        _CoreCrtNonSecureSearchSortCompareFunction _CompareFunction
    );

         void __cdecl qsort(
                                                            void* _Base,
                                                            size_t _NumOfElements,
                                                            size_t _SizeOfElements,
                        _CoreCrtNonSecureSearchSortCompareFunction _CompareFunction
    );


         void* __cdecl _lfind_s(
                                                          void const* _Key,
                                                          void const* _Base,
                                                          unsigned int* _NumOfElements,
                                                          size_t _SizeOfElements,
                                _CoreCrtSecureSearchSortCompareFunction _CompareFunction,
                                                          void* _Context
    );


         void* __cdecl _lfind(
                                                          void const* _Key,
                                                          void const* _Base,
                                                          unsigned int* _NumOfElements,
                                                          unsigned int _SizeOfElements,
                             _CoreCrtNonSecureSearchSortCompareFunction _CompareFunction
    );


         void* __cdecl _lsearch_s(
                                                                void const* _Key,
                                                                void* _Base,
                                                                unsigned int* _NumOfElements,
                                                                size_t _SizeOfElements,
                                      _CoreCrtSecureSearchSortCompareFunction _CompareFunction,
                                                                void* _Context
    );


         void* __cdecl _lsearch(
                                                                void const* _Key,
                                                                void* _Base,
                                                                unsigned int* _NumOfElements,
                                                                unsigned int _SizeOfElements,
                                   _CoreCrtNonSecureSearchSortCompareFunction _CompareFunction
    );
# 190 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_search.h" 3
                   __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_lfind" ". See online help for details."))
             void* __cdecl lfind(
                                                              void const* _Key,
                                                              void const* _Base,
                                                              unsigned int* _NumOfElements,
                                                              unsigned int _SizeOfElements,
                                 _CoreCrtNonSecureSearchSortCompareFunction _CompareFunction
        );

                   __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_lsearch" ". See online help for details."))
             void* __cdecl lsearch(
                                                                   void const* _Key,
                                                                   void* _Base,
                                                                   unsigned int* _NumOfElements,
                                                                   unsigned int _SizeOfElements,
                                      _CoreCrtNonSecureSearchSortCompareFunction _CompareFunction
        );





}
# 212 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_search.h" 3
#pragma pack(pop)
# 15 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 2 3
# 1 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_wstdlib.h" 1 3
# 13 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_wstdlib.h" 3
#pragma pack(push, 8)
# 13 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_wstdlib.h" 3
 extern "C" {
# 50 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_wstdlib.h" 3
             errno_t __cdecl _itow_s(
                                     int _Value,
                                     wchar_t* _Buffer,
                                     size_t _BufferCount,
                                     int _Radix
        );

    extern "C++" { template <size_t _Size> inline errno_t __cdecl _itow_s(int _Value, wchar_t (&_Buffer)[_Size], int _Radix) throw() { return _itow_s(_Value, _Buffer, _Size, _Radix); } }






    __declspec(deprecated("This function or variable may be unsafe. Consider using " "_itow_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) wchar_t* __cdecl _itow(int _Value, wchar_t *_Buffer, int _Radix);
# 73 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_wstdlib.h" 3
             errno_t __cdecl _ltow_s(
                                     long _Value,
                                     wchar_t* _Buffer,
                                     size_t _BufferCount,
                                     int _Radix
        );

    extern "C++" { template <size_t _Size> inline errno_t __cdecl _ltow_s(long _Value, wchar_t (&_Buffer)[_Size], int _Radix) throw() { return _ltow_s(_Value, _Buffer, _Size, _Radix); } }






    __declspec(deprecated("This function or variable may be unsafe. Consider using " "_ltow_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) wchar_t* __cdecl _ltow(long _Value, wchar_t *_Buffer, int _Radix);







             errno_t __cdecl _ultow_s(
                                     unsigned long _Value,
                                     wchar_t* _Buffer,
                                     size_t _BufferCount,
                                     int _Radix
        );

    extern "C++" { template <size_t _Size> inline errno_t __cdecl _ultow_s(unsigned long _Value, wchar_t (&_Buffer)[_Size], int _Radix) throw() { return _ultow_s(_Value, _Buffer, _Size, _Radix); } }






    __declspec(deprecated("This function or variable may be unsafe. Consider using " "_ultow_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) wchar_t* __cdecl _ultow(unsigned long _Value, wchar_t *_Buffer, int _Radix);







             double __cdecl wcstod(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr
        );


             double __cdecl _wcstod_l(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr,
                                 _locale_t _Locale
        );


             long __cdecl wcstol(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr,
                                 int _Radix
        );


             long __cdecl _wcstol_l(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr,
                                 int _Radix,
                                 _locale_t _Locale
        );


             long long __cdecl wcstoll(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr,
                                 int _Radix
        );


             long long __cdecl _wcstoll_l(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr,
                                 int _Radix,
                                 _locale_t _Locale
        );


             unsigned long __cdecl wcstoul(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr,
                                 int _Radix
        );


             unsigned long __cdecl _wcstoul_l(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr,
                                 int _Radix,
                                 _locale_t _Locale
        );


             unsigned long long __cdecl wcstoull(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr,
                                 int _Radix
        );


             unsigned long long __cdecl _wcstoull_l(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr,
                                 int _Radix,
                                 _locale_t _Locale
        );


             long double __cdecl wcstold(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr
        );


             long double __cdecl _wcstold_l(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr,
                                 _locale_t _Locale
        );


             float __cdecl wcstof(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr
        );


             float __cdecl _wcstof_l(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr,
                                 _locale_t _Locale
        );


             double __cdecl _wtof(
               wchar_t const* _String
        );


             double __cdecl _wtof_l(
                 wchar_t const* _String,
                 _locale_t _Locale
        );


             int __cdecl _wtoi(
               wchar_t const* _String
        );


             int __cdecl _wtoi_l(
                 wchar_t const* _String,
                 _locale_t _Locale
        );


             long __cdecl _wtol(
               wchar_t const* _String
        );


             long __cdecl _wtol_l(
                 wchar_t const* _String,
                 _locale_t _Locale
        );


             long long __cdecl _wtoll(
               wchar_t const* _String
        );


             long long __cdecl _wtoll_l(
                 wchar_t const* _String,
                 _locale_t _Locale
        );


             errno_t __cdecl _i64tow_s(
                                     __int64 _Value,
                                     wchar_t* _Buffer,
                                     size_t _BufferCount,
                                     int _Radix
        );

    __declspec(deprecated("This function or variable may be unsafe. Consider using " "_i64tow_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
             wchar_t* __cdecl _i64tow(
                               __int64 _Value,
                               wchar_t* _Buffer,
                               int _Radix
        );


             errno_t __cdecl _ui64tow_s(
                                     unsigned __int64 _Value,
                                     wchar_t* _Buffer,
                                     size_t _BufferCount,
                                     int _Radix
        );

    __declspec(deprecated("This function or variable may be unsafe. Consider using " "_ui64tow_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
             wchar_t* __cdecl _ui64tow(
                               unsigned __int64 _Value,
                               wchar_t* _Buffer,
                               int _Radix
        );


             __int64 __cdecl _wtoi64(
               wchar_t const* _String
        );


             __int64 __cdecl _wtoi64_l(
                 wchar_t const* _String,
                 _locale_t _Locale
        );


             __int64 __cdecl _wcstoi64(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr,
                                 int _Radix
        );


             __int64 __cdecl _wcstoi64_l(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr,
                                 int _Radix,
                                 _locale_t _Locale
        );


             unsigned __int64 __cdecl _wcstoui64(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr,
                                 int _Radix
        );


             unsigned __int64 __cdecl _wcstoui64_l(
                                 wchar_t const* _String,
                                 wchar_t** _EndPtr,
                                 int _Radix,
                                 _locale_t _Locale
        );






             __declspec(allocator) wchar_t* __cdecl _wfullpath(
                                         wchar_t* _Buffer,
                                         wchar_t const* _Path,
                                         size_t _BufferCount
        );




             errno_t __cdecl _wmakepath_s(
                                     wchar_t* _Buffer,
                                     size_t _BufferCount,
                                     wchar_t const* _Drive,
                                     wchar_t const* _Dir,
                                     wchar_t const* _Filename,
                                     wchar_t const* _Ext
        );

    extern "C++" { template <size_t _Size> inline errno_t __cdecl _wmakepath_s(wchar_t (&_Buffer)[_Size], wchar_t const* _Drive, wchar_t const* _Dir, wchar_t const* _Filename, wchar_t const* _Ext) throw() { return _wmakepath_s(_Buffer, _Size, _Drive, _Dir, _Filename, _Ext); } }








#pragma warning(push)
#pragma warning(disable: 28719)
#pragma warning(disable: 28726)
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wmakepath_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) void __cdecl _wmakepath( wchar_t *_Buffer, wchar_t const* _Drive, wchar_t const* _Dir, wchar_t const* _Filename, wchar_t const* _Ext);







#pragma warning(pop)

 void __cdecl _wperror(
                   wchar_t const* _ErrorMessage
        );

    __declspec(deprecated("This function or variable may be unsafe. Consider using " "_wsplitpath_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
             void __cdecl _wsplitpath(
                                 wchar_t const* _FullPath,
                                 wchar_t* _Drive,
                                 wchar_t* _Dir,
                                 wchar_t* _Filename,
                                 wchar_t* _Ext
        );

             errno_t __cdecl _wsplitpath_s(
                                           wchar_t const* _FullPath,
                                           wchar_t* _Drive,
                                           size_t _DriveCount,
                                           wchar_t* _Dir,
                                           size_t _DirCount,
                                           wchar_t* _Filename,
                                           size_t _FilenameCount,
                                           wchar_t* _Ext,
                                           size_t _ExtCount
        );

    extern "C++" { template <size_t _DriveSize, size_t _DirSize, size_t _NameSize, size_t _ExtSize> inline errno_t __cdecl _wsplitpath_s( wchar_t const* _Path, wchar_t (&_Drive)[_DriveSize], wchar_t (&_Dir)[_DirSize], wchar_t (&_Name)[_NameSize], wchar_t (&_Ext)[_ExtSize] ) throw() { return _wsplitpath_s(_Path, _Drive, _DriveSize, _Dir, _DirSize, _Name, _NameSize, _Ext, _ExtSize); } }
# 409 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_wstdlib.h" 3
                 errno_t __cdecl _wdupenv_s(
                                                                                        wchar_t** _Buffer,
                                                                                        size_t* _BufferCount,
                                                                                        wchar_t const* _VarName
            );



                       __declspec(deprecated("This function or variable may be unsafe. Consider using " "_wdupenv_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
                 wchar_t* __cdecl _wgetenv(
                   wchar_t const* _VarName
            );



                 errno_t __cdecl _wgetenv_s(
                                             size_t* _RequiredCount,
                                             wchar_t* _Buffer,
                                             size_t _BufferCount,
                                             wchar_t const* _VarName
            );

        extern "C++" { template <size_t _Size> inline errno_t __cdecl _wgetenv_s(size_t* _RequiredCount, wchar_t (&_Buffer)[_Size], wchar_t const* _VarName) throw() { return _wgetenv_s(_RequiredCount, _Buffer, _Size, _VarName); } }
# 440 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_wstdlib.h" 3
                 int __cdecl _wputenv(
                   wchar_t const* _EnvString
            );


                 errno_t __cdecl _wputenv_s(
                   wchar_t const* _Name,
                   wchar_t const* _Value
            );

                 errno_t __cdecl _wsearchenv_s(
                                         wchar_t const* _Filename,
                                         wchar_t const* _VarName,
                                         wchar_t* _Buffer,
                                         size_t _BufferCount
            );

        extern "C++" { template <size_t _Size> inline errno_t __cdecl _wsearchenv_s(wchar_t const* _Filename, wchar_t const* _VarName, wchar_t (&_ResultPath)[_Size]) throw() { return _wsearchenv_s(_Filename, _VarName, _ResultPath, _Size); } }






        __declspec(deprecated("This function or variable may be unsafe. Consider using " "_wsearchenv_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) void __cdecl _wsearchenv(wchar_t const* _Filename, wchar_t const* _VarName, wchar_t *_ResultPath);






                 int __cdecl _wsystem(
                       wchar_t const* _Command
            );





}
# 479 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\corecrt_wstdlib.h" 3
#pragma pack(pop)
# 16 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 2 3
# 1 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\limits.h" 1 3
# 37 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\limits.h" 3
# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\limits.h" 1 3
# 11 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\limits.h" 3
# 1 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\vcruntime.h" 1 3
# 12 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\limits.h" 2 3

#pragma pack(push, 8)
# 13 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\limits.h" 3
 extern "C" {
# 88 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\limits.h" 3
}
# 88 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\limits.h" 3
#pragma pack(pop)
# 38 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\limits.h" 2 3
# 17 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 2 3

#pragma pack(push, 8)
# 18 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
 extern "C" {
# 34 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
         void __cdecl _swab(
                                                                     char* _Buf1,
                                                                     char* _Buf2,
                                                                     int _SizeInBytes
    );
# 52 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
             __declspec(noreturn) void __cdecl exit( int _Code);
             __declspec(noreturn) void __cdecl _exit( int _Code);
             __declspec(noreturn) void __cdecl _Exit( int _Code);
             __declspec(noreturn) void __cdecl quick_exit( int _Code);
             __declspec(noreturn) void __cdecl abort(void);






         unsigned int __cdecl _set_abort_behavior(
         unsigned int _Flags,
         unsigned int _Mask
    );






    typedef int (__cdecl* _onexit_t)(void);
# 140 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
    int __cdecl atexit(void (__cdecl*)(void));
    _onexit_t __cdecl _onexit( _onexit_t _Func);


int __cdecl at_quick_exit(void (__cdecl*)(void));
# 155 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
    typedef void (__cdecl* _purecall_handler)(void);


    typedef void (__cdecl* _invalid_parameter_handler)(
        wchar_t const*,
        wchar_t const*,
        wchar_t const*,
        unsigned int,
        uintptr_t
        );


             _purecall_handler __cdecl _set_purecall_handler(
                 _purecall_handler _Handler
        );

             _purecall_handler __cdecl _get_purecall_handler(void);


             _invalid_parameter_handler __cdecl _set_invalid_parameter_handler(
                 _invalid_parameter_handler _Handler
        );

             _invalid_parameter_handler __cdecl _get_invalid_parameter_handler(void);

             _invalid_parameter_handler __cdecl _set_thread_local_invalid_parameter_handler(
                 _invalid_parameter_handler _Handler
        );

             _invalid_parameter_handler __cdecl _get_thread_local_invalid_parameter_handler(void);
# 208 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
                            int __cdecl _set_error_mode( int _Mode);




             int* __cdecl _errno(void);


             errno_t __cdecl _set_errno( int _Value);
             errno_t __cdecl _get_errno( int* _Value);

             unsigned long* __cdecl __doserrno(void);


             errno_t __cdecl _set_doserrno( unsigned long _Value);
             errno_t __cdecl _get_doserrno( unsigned long * _Value);


             __declspec(deprecated("This function or variable may be unsafe. Consider using " "strerror" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) char** __cdecl __sys_errlist(void);


             __declspec(deprecated("This function or variable may be unsafe. Consider using " "strerror" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) int * __cdecl __sys_nerr(void);


             void __cdecl perror( char const* _ErrMsg);





__declspec(deprecated("This function or variable may be unsafe. Consider using " "_get_pgmptr" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) char** __cdecl __p__pgmptr (void);
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_get_wpgmptr" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) wchar_t** __cdecl __p__wpgmptr(void);
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_get_fmode" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) int* __cdecl __p__fmode (void);
# 255 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
         errno_t __cdecl _get_pgmptr ( char** _Value);


         errno_t __cdecl _get_wpgmptr( wchar_t** _Value);

         errno_t __cdecl _set_fmode ( int _Mode );

         errno_t __cdecl _get_fmode ( int* _PMode);
# 271 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
typedef struct _div_t
{
    int quot;
    int rem;
} div_t;

typedef struct _ldiv_t
{
    long quot;
    long rem;
} ldiv_t;

typedef struct _lldiv_t
{
    long long quot;
    long long rem;
} lldiv_t;

               int __cdecl abs ( int _Number);
               long __cdecl labs ( long _Number);
               long long __cdecl llabs ( long long _Number);
               __int64 __cdecl _abs64( __int64 _Number);

               unsigned short __cdecl _byteswap_ushort( unsigned short _Number);
               unsigned long __cdecl _byteswap_ulong ( unsigned long _Number);
               unsigned __int64 __cdecl _byteswap_uint64( unsigned __int64 _Number);

                        div_t __cdecl div ( int _Numerator, int _Denominator);
                        ldiv_t __cdecl ldiv ( long _Numerator, long _Denominator);
                        lldiv_t __cdecl lldiv( long long _Numerator, long long _Denominator);



#pragma warning(push)
#pragma warning(disable: 6540)

unsigned int __cdecl _rotl(
         unsigned int _Value,
         int _Shift
    );


unsigned long __cdecl _lrotl(
         unsigned long _Value,
         int _Shift
    );

unsigned __int64 __cdecl _rotl64(
         unsigned __int64 _Value,
         int _Shift
    );

unsigned int __cdecl _rotr(
         unsigned int _Value,
         int _Shift
    );


unsigned long __cdecl _lrotr(
         unsigned long _Value,
         int _Shift
    );

unsigned __int64 __cdecl _rotr64(
         unsigned __int64 _Value,
         int _Shift
    );

#pragma warning(pop)






 void __cdecl srand( unsigned int _Seed);

                        int __cdecl rand(void);
# 357 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
extern "C++"
{
    inline long abs(long const _X) throw()
    {
        return labs(_X);
    }

    inline long long abs(long long const _X) throw()
    {
        return llabs(_X);
    }

    inline ldiv_t div(long const _A1, long const _A2) throw()
    {
        return ldiv(_A1, _A2);
    }

    inline lldiv_t div(long long const _A1, long long const _A2) throw()
    {
        return lldiv(_A1, _A2);
    }
}
# 390 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
#pragma pack(push, 4)
 typedef struct
    {
        unsigned char ld[10];
    } _LDOUBLE;
#pragma pack(pop)
# 411 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
typedef struct
{
    double x;
} _CRT_DOUBLE;

typedef struct
{
    float f;
} _CRT_FLOAT;





typedef struct
{
    long double x;
} _LONGDOUBLE;



#pragma pack(push, 4)
typedef struct
{
    unsigned char ld12[12];
} _LDBL12;
#pragma pack(pop)








 double __cdecl atof ( char const* _String);
                                           int __cdecl atoi ( char const* _String);
                                           long __cdecl atol ( char const* _String);
                                           long long __cdecl atoll ( char const* _String);
                                           __int64 __cdecl _atoi64( char const* _String);

                        double __cdecl _atof_l ( char const* _String, _locale_t _Locale);
                        int __cdecl _atoi_l ( char const* _String, _locale_t _Locale);
                        long __cdecl _atol_l ( char const* _String, _locale_t _Locale);
                        long long __cdecl _atoll_l ( char const* _String, _locale_t _Locale);
                        __int64 __cdecl _atoi64_l( char const* _String, _locale_t _Locale);

                        int __cdecl _atoflt ( _CRT_FLOAT* _Result, char const* _String);
                        int __cdecl _atodbl ( _CRT_DOUBLE* _Result, char* _String);
                        int __cdecl _atoldbl( _LDOUBLE* _Result, char* _String);


         int __cdecl _atoflt_l(
             _CRT_FLOAT* _Result,
             char const* _String,
             _locale_t _Locale
    );


         int __cdecl _atodbl_l(
             _CRT_DOUBLE* _Result,
             char* _String,
             _locale_t _Locale
    );



         int __cdecl _atoldbl_l(
             _LDOUBLE* _Result,
             char* _String,
             _locale_t _Locale
    );


         float __cdecl strtof(
                             char const* _String,
                             char** _EndPtr
    );


         float __cdecl _strtof_l(
                             char const* _String,
                             char** _EndPtr,
                             _locale_t _Locale
    );


         double __cdecl strtod(
                             char const* _String,
                             char** _EndPtr
    );


         double __cdecl _strtod_l(
                             char const* _String,
                             char** _EndPtr,
                             _locale_t _Locale
    );


         long double __cdecl strtold(
                             char const* _String,
                             char** _EndPtr
    );


         long double __cdecl _strtold_l(
                             char const* _String,
                             char** _EndPtr,
                             _locale_t _Locale
    );


         long __cdecl strtol(
                             char const* _String,
                             char** _EndPtr,
                             int _Radix
    );


         long __cdecl _strtol_l(
                             char const* _String,
                             char** _EndPtr,
                             int _Radix,
                             _locale_t _Locale
    );


         long long __cdecl strtoll(
                             char const* _String,
                             char** _EndPtr,
                             int _Radix
    );


         long long __cdecl _strtoll_l(
                             char const* _String,
                             char** _EndPtr,
                             int _Radix,
                             _locale_t _Locale
    );


         unsigned long __cdecl strtoul(
                             char const* _String,
                             char** _EndPtr,
                             int _Radix
    );


         unsigned long __cdecl _strtoul_l(
                             char const* _String,
                             char** _EndPtr,
                             int _Radix,
                             _locale_t _Locale
    );


         unsigned long long __cdecl strtoull(
                             char const* _String,
                             char** _EndPtr,
                             int _Radix
    );


         unsigned long long __cdecl _strtoull_l(
                             char const* _String,
                             char** _EndPtr,
                             int _Radix,
                             _locale_t _Locale
    );


         __int64 __cdecl _strtoi64(
                             char const* _String,
                             char** _EndPtr,
                             int _Radix
    );


         __int64 __cdecl _strtoi64_l(
                             char const* _String,
                             char** _EndPtr,
                             int _Radix,
                             _locale_t _Locale
    );


         unsigned __int64 __cdecl _strtoui64(
                             char const* _String,
                             char** _EndPtr,
                             int _Radix
    );


         unsigned __int64 __cdecl _strtoui64_l(
                             char const* _String,
                             char** _EndPtr,
                             int _Radix,
                             _locale_t _Locale
    );
# 622 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
         errno_t __cdecl _itoa_s(
                                 int _Value,
                                 char* _Buffer,
                                 size_t _BufferCount,
                                 int _Radix
    );

extern "C++" { template <size_t _Size> inline errno_t __cdecl _itoa_s(int _Value, char (&_Buffer)[_Size], int _Radix) throw() { return _itoa_s(_Value, _Buffer, _Size, _Radix); } }







#pragma warning(push)
#pragma warning(disable: 28719)
#pragma warning(disable: 28726)
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_itoa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) char* __cdecl _itoa(int _Value, char *_Buffer, int _Radix);





#pragma warning(pop)



 errno_t __cdecl _ltoa_s(
                                 long _Value,
                                 char* _Buffer,
                                 size_t _BufferCount,
                                 int _Radix
    );

extern "C++" { template <size_t _Size> inline errno_t __cdecl _ltoa_s(long _Value, char (&_Buffer)[_Size], int _Radix) throw() { return _ltoa_s(_Value, _Buffer, _Size, _Radix); } }






__declspec(deprecated("This function or variable may be unsafe. Consider using " "_ltoa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) char* __cdecl _ltoa(long _Value, char *_Buffer, int _Radix);
# 673 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
         errno_t __cdecl _ultoa_s(
                                 unsigned long _Value,
                                 char* _Buffer,
                                 size_t _BufferCount,
                                 int _Radix
    );

extern "C++" { template <size_t _Size> inline errno_t __cdecl _ultoa_s(unsigned long _Value, char (&_Buffer)[_Size], int _Radix) throw() { return _ultoa_s(_Value, _Buffer, _Size, _Radix); } }






#pragma warning(push)
#pragma warning(disable: 28726)
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_ultoa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) char* __cdecl _ultoa(unsigned long _Value, char *_Buffer, int _Radix);





#pragma warning(pop)



 errno_t __cdecl _i64toa_s(
                                 __int64 _Value,
                                 char* _Buffer,
                                 size_t _BufferCount,
                                 int _Radix
    );


__declspec(deprecated("This function or variable may be unsafe. Consider using " "_i64toa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
         char* __cdecl _i64toa(
                           __int64 _Value,
                           char* _Buffer,
                           int _Radix
    );



         errno_t __cdecl _ui64toa_s(
                                 unsigned __int64 _Value,
                                 char* _Buffer,
                                 size_t _BufferCount,
                                 int _Radix
    );

__declspec(deprecated("This function or variable may be unsafe. Consider using " "_ui64toa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
         char* __cdecl _ui64toa(
                           unsigned __int64 _Value,
                           char* _Buffer,
                           int _Radix
    );
# 744 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
         errno_t __cdecl _ecvt_s(
                                 char* _Buffer,
          size_t _BufferCount,
          double _Value,
          int _DigitCount,
          int* _PtDec,
          int* _PtSign
    );

extern "C++" { template <size_t _Size> inline errno_t __cdecl _ecvt_s(char (&_Buffer)[_Size], double _Value, int _DigitCount, int* _PtDec, int* _PtSign) throw() { return _ecvt_s(_Buffer, _Size, _Value, _DigitCount, _PtDec, _PtSign); } }
# 762 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
               __declspec(deprecated("This function or variable may be unsafe. Consider using " "_ecvt_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
         char* __cdecl _ecvt(
          double _Value,
          int _DigitCount,
          int* _PtDec,
          int* _PtSign
    );



         errno_t __cdecl _fcvt_s(
                                 char* _Buffer,
                                 size_t _BufferCount,
                                 double _Value,
                                 int _FractionalDigitCount,
                                 int* _PtDec,
                                 int* _PtSign
    );

extern "C++" { template <size_t _Size> inline errno_t __cdecl _fcvt_s(char (&_Buffer)[_Size], double _Value, int _FractionalDigitCount, int* _PtDec, int* _PtSign) throw() { return _fcvt_s(_Buffer, _Size, _Value, _FractionalDigitCount, _PtDec, _PtSign); } }
# 792 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
               __declspec(deprecated("This function or variable may be unsafe. Consider using " "_fcvt_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
         char* __cdecl _fcvt(
          double _Value,
          int _FractionalDigitCount,
          int* _PtDec,
          int* _PtSign
    );


         errno_t __cdecl _gcvt_s(
                                 char* _Buffer,
                                 size_t _BufferCount,
                                 double _Value,
                                 int _DigitCount
    );

extern "C++" { template <size_t _Size> inline errno_t __cdecl _gcvt_s(char (&_Buffer)[_Size], double _Value, int _DigitCount) throw() { return _gcvt_s(_Buffer, _Size, _Value, _DigitCount); } }







__declspec(deprecated("This function or variable may be unsafe. Consider using " "_gcvt_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
         char* __cdecl _gcvt(
                           double _Value,
                           int _DigitCount,
                           char* _Buffer
    );
# 846 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
             int __cdecl ___mb_cur_max_func(void);


             int __cdecl ___mb_cur_max_l_func(_locale_t _Locale);





         int __cdecl mblen(
                                                char const* _Ch,
                                                size_t _MaxCount
    );


         int __cdecl _mblen_l(
                                                char const* _Ch,
                                                size_t _MaxCount,
                                                _locale_t _Locale
    );



         size_t __cdecl _mbstrlen(
           char const* _String
    );



         size_t __cdecl _mbstrlen_l(
             char const* _String,
             _locale_t _Locale
    );



         size_t __cdecl _mbstrnlen(
           char const* _String,
           size_t _MaxCount
    );



         size_t __cdecl _mbstrnlen_l(
             char const* _String,
             size_t _MaxCount,
             _locale_t _Locale
    );


         int __cdecl mbtowc(
                                         wchar_t* _DstCh,
                                         char const* _SrcCh,
                                         size_t _SrcSizeInBytes
    );


         int __cdecl _mbtowc_l(
                                         wchar_t* _DstCh,
                                         char const* _SrcCh,
                                         size_t _SrcSizeInBytes,
                                         _locale_t _Locale
    );


         errno_t __cdecl mbstowcs_s(
                                                              size_t* _PtNumOfCharConverted,
                                                              wchar_t* _DstBuf,
                                                              size_t _SizeInWords,
                                                              char const* _SrcBuf,
                                                              size_t _MaxCount
    );

extern "C++" { template <size_t _Size> inline errno_t __cdecl mbstowcs_s(size_t* _PtNumOfCharConverted, wchar_t (&_Dest)[_Size], char const* _Source, size_t _MaxCount) throw() { return mbstowcs_s(_PtNumOfCharConverted, _Dest, _Size, _Source, _MaxCount); } }







__declspec(deprecated("This function or variable may be unsafe. Consider using " "mbstowcs_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) size_t __cdecl mbstowcs( wchar_t *_Dest, char const* _Source, size_t _MaxCount);







         errno_t __cdecl _mbstowcs_s_l(
                                                              size_t* _PtNumOfCharConverted,
                                                              wchar_t* _DstBuf,
                                                              size_t _SizeInWords,
                                                              char const* _SrcBuf,
                                                              size_t _MaxCount,
                                                              _locale_t _Locale
    );

extern "C++" { template <size_t _Size> inline errno_t __cdecl _mbstowcs_s_l(size_t* _PtNumOfCharConverted, wchar_t (&_Dest)[_Size], char const* _Source, size_t _MaxCount, _locale_t _Locale) throw() { return _mbstowcs_s_l(_PtNumOfCharConverted, _Dest, _Size, _Source, _MaxCount, _Locale); } }
# 953 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_mbstowcs_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) size_t __cdecl _mbstowcs_l( wchar_t *_Dest, char const* _Source, size_t _MaxCount, _locale_t _Locale);
# 965 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
__declspec(deprecated("This function or variable may be unsafe. Consider using " "wctomb_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
         int __cdecl wctomb(
                                   char* _MbCh,
                                   wchar_t _WCh
    );

__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wctomb_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
         int __cdecl _wctomb_l(
                             char* _MbCh,
                             wchar_t _WCh,
                             _locale_t _Locale
    );




             errno_t __cdecl wctomb_s(
                                                                 int* _SizeConverted,
                                                                 char* _MbCh,
                                                                 rsize_t _SizeInBytes,
                                                                 wchar_t _WCh
        );




         errno_t __cdecl _wctomb_s_l(
                                     int* _SizeConverted,
                                     char* _MbCh,
                                     size_t _SizeInBytes,
                                     wchar_t _WCh,
                                     _locale_t _Locale);


         errno_t __cdecl wcstombs_s(
                                                                       size_t* _PtNumOfCharConverted,
                                                                       char* _Dst,
                                                                       size_t _DstSizeInBytes,
                                                                       wchar_t const* _Src,
                                                                       size_t _MaxCountInBytes
    );

extern "C++" { template <size_t _Size> inline errno_t __cdecl wcstombs_s(size_t* _PtNumOfCharConverted, char (&_Dest)[_Size], wchar_t const* _Source, size_t _MaxCount) throw() { return wcstombs_s(_PtNumOfCharConverted, _Dest, _Size, _Source, _MaxCount); } }







__declspec(deprecated("This function or variable may be unsafe. Consider using " "wcstombs_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) size_t __cdecl wcstombs( char *_Dest, wchar_t const* _Source, size_t _MaxCount);







         errno_t __cdecl _wcstombs_s_l(
                                                                       size_t* _PtNumOfCharConverted,
                                                                       char* _Dst,
                                                                       size_t _DstSizeInBytes,
                                                                       wchar_t const* _Src,
                                                                       size_t _MaxCountInBytes,
                                                                       _locale_t _Locale
    );

extern "C++" { template <size_t _Size> inline errno_t __cdecl _wcstombs_s_l(size_t* _PtNumOfCharConverted, char (&_Dest)[_Size], wchar_t const* _Source, size_t _MaxCount, _locale_t _Locale) throw() { return _wcstombs_s_l(_PtNumOfCharConverted, _Dest, _Size, _Source, _MaxCount, _Locale); } }
# 1041 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_wcstombs_s_l" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) size_t __cdecl _wcstombs_l( char *_Dest, wchar_t const* _Source, size_t _MaxCount, _locale_t _Locale);
# 1071 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
         __declspec(allocator) char* __cdecl _fullpath(
                                     char* _Buffer,
                                     char const* _Path,
                                     size_t _BufferCount
    );




         errno_t __cdecl _makepath_s(
                                 char* _Buffer,
                                 size_t _BufferCount,
                                 char const* _Drive,
                                 char const* _Dir,
                                 char const* _Filename,
                                 char const* _Ext
    );

extern "C++" { template <size_t _Size> inline errno_t __cdecl _makepath_s(char (&_Buffer)[_Size], char const* _Drive, char const* _Dir, char const* _Filename, char const* _Ext) throw() { return _makepath_s(_Buffer, _Size, _Drive, _Dir, _Filename, _Ext); } }








#pragma warning(push)
#pragma warning(disable: 28719)
#pragma warning(disable: 28726)
__declspec(deprecated("This function or variable may be unsafe. Consider using " "_makepath_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) void __cdecl _makepath( char *_Buffer, char const* _Drive, char const* _Dir, char const* _Filename, char const* _Ext);







#pragma warning(pop)

__declspec(deprecated("This function or variable may be unsafe. Consider using " "_splitpath_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
         void __cdecl _splitpath(
                             char const* _FullPath,
                             char* _Drive,
                             char* _Dir,
                             char* _Filename,
                             char* _Ext
    );


         errno_t __cdecl _splitpath_s(
                                       char const* _FullPath,
                                       char* _Drive,
                                       size_t _DriveCount,
                                       char* _Dir,
                                       size_t _DirCount,
                                       char* _Filename,
                                       size_t _FilenameCount,
                                       char* _Ext,
                                       size_t _ExtCount
    );

extern "C++" { template <size_t _DriveSize, size_t _DirSize, size_t _NameSize, size_t _ExtSize> inline errno_t __cdecl _splitpath_s( char const* _Dest, char (&_Drive)[_DriveSize], char (&_Dir)[_DirSize], char (&_Name)[_NameSize], char (&_Ext)[_ExtSize] ) throw() { return _splitpath_s(_Dest, _Drive, _DriveSize, _Dir, _DirSize, _Name, _NameSize, _Ext, _ExtSize); } }





         errno_t __cdecl getenv_s(
                                     size_t* _RequiredCount,
                                     char* _Buffer,
                                     rsize_t _BufferCount,
                                     char const* _VarName
    );






         int* __cdecl __p___argc (void);
         char*** __cdecl __p___argv (void);
         wchar_t*** __cdecl __p___wargv(void);
# 1165 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
         char*** __cdecl __p__environ (void);
         wchar_t*** __cdecl __p__wenviron(void);
# 1190 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
                   __declspec(deprecated("This function or variable may be unsafe. Consider using " "_dupenv_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
             char* __cdecl getenv(
               char const* _VarName
        );

    extern "C++" { template <size_t _Size> inline errno_t __cdecl getenv_s(size_t* _RequiredCount, char (&_Buffer)[_Size], char const* _VarName) throw() { return getenv_s(_RequiredCount, _Buffer, _Size, _VarName); } }
# 1208 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
             errno_t __cdecl _dupenv_s(
                                                                                    char** _Buffer,
                                                                                    size_t* _BufferCount,
                                                                                    char const* _VarName
        );





             int __cdecl system(
                   char const* _Command
        );



#pragma warning(push)
#pragma warning(disable: 6540)


 int __cdecl _putenv(
               char const* _EnvString
        );


             errno_t __cdecl _putenv_s(
               char const* _Name,
               char const* _Value
        );

#pragma warning(pop)

 errno_t __cdecl _searchenv_s(
                                     char const* _Filename,
                                     char const* _VarName,
                                     char* _Buffer,
                                     size_t _BufferCount
        );

    extern "C++" { template <size_t _Size> inline errno_t __cdecl _searchenv_s(char const* _Filename, char const* _VarName, char (&_Buffer)[_Size]) throw() { return _searchenv_s(_Filename, _VarName, _Buffer, _Size); } }






    __declspec(deprecated("This function or variable may be unsafe. Consider using " "_searchenv_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details.")) void __cdecl _searchenv(char const* _Filename, char const* _VarName, char *_Buffer);







    __declspec(deprecated("This function or variable has been superceded by newer library " "or operating system functionality. Consider using " "SetErrorMode" " " "instead. See online help for details."))
             void __cdecl _seterrormode(
             int _Mode
        );

    __declspec(deprecated("This function or variable has been superceded by newer library " "or operating system functionality. Consider using " "Beep" " " "instead. See online help for details."))
             void __cdecl _beep(
             unsigned _Frequency,
             unsigned _Duration
        );

    __declspec(deprecated("This function or variable has been superceded by newer library " "or operating system functionality. Consider using " "Sleep" " " "instead. See online help for details."))
             void __cdecl _sleep(
             unsigned long _Duration
        );
# 1296 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
#pragma warning(push)
#pragma warning(disable: 4141)

 __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_ecvt" ". See online help for details.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "_ecvt_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
             char* __cdecl ecvt(
              double _Value,
              int _DigitCount,
              int* _PtDec,
              int* _PtSign
        );

                   __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_fcvt" ". See online help for details.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "_fcvt_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
             char* __cdecl fcvt(
              double _Value,
              int _FractionalDigitCount,
              int* _PtDec,
              int* _PtSign
        );

    __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_gcvt" ". See online help for details.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "_fcvt_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
             char* __cdecl gcvt(
                               double _Value,
                               int _DigitCount,
                               char* _DstBuf
        );

    __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_itoa" ". See online help for details.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "_itoa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
             char* __cdecl itoa(
                               int _Value,
                               char* _Buffer,
                               int _Radix
        );

    __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_ltoa" ". See online help for details.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "_ltoa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
             char* __cdecl ltoa(
                               long _Value,
                               char* _Buffer,
                               int _Radix
        );


    __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_swab" ". See online help for details."))
             void __cdecl swab(
                                        char* _Buf1,
                                        char* _Buf2,
                                        int _SizeInBytes
        );

    __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_ultoa" ". See online help for details.")) __declspec(deprecated("This function or variable may be unsafe. Consider using " "_ultoa_s" " instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. " "See online help for details."))
             char* __cdecl ultoa(
                               unsigned long _Value,
                               char* _Buffer,
                               int _Radix
        );



                   __declspec(deprecated("The POSIX name for this item is deprecated. Instead, use the ISO C " "and C++ conformant name: " "_putenv" ". See online help for details."))
             int __cdecl putenv(
               char const* _EnvString
        );

#pragma warning(pop)

 _onexit_t __cdecl onexit( _onexit_t _Func);





}
# 1366 "C:\\Program Files (x86)\\Windows Kits\\10\\include\\10.0.17763.0\\ucrt\\stdlib.h" 3
#pragma pack(pop)
# 14 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\cstdlib" 2 3



namespace std {
using :: size_t; using :: div_t; using :: ldiv_t;
using :: abort; using :: abs; using :: atexit;
using :: atof; using :: atoi; using :: atol;
using :: bsearch; using :: calloc; using :: div;
using :: exit; using :: free;
using :: labs; using :: ldiv; using :: malloc;
using :: mblen; using :: mbstowcs; using :: mbtowc;
using :: qsort; using :: rand; using :: realloc;
using :: srand; using :: strtod; using :: strtol;
using :: strtoul;
using :: wcstombs; using :: wctomb;

using :: lldiv_t;


using :: getenv;
using :: system;


using :: atoll; using :: llabs; using :: lldiv;
using :: strtof; using :: strtold;
using :: strtoll; using :: strtoull;

using :: _Exit; using :: at_quick_exit; using :: quick_exit;
}
# 7 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\xtr1common" 2 3

#pragma pack(push,8)
#pragma warning(push, 3)



namespace std {

struct _Nil
 {
 };


template<class _Ty,
 _Ty _Val>
 struct integral_constant
 {
 static constexpr _Ty value = _Val;

 typedef _Ty value_type;
 typedef integral_constant<_Ty, _Val> type;

 constexpr operator value_type() const noexcept
  {
  return (value);
  }

 constexpr value_type operator()() const noexcept
  {
  return (value);
  }
 };

typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;


template<bool _Val>
 using bool_constant = integral_constant<bool, _Val>;


template<bool _Val>
 struct _Cat_base
  : integral_constant<bool, _Val>
 {
 };


template<bool _Test,
 class _Ty = void>
 struct enable_if
 {
 };

template<class _Ty>
 struct enable_if<true, _Ty>
 {
 typedef _Ty type;
 };


template<bool _Test,
 class _Ty1,
 class _Ty2>
 struct conditional
 {
 typedef _Ty2 type;
 };

template<class _Ty1,
 class _Ty2>
 struct conditional<true, _Ty1, _Ty2>
 {
 typedef _Ty1 type;
 };


template<class _Ty1,
 class _Ty2>
 struct is_same
  : false_type
 {
 };

template<class _Ty1>
 struct is_same<_Ty1, _Ty1>
  : true_type
 {
 };


template<class _Ty,
 class _Uty>
 constexpr bool is_same_v = is_same<_Ty, _Uty>::value;



template<class _Ty>
 struct remove_const
 {
 typedef _Ty type;
 };

template<class _Ty>
 struct remove_const<const _Ty>
 {
 typedef _Ty type;
 };


template<class _Ty>
 struct remove_volatile
 {
 typedef _Ty type;
 };

template<class _Ty>
 struct remove_volatile<volatile _Ty>
 {
 typedef _Ty type;
 };


template<class _Ty>
 struct remove_cv
 {
 typedef typename remove_const<typename remove_volatile<_Ty>::type>::type
  type;
 };


template<class _Ty>
 struct _Is_integral
  : false_type
 {
 };

template<>
 struct _Is_integral<bool>
  : true_type
 {
 };

template<>
 struct _Is_integral<char>
  : true_type
 {
 };

template<>
 struct _Is_integral<unsigned char>
  : true_type
 {
 };

template<>
 struct _Is_integral<signed char>
  : true_type
 {
 };


template<>
 struct _Is_integral<wchar_t>
  : true_type
 {
 };


template<>
 struct _Is_integral<unsigned short>
  : true_type
 {
 };

template<>
 struct _Is_integral<signed short>
  : true_type
 {
 };

template<>
 struct _Is_integral<unsigned int>
  : true_type
 {
 };

template<>
 struct _Is_integral<signed int>
  : true_type
 {
 };

template<>
 struct _Is_integral<unsigned long>
  : true_type
 {
 };

template<>
 struct _Is_integral<signed long>
  : true_type
 {
 };

template<>
 struct _Is_integral<char16_t>
  : true_type
 {
 };

template<>
 struct _Is_integral<char32_t>
  : true_type
 {
 };

template<>
 struct _Is_integral<long long>
  : true_type
 {
 };

template<>
 struct _Is_integral<unsigned long long>
  : true_type
 {
 };


template<class _Ty>
 struct is_integral
  : _Is_integral<typename remove_cv<_Ty>::type>
 {
 };


template<class _Ty>
 constexpr bool is_integral_v = is_integral<_Ty>::value;



template<class _Ty>
 struct _Is_floating_point
  : false_type
 {
 };

template<>
 struct _Is_floating_point<float>
  : true_type
 {
 };

template<>
 struct _Is_floating_point<double>
  : true_type
 {
 };

template<>
 struct _Is_floating_point<long double>
  : true_type
 {
 };


template<class _Ty>
 struct is_floating_point
  : _Is_floating_point<typename remove_cv<_Ty>::type>
 {
 };


template<class _Ty>
 constexpr bool is_floating_point_v = is_floating_point<_Ty>::value;



template<class _Ty>
 struct is_arithmetic
  : _Cat_base<is_integral<_Ty>::value
   || is_floating_point<_Ty>::value>
 {
 };


template<class _Ty>
 constexpr bool is_arithmetic_v = is_arithmetic<_Ty>::value;



template<class _Ty>
 struct remove_reference
 {
 typedef _Ty type;
 };

template<class _Ty>
 struct remove_reference<_Ty&>
 {
 typedef _Ty type;
 };

template<class _Ty>
 struct remove_reference<_Ty&&>
 {
 typedef _Ty type;
 };


struct _Wrap_int
 {
 _Wrap_int(int)
  {
  }
 };

template<class _Ty>
 struct _Identity
 {
 typedef _Ty type;
 };
# 344 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\xtr1common" 3
}

#pragma warning(pop)
#pragma pack(pop)
# 9 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\xtgmath.h" 2 3

#pragma pack(push,8)
#pragma warning(push, 3)



namespace std {
template<class _Ty>
 struct _Promote_to_float
 {
 typedef typename conditional<is_integral<_Ty>::value,
  double, _Ty>::type type;
 };

template<class _Ty1,
 class _Ty2>
 struct _Common_float_type
 {
 typedef typename _Promote_to_float<_Ty1>::type _Ty1f;
 typedef typename _Promote_to_float<_Ty2>::type _Ty2f;
 typedef typename conditional<is_same<_Ty1f, long double>::value
  || is_same<_Ty2f, long double>::value, long double,
  typename conditional<is_same<_Ty1f, double>::value
   || is_same<_Ty2f, double>::value, double,
   float>::type>::type type;
 };
}
# 76 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\xtgmath.h" 3
template<class _Ty1,
 class _Ty2> inline
 typename ::std:: enable_if< ::std:: is_arithmetic<_Ty1>::value
  && ::std:: is_arithmetic<_Ty2>::value,
  typename ::std:: _Common_float_type<_Ty1, _Ty2>::type>::type
 pow(const _Ty1 _Left, const _Ty2 _Right)
 {
 typedef typename ::std:: _Common_float_type<_Ty1, _Ty2>::type type;
 return (:: pow(type(_Left), type(_Right)));
 }


extern "C" double __cdecl acos( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type acos(_Ty _Left) { return (:: acos((double)_Left)); }
extern "C" double __cdecl asin( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type asin(_Ty _Left) { return (:: asin((double)_Left)); }
extern "C" double __cdecl atan( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type atan(_Ty _Left) { return (:: atan((double)_Left)); }
extern "C" double __cdecl atan2( double, double); template<class _Ty1, class _Ty2> inline typename ::std:: enable_if< ::std:: is_arithmetic<_Ty1>::value && ::std:: is_arithmetic<_Ty2>::value, typename ::std:: _Common_float_type<_Ty1, _Ty2>::type>::type atan2(_Ty1 _Left, _Ty2 _Right) { typedef typename ::std:: _Common_float_type<_Ty1, _Ty2>::type type; return (:: atan2((type)_Left, (type)_Right)); }
extern "C" double __cdecl ceil( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type ceil(_Ty _Left) { return (:: ceil((double)_Left)); }
extern "C" double __cdecl cos( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type cos(_Ty _Left) { return (:: cos((double)_Left)); }
extern "C" double __cdecl cosh( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type cosh(_Ty _Left) { return (:: cosh((double)_Left)); }
extern "C" double __cdecl exp( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type exp(_Ty _Left) { return (:: exp((double)_Left)); }

extern "C" double __cdecl fabs( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type fabs(_Ty _Left) { return (:: fabs((double)_Left)); }

extern "C" double __cdecl floor( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type floor(_Ty _Left) { return (:: floor((double)_Left)); }
extern "C" double __cdecl fmod( double, double); template<class _Ty1, class _Ty2> inline typename ::std:: enable_if< ::std:: is_arithmetic<_Ty1>::value && ::std:: is_arithmetic<_Ty2>::value, typename ::std:: _Common_float_type<_Ty1, _Ty2>::type>::type fmod(_Ty1 _Left, _Ty2 _Right) { typedef typename ::std:: _Common_float_type<_Ty1, _Ty2>::type type; return (:: fmod((type)_Left, (type)_Right)); }
extern "C" double __cdecl frexp( double, int *); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type frexp(_Ty _Left, int * _Arg2) { return (:: frexp((double)_Left, _Arg2)); }
extern "C" double __cdecl ldexp( double, int); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type ldexp(_Ty _Left, int _Arg2) { return (:: ldexp((double)_Left, _Arg2)); }
extern "C" double __cdecl log( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type log(_Ty _Left) { return (:: log((double)_Left)); }
extern "C" double __cdecl log10( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type log10(_Ty _Left) { return (:: log10((double)_Left)); }


extern "C" double __cdecl sin( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type sin(_Ty _Left) { return (:: sin((double)_Left)); }
extern "C" double __cdecl sinh( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type sinh(_Ty _Left) { return (:: sinh((double)_Left)); }
extern "C" double __cdecl sqrt( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type sqrt(_Ty _Left) { return (:: sqrt((double)_Left)); }
extern "C" double __cdecl tan( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type tan(_Ty _Left) { return (:: tan((double)_Left)); }
extern "C" double __cdecl tanh( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type tanh(_Ty _Left) { return (:: tanh((double)_Left)); }
# 125 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\xtgmath.h" 3
inline float _Fma(float _Left, float _Middle, float _Right)
 {
 return (:: fmaf(_Left, _Middle, _Right));
 }

inline double _Fma(double _Left, double _Middle, double _Right)
 {
 return (:: fma(_Left, _Middle, _Right));
 }

inline long double _Fma(long double _Left, long double _Middle,
 long double _Right)
 {
 return (:: fmal(_Left, _Middle, _Right));
 }

template<class _Ty1,
 class _Ty2,
 class _Ty3> inline
 typename ::std:: _Common_float_type<_Ty1,
  typename ::std:: _Common_float_type<_Ty2, _Ty3>::type>::type
 fma(_Ty1 _Left, _Ty2 _Middle, _Ty3 _Right)
 {
 typedef typename ::std:: _Common_float_type<_Ty1,
  typename ::std:: _Common_float_type<_Ty2, _Ty3>::type>::type type;
 return (_Fma((type)_Left, (type)_Middle, (type)_Right));
 }



inline float _Remquo(float _Left, float _Right, int *_Pquo)
 {
 return (:: remquof(_Left, _Right, _Pquo));
 }

inline double _Remquo(double _Left, double _Right, int *_Pquo)
 {
 return (:: remquo(_Left, _Right, _Pquo));
 }

inline long double _Remquo(long double _Left, long double _Right, int *_Pquo)
 {
 return (:: remquol(_Left, _Right, _Pquo));
 }

template<class _Ty1,
 class _Ty2> inline
 typename ::std:: _Common_float_type<_Ty1, _Ty2>::type
 remquo(_Ty1 _Left, _Ty2 _Right, int *_Pquo)
 {
 typedef typename ::std:: _Common_float_type<_Ty1, _Ty2>::type type;
 return (_Remquo((type)_Left, (type)_Right, _Pquo));
 }

extern "C" double __cdecl acosh( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type acosh(_Ty _Left) { return (:: acosh((double)_Left)); }
extern "C" double __cdecl asinh( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type asinh(_Ty _Left) { return (:: asinh((double)_Left)); }
extern "C" double __cdecl atanh( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type atanh(_Ty _Left) { return (:: atanh((double)_Left)); }
extern "C" double __cdecl cbrt( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type cbrt(_Ty _Left) { return (:: cbrt((double)_Left)); }
extern "C" double __cdecl copysign( double, double); template<class _Ty1, class _Ty2> inline typename ::std:: enable_if< ::std:: is_arithmetic<_Ty1>::value && ::std:: is_arithmetic<_Ty2>::value, typename ::std:: _Common_float_type<_Ty1, _Ty2>::type>::type copysign(_Ty1 _Left, _Ty2 _Right) { typedef typename ::std:: _Common_float_type<_Ty1, _Ty2>::type type; return (:: copysign((type)_Left, (type)_Right)); }
extern "C" double __cdecl erf( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type erf(_Ty _Left) { return (:: erf((double)_Left)); }
extern "C" double __cdecl erfc( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type erfc(_Ty _Left) { return (:: erfc((double)_Left)); }
extern "C" double __cdecl expm1( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type expm1(_Ty _Left) { return (:: expm1((double)_Left)); }
extern "C" double __cdecl exp2( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type exp2(_Ty _Left) { return (:: exp2((double)_Left)); }
extern "C" double __cdecl fdim( double, double); template<class _Ty1, class _Ty2> inline typename ::std:: enable_if< ::std:: is_arithmetic<_Ty1>::value && ::std:: is_arithmetic<_Ty2>::value, typename ::std:: _Common_float_type<_Ty1, _Ty2>::type>::type fdim(_Ty1 _Left, _Ty2 _Right) { typedef typename ::std:: _Common_float_type<_Ty1, _Ty2>::type type; return (:: fdim((type)_Left, (type)_Right)); }

extern "C" double __cdecl fmax( double, double); template<class _Ty1, class _Ty2> inline typename ::std:: enable_if< ::std:: is_arithmetic<_Ty1>::value && ::std:: is_arithmetic<_Ty2>::value, typename ::std:: _Common_float_type<_Ty1, _Ty2>::type>::type fmax(_Ty1 _Left, _Ty2 _Right) { typedef typename ::std:: _Common_float_type<_Ty1, _Ty2>::type type; return (:: fmax((type)_Left, (type)_Right)); }
extern "C" double __cdecl fmin( double, double); template<class _Ty1, class _Ty2> inline typename ::std:: enable_if< ::std:: is_arithmetic<_Ty1>::value && ::std:: is_arithmetic<_Ty2>::value, typename ::std:: _Common_float_type<_Ty1, _Ty2>::type>::type fmin(_Ty1 _Left, _Ty2 _Right) { typedef typename ::std:: _Common_float_type<_Ty1, _Ty2>::type type; return (:: fmin((type)_Left, (type)_Right)); }
extern "C" double __cdecl hypot( double, double); template<class _Ty1, class _Ty2> inline typename ::std:: enable_if< ::std:: is_arithmetic<_Ty1>::value && ::std:: is_arithmetic<_Ty2>::value, typename ::std:: _Common_float_type<_Ty1, _Ty2>::type>::type hypot(_Ty1 _Left, _Ty2 _Right) { typedef typename ::std:: _Common_float_type<_Ty1, _Ty2>::type type; return (:: hypot((type)_Left, (type)_Right)); }
extern "C" int __cdecl ilogb( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, int>::type ilogb(_Ty _Left) { return (:: ilogb((double)_Left)); }
extern "C" double __cdecl lgamma( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type lgamma(_Ty _Left) { return (:: lgamma((double)_Left)); }
extern "C" long long __cdecl llrint( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, long long>::type llrint(_Ty _Left) { return (:: llrint((double)_Left)); }
extern "C" long long __cdecl llround( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, long long>::type llround(_Ty _Left) { return (:: llround((double)_Left)); }
extern "C" double __cdecl log1p( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type log1p(_Ty _Left) { return (:: log1p((double)_Left)); }
extern "C" double __cdecl log2( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type log2(_Ty _Left) { return (:: log2((double)_Left)); }
extern "C" double __cdecl logb( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type logb(_Ty _Left) { return (:: logb((double)_Left)); }
extern "C" long __cdecl lrint( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, long>::type lrint(_Ty _Left) { return (:: lrint((double)_Left)); }
extern "C" long __cdecl lround( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, long>::type lround(_Ty _Left) { return (:: lround((double)_Left)); }
extern "C" double __cdecl nearbyint( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type nearbyint(_Ty _Left) { return (:: nearbyint((double)_Left)); }
extern "C" double __cdecl nextafter( double, double); template<class _Ty1, class _Ty2> inline typename ::std:: enable_if< ::std:: is_arithmetic<_Ty1>::value && ::std:: is_arithmetic<_Ty2>::value, typename ::std:: _Common_float_type<_Ty1, _Ty2>::type>::type nextafter(_Ty1 _Left, _Ty2 _Right) { typedef typename ::std:: _Common_float_type<_Ty1, _Ty2>::type type; return (:: nextafter((type)_Left, (type)_Right)); }
extern "C" double __cdecl nexttoward( double, long double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type nexttoward(_Ty _Left, long double _Arg2) { return (:: nexttoward((double)_Left, _Arg2)); }
extern "C" double __cdecl remainder( double, double); template<class _Ty1, class _Ty2> inline typename ::std:: enable_if< ::std:: is_arithmetic<_Ty1>::value && ::std:: is_arithmetic<_Ty2>::value, typename ::std:: _Common_float_type<_Ty1, _Ty2>::type>::type remainder(_Ty1 _Left, _Ty2 _Right) { typedef typename ::std:: _Common_float_type<_Ty1, _Ty2>::type type; return (:: remainder((type)_Left, (type)_Right)); }

extern "C" double __cdecl rint( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type rint(_Ty _Left) { return (:: rint((double)_Left)); }
extern "C" double __cdecl round( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type round(_Ty _Left) { return (:: round((double)_Left)); }
extern "C" double __cdecl scalbln( double, long); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type scalbln(_Ty _Left, long _Arg2) { return (:: scalbln((double)_Left, _Arg2)); }
extern "C" double __cdecl scalbn( double, int); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type scalbn(_Ty _Left, int _Arg2) { return (:: scalbn((double)_Left, _Arg2)); }
extern "C" double __cdecl tgamma( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type tgamma(_Ty _Left) { return (:: tgamma((double)_Left)); }
extern "C" double __cdecl trunc( double); template<class _Ty> inline typename ::std:: enable_if< ::std:: is_integral<_Ty>::value, double>::type trunc(_Ty _Left) { return (:: trunc((double)_Left)); }


#pragma warning(pop)
#pragma pack(pop)
# 648 "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\INCLUDE\\cmath" 2 3



namespace std {
using :: abs; using :: acos; using :: asin;
using :: atan; using :: atan2; using :: ceil;
using :: cos; using :: cosh; using :: exp;
using :: fabs; using :: floor; using :: fmod;
using :: frexp; using :: ldexp; using :: log;
using :: log10; using :: modf; using :: pow;
using :: sin; using :: sinh; using :: sqrt;
using :: tan; using :: tanh;

using :: acosf; using :: asinf;
using :: atanf; using :: atan2f; using :: ceilf;
using :: cosf; using :: coshf; using :: expf;
using :: fabsf; using :: floorf; using :: fmodf;
using :: frexpf; using :: ldexpf; using :: logf;
using :: log10f; using :: modff; using :: powf;
using :: sinf; using :: sinhf; using :: sqrtf;
using :: tanf; using :: tanhf;

using :: acosl; using :: asinl;
using :: atanl; using :: atan2l; using :: ceill;
using :: cosl; using :: coshl; using :: expl;
using :: fabsl; using :: floorl; using :: fmodl;
using :: frexpl; using :: ldexpl; using :: logl;
using :: log10l; using :: modfl; using :: powl;
using :: sinl; using :: sinhl; using :: sqrtl;
using :: tanl; using :: tanhl;

using :: float_t; using :: double_t;

using :: acosh; using :: asinh; using :: atanh;
using :: cbrt; using :: erf; using :: erfc;
using :: expm1; using :: exp2;
using :: hypot; using :: ilogb; using :: lgamma;
using :: log1p; using :: log2; using :: logb;
using :: llrint; using :: lrint; using :: nearbyint;
using :: rint; using :: llround; using :: lround;
using :: fdim; using :: fma; using :: fmax; using :: fmin;
using :: round; using :: trunc;
using :: remainder; using :: remquo;
using :: copysign; using :: nan; using :: nextafter;
using :: scalbn; using :: scalbln;
using :: nexttoward; using :: tgamma;

using :: acoshf; using :: asinhf; using :: atanhf;
using :: cbrtf; using :: erff; using :: erfcf;
using :: expm1f; using :: exp2f;
using :: hypotf; using :: ilogbf; using :: lgammaf;
using :: log1pf; using :: log2f; using :: logbf;
using :: llrintf; using :: lrintf; using :: nearbyintf;
using :: rintf; using :: llroundf; using :: lroundf;
using :: fdimf; using :: fmaf; using :: fmaxf; using :: fminf;
using :: roundf; using :: truncf;
using :: remainderf; using :: remquof;
using :: copysignf; using :: nanf;
using :: nextafterf; using :: scalbnf; using :: scalblnf;
using :: nexttowardf; using :: tgammaf;

using :: acoshl; using :: asinhl; using :: atanhl;
using :: cbrtl; using :: erfl; using :: erfcl;
using :: expm1l; using :: exp2l;
using :: hypotl; using :: ilogbl; using :: lgammal;
using :: log1pl; using :: log2l; using :: logbl;
using :: llrintl; using :: lrintl; using :: nearbyintl;
using :: rintl; using :: llroundl; using :: lroundl;
using :: fdiml; using :: fmal; using :: fmaxl; using :: fminl;
using :: roundl; using :: truncl;
using :: remainderl; using :: remquol;
using :: copysignl; using :: nanl;
using :: nextafterl; using :: scalbnl; using :: scalblnl;
using :: nexttowardl; using :: tgammal;

using :: fpclassify; using :: signbit;
using :: isfinite; using :: isinf;
using :: isnan; using :: isnormal;
using :: isgreater; using :: isgreaterequal;
using :: isless; using :: islessequal;
using :: islessgreater; using :: isunordered;
}
# 5 "./intrinsics.h" 2






inline s32 sign_of(s32 value) {
    s32 result = value >= 0 ? 1 : -1;
    return result;
}

inline f32 sign_of(f32 value) {
    f32 result = value >= 0.0f ? 1.0f : -1.0f;
    return result;
}

inline f32 square_root(f32 f) {
    return sqrtf(f);
}

inline f32 fract(f32 f) {
    f32 ignored;
    return modf(f, &ignored);
}



inline f32 my_abs(f32 f) {
    return fabs(f);
}

inline u32 rotate_left(u32 value, s32 amount) {



    amount &= 31;
    u32 result = ((value << amount) | (value >> (32 - amount)));


    return result;
}

inline u32 rotate_right(u32 value, s32 amount) {



    amount &= 31;
    u32 result = ((value >> amount) | (value << (32 - amount)));

    return result;
}

inline s32 round_f32_to_i32(f32 f) {
    return (s32)roundf(f);
}

inline u32 round_f32_to_u32(f32 f) {
    return (u32)roundf(f);
}

inline s32 floor_f32_to_i32(f32 f) {
    return (s32)floorf(f);
}

inline s32 ceil_f32_to_i32(f32 f) {
    return (s32) ceilf(f);
}

inline u64 round_f64_to_u64(f64 f) {
    return (u64) round(f);
}

inline s32 truncate_f32_to_i32(f32 f) {
    return (s32)f;
}





inline f32 my_sin(f32 a) {
    return sinf(a);
}





inline f32 my_cos(f32 a) {
    return cosf(a);
}





inline f32 my_atan2(f32 y, f32 x) {
    return atan2f(y, x);
}

struct BitScanResult {
    u32 found;
    u32 index;
};

inline BitScanResult find_least_significant_set_bit(u32 value) {
    BitScanResult result = {};




    for (u32 test = 0; test < 32; test++) {
        if (value & (1 << test)) {
            result.index = test;
            result.found = true;
            break;
        }
    }

    return result;
}
# 5 "./math.h" 2
# 16 "./math.h"
inline f32 deg_to_rad(f32 deg) {
    f32 rad = deg * (6.28318530717f/360.0f);
    return rad;
}

inline f32 rad_to_deg(f32 rad) {
    f32 deg = rad * (360.0f/6.28318530717f);
    return deg;
}

inline f32 square(f32 a) {
    return a * a;
}

inline s32 square(s32 a) {
    return a * a;
}

inline u32 square(u32 a) {
    return a * a;
}

inline f32 lerp(f32 a, f32 b, f32 t) {
    return a * (1.0f - t) + b * t;
}


inline f32 max(f32 a, f32 b) {
    return a > b ? a : b;
}


inline f32 min(f32 a, f32 b) {
    return a < b ? a : b;
}


inline f32 clamp(f32 n, f32 lo, f32 hi) {
    return max(lo, min(hi, n));
}



inline f32 map_to_range(f32 t, f32 min, f32 max) {
    f32 result = 0.0f;
    f32 range = max - min;
    if (range != 0.0f) {
        result = (t - min) / range;
    }
    return result;
}

inline f32 clamp01_map_to_range(f32 t, f32 min, f32 max) {
     f32 result = clamp((map_to_range(t, min, max)), 0, 1);
     return result;
}

inline f32 safe_ratio_n(f32 numerator, f32 divisor, f32 n) {
    f32 result = n;

    if (divisor != 0.0f) {
        result = numerator / divisor;
    }

    return result;
}

inline f32 safe_ratio_0(f32 numerator, f32 divisor) {
    f32 result = safe_ratio_n(numerator, divisor, 0.0f);
    return result;
}

inline f32 safe_ratio_1(f32 numerator, f32 divisor) {
    f32 result = safe_ratio_n(numerator, divisor, 1.0f);
    return result;
}

inline f32 neighborhood_distance(f32 a, f32 b, f32 period = 6.28318530717f) {
    f32 d0 = my_abs(a - b);
    f32 d1 = my_abs(a - period*sign_of(a) - b);
    f32 result = min(d0, d1);
    return result;
}





union v2 {
    struct {
        f32 x, y;
    };
    struct {
        f32 u, v;
    };
    f32 e[2];
};

union v3 {
    struct {
        f32 x, y, z;
    };
    struct {
        f32 r, g, b;
    };
    struct {
        f32 u, v, w;
    };
    struct {
        v2 xy;
        f32 ignored0_;
    };
    struct {
        f32 ignored1_;
        v2 yz;
    };
    struct {
        v2 rg;
        f32 ignored2_;
    };
    struct {
        f32 ignored3_;
        v2 gb;
    };
    struct {
        v2 uv;
        f32 ignored4_;
    };
    struct {
        f32 ignored5_;
        v2 vw;
    };
    f32 e[3];
};

union v4 {
    struct {
        f32 x, y, z, w;
    };
    struct {
        f32 r, g, b, a;
    };
    struct {
        v3 xyz;
        f32 ignored0_;
    };
    struct {
        f32 ignored1_;
        v3 yzw;
    };
    struct {
        v2 xy;
        v2 zw;
    };
    struct {
        f32 ignored2_;
        v2 yz;
        f32 ignored3_;
    };
    struct {
        v3 rgb;
        f32 ignored4_;
    };
    struct {
        f32 ignored5_;
        v3 gba;
    };
    struct {
        v2 rg;
        v2 ba;
    };
    struct {
        f32 ignored6_;
        v2 gb;
        f32 ignored7_;
    };
    f32 e[4];
};

struct Rect2 {
    v2 min;
    v2 max;
};

struct Rect3 {
    v3 min;
    v3 max;
};





inline b32 vectors_equal(v2 a, v2 b) {
    b32 result = (a.x == b.x) && (a.y == b.y);
    return result;
}





inline v2 vec2(f32 x, f32 y) {
    v2 result;
    result.x = x;
    result.y = y;
    return result;
}

inline v2 vec2(u32 x, u32 y) {
    v2 result;
    result.x = (f32)x;
    result.y = (f32)y;
    return result;
}

inline v2 vec2(s32 x, s32 y) {
    v2 result;
    result.x = (f32)x;
    result.y = (f32)y;
    return result;
}

inline v2 vec2(f32 s) {
    v2 result;
    result.x = s;
    result.y = s;
    return result;
}

inline b32 any(v2 v) {
    b32 result = v.x && v.y;
    return result;
}

inline v2 operator - (v2 a) {
    v2 result;
    result.x = -a.x;
    result.y = -a.y;
    return result;
}
# 290 "./math.h"
inline v2 operator + (v2 a, v2 b) { v2 result; result.x = a.x + b.x; result.y = a.y + b.y; return result; } inline v2 &operator += (v2& a, v2 b) { a = a + b; return a; }
inline v2 operator - (v2 a, v2 b) { v2 result; result.x = a.x - b.x; result.y = a.y - b.y; return result; } inline v2 &operator -= (v2& a, v2 b) { a = a - b; return a; }
inline v2 operator * (v2 a, f32 b) { v2 result; result.x = a.x * b; result.y = a.y * b; return result; } inline v2 operator * (f32 a, v2 b) { v2 result; result.x = a * b.x; result.y = a * b.y; return result; } inline v2 &operator *= (v2& a, f32 b) { a = a * b; return a; }
inline v2 operator / (v2 a, f32 b) { v2 result; result.x = a.x / b; result.y = a.y / b; return result; } inline v2 operator / (f32 a, v2 b) { v2 result; result.x = a / b.x; result.y = a / b.y; return result; } inline v2 &operator /= (v2& a, f32 b) { a = a / b; return a; }
inline v2 operator * (v2 a, v2 b) { v2 result; result.x = a.x * b.x; result.y = a.y * b.y; return result; } inline v2 &operator *= (v2& a, v2 b) { a = a * b; return a; }
inline v2 operator / (v2 a, v2 b) { v2 result; result.x = a.x / b.x; result.y = a.y / b.y; return result; } inline v2 &operator /= (v2& a, v2 b) { a = a / b; return a; }




inline v2 perp(v2 v) {
    v2 result = vec2(-v.y, v.x);
    return result;
}

inline v2 perp_clockwise(v2 v) {
    v2 result = vec2(v.y, -v.x);
    return result;
}

inline v2 mul(v2 a, v2 b) {
    v2 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    return result;
}

inline v2 square(v2 v) {
    v2 result;
    result.x = v.x * v.x;
    result.y = v.y * v.y;
    return result;
}

inline v2 lerp(v2 a, v2 b, f32 t) {
    return a * (1.0f - t) + b * t;
}

inline f32 dot(v2 a, v2 b) {
    return a.x * b.x + a.y * b.y;
}

inline v2 reflect(v2 v, v2 n) {
    return v - 2.0f*n*dot(v, n);
}

inline f32 length_sq(v2 v) {
    f32 result = dot(v, v);
    return result;
}

inline f32 length(v2 v) {
    f32 result = square_root(dot(v, v));
    return result;
}

inline v2 normalize(v2 v) {
    (void)( (!!(length(v) != 0.0f)) || (_wassert(L"length(v) != 0.0f", L"./math.h", (unsigned)(347)), 0) );
    return v / length(v);
}

inline v2 normalize_or_zero(v2 v) {
    v2 result = {};

    f32 l_sq = length_sq(v);
    if (l_sq > square(0.0001f)) {
        result = v * (1.0f / square_root(l_sq));
    }
    return result;
}

inline v2 min(v2 a, v2 b) {
    v2 result;
    result.x = min(a.x, b.x);
    result.y = min(a.y, b.y);
    return result;
}

inline v2 min(v2 a, f32 b) {
    v2 result;
    result.x = min(a.x, b);
    result.y = min(a.y, b);
    return result;
}

inline v2 min(f32 a, v2 b) {
    v2 result;
    result.x = min(a, b.x);
    result.y = min(a, b.y);
    return result;
}

inline v2 max(v2 a, v2 b) {
    v2 result;
    result.x = max(a.x, b.x);
    result.y = max(a.y, b.y);
    return result;
}

inline v2 max(v2 a, f32 b) {
    v2 result;
    result.x = max(a.x, b);
    result.y = max(a.y, b);
    return result;
}

inline v2 max(f32 a, v2 b) {
    v2 result;
    result.x = max(a, b.x);
    result.y = max(a, b.y);
    return result;
}

inline v2 clamp(v2 n, v2 lo, v2 hi) {
    v2 result;
    result.x = max(lo.x, min(hi.x, n.x));
    result.y = max(lo.y, min(hi.y, n.y));
    return result;
}

inline v2 clamp(v2 n, f32 lo, v2 hi) {
    v2 result;
    result.x = max(lo, min(hi.x, n.x));
    result.y = max(lo, min(hi.y, n.y));
    return result;
}

inline v2 clamp(v2 n, v2 lo, f32 hi) {
    v2 result;
    result.x = max(lo.x, min(hi, n.x));
    result.y = max(lo.y, min(hi, n.y));
    return result;
}

inline v2 clamp(v2 n, f32 lo, f32 hi) {
    v2 result;
    result.x = max(lo, min(hi, n.x));
    result.y = max(lo, min(hi, n.y));
    return result;
}

inline v2 arm2(f32 angle) {
    v2 result;
    result.x = my_cos(angle);
    result.y = my_sin(angle);
    return result;
}

inline v2 rotate(v2 v, v2 cos_sin) {
    v2 result = vec2(v.x*cos_sin.x - v.y*cos_sin.y, v.x*cos_sin.y + v.y*cos_sin.x);
    return result;
}

inline v2 rotate_clockwise(v2 v, v2 cos_sin) {
    v2 result = vec2(v.x*cos_sin.x + v.y*cos_sin.y, -v.x*cos_sin.y + v.y*cos_sin.x);
    return result;
}





inline v2 get_dim(Rect2 rect) {
    v2 result = rect.max - rect.min;
    return result;
}

inline v2 get_min_corner(Rect2 rect) {
    v2 result = rect.min;
    return result;
}

inline v2 get_max_corner(Rect2 rect) {
    v2 result = rect.max;
    return result;
}

inline v2 get_center(Rect2 rect) {
    v2 result = 0.5f * (rect.min + rect.max);
    return result;
}

inline Rect2 rect_min_max(v2 min, v2 max) {
    Rect2 result;
    result.min = min;
    result.max = max;
    return result;
}

inline Rect2 rect_center_half_dim(v2 center, v2 half_dim) {
    Rect2 result;
    result.min = center - half_dim;
    result.max = center + half_dim;
    return result;
}

inline Rect2 rect_center_dim(v2 center, v2 dim) {
    Rect2 result;
    result.min = center - dim * 0.5f;
    result.max = center + dim * 0.5f;
    return result;
}

inline Rect2 rect_min_dim(v2 min, v2 dim) {
    Rect2 result;
    result.min = min;
    result.max = min + dim;
    return result;
}

inline Rect2 multiply_dimensions(Rect2 rect, v2 mul) {
    Rect2 result;
    result.min = rect.min * mul;
    result.max = rect.max * mul;
    return result;
}

inline Rect2 grow_by_radius(Rect2 rect, v2 r_dim) {
    Rect2 result;
    result.min = rect.min - r_dim;
    result.max = rect.max + r_dim;
    return result;
}

inline Rect2 grow_by_diameter(Rect2 rect, v2 d_dim) {
    Rect2 result;
    result.min = rect.min - d_dim * 0.5f;
    result.max = rect.max + d_dim * 0.5f;
    return result;
}

inline Rect2 offset(Rect2 a, v2 offset) {
    Rect2 result;
    result.min = a.min + offset;
    result.max = a.max + offset;
    return result;
}

inline b32 is_in_rect(Rect2 rect, v2 test) {
    b32 result = (test.x >= rect.min.x && test.x < rect.max.x) &&
                  (test.y >= rect.min.y && test.y < rect.max.y);
    return result;
}

inline b32 rect_contained_in_rect(Rect2 outer, Rect2 inner) {
    b32 result = (outer.min.x >= inner.min.x && outer.max.x <= inner.max.x) &&
                  (outer.min.y >= inner.min.y && outer.max.y <= inner.max.y);
    return result;
}

inline b32 rects_intersect(Rect2 a, Rect2 b) {
    b32 result = !(b.max.x <= a.min.x ||
                    b.min.x >= a.max.x ||
                    b.max.y <= a.min.y ||
                    b.min.y >= a.max.y);
    return result;
}

inline v2 get_barycentric(Rect2 rect, v2 p) {
    v2 result;
    result.x = safe_ratio_0(p.x - rect.min.x, rect.max.x - rect.min.x);
    result.y = safe_ratio_0(p.y - rect.min.y, rect.max.y - rect.min.y);
    return result;
}

inline Rect2 rect_union(Rect2 a, Rect2 b) {
    Rect2 result;
    result.min.x = min(a.min.x, b.min.x);
    result.min.y = min(a.min.y, b.min.y);
    result.max.x = max(a.max.x, b.max.x);
    result.max.y = max(a.max.y, b.max.y);
    return result;
}

inline Rect2 intersect(Rect2 a, Rect2 b) {
    Rect2 result;
    result.min.x = max(a.min.x, b.min.x);
    result.min.y = max(a.min.y, b.min.y);
    result.max.x = min(a.max.x, b.max.x);
    result.max.y = min(a.max.y, b.max.y);
    return result;
}

inline Rect2 inverted_infinity_rectangle2() {
    Rect2 result;
    result.min.x = result.min.y = 3.40282347e+38F;
    result.max.x = result.max.y = -3.40282347e+38F;
    return result;
}





struct Rect2i {
    s32 min_x, min_y;
    s32 max_x, max_y;
};

inline Rect2i rect_union(Rect2i a, Rect2i b) {
    Rect2i result;
    result.min_x = ((a.min_x) < (b.min_x) ? (a.min_x) : (b.min_x));
    result.min_y = ((a.min_y) < (b.min_y) ? (a.min_y) : (b.min_y));
    result.max_x = ((a.max_x) > (b.max_x) ? (a.max_x) : (b.max_x));
    result.max_y = ((a.max_y) > (b.max_y) ? (a.max_y) : (b.max_y));
    return result;
}

inline Rect2i intersect(Rect2i a, Rect2i b) {
    Rect2i result;
    result.min_x = ((a.min_x) > (b.min_x) ? (a.min_x) : (b.min_x));
    result.min_y = ((a.min_y) > (b.min_y) ? (a.min_y) : (b.min_y));
    result.max_x = ((a.max_x) < (b.max_x) ? (a.max_x) : (b.max_x));
    result.max_y = ((a.max_y) < (b.max_y) ? (a.max_y) : (b.max_y));
    return result;
}

inline s32 get_area(Rect2i a) {
    s32 w = (a.max_x - a.min_x);
    s32 h = (a.max_y - a.min_y);
    s32 area = 0;
    if (w > 0 && h > 0) {
        area = w * h;
    }
    return area;
}

inline Rect2i inverted_infinity_rectangle2i() {
    Rect2i result;
    result.min_x = result.min_y = 2147483647i32;
    result.max_x = result.max_y = -2147483647i32;
    return result;
}





inline v3 vec3(f32 x, f32 y, f32 z) {
    v3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

inline v3 vec3(f32 s) {
    v3 result;
    result.x = s;
    result.y = s;
    result.z = s;
    return result;
}

inline v3 vec3(v2 v2, f32 z) {
    v3 result;
    result.x = v2.x;
    result.y = v2.y;
    result.z = z;
    return result;
}

inline v3 vec3(f32 x, v2 v2) {
    v3 result;
    result.x = x;
    result.y = v2.x;
    result.z = v2.y;
    return result;
}

inline b32 any(v3 v) {
    b32 result = v.x && v.y && v.z;
    return result;
}

inline v3 operator - (v3 a) {
    v3 result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    return result;
}
# 708 "./math.h"
inline v3 operator + (v3 a, v3 b) { v3 result; result.x = a.x + b.x; result.y = a.y + b.y; result.z = a.z + b.z; return result; } inline v3 &operator += (v3& a, v3 b) { a = a + b; return a; }
inline v3 operator - (v3 a, v3 b) { v3 result; result.x = a.x - b.x; result.y = a.y - b.y; result.z = a.z - b.z; return result; } inline v3 &operator -= (v3& a, v3 b) { a = a - b; return a; }
inline v3 operator * (v3 a, f32 b) { v3 result; result.x = a.x * b; result.y = a.y * b; result.z = a.z * b; return result; } inline v3 operator * (f32 a, v3 b) { v3 result; result.x = a * b.x; result.y = a * b.y; result.z = a * b.z; return result; } inline v3 &operator *= (v3& a, f32 b) { a = a * b; return a; }
inline v3 operator / (v3 a, f32 b) { v3 result; result.x = a.x / b; result.y = a.y / b; result.z = a.z / b; return result; } inline v3 operator / (f32 a, v3 b) { v3 result; result.x = a / b.x; result.y = a / b.y; result.z = a / b.z; return result; } inline v3 &operator /= (v3& a, f32 b) { a = a / b; return a; }
inline v3 operator * (v3 a, v3 b) { v3 result; result.x = a.x * b.x; result.y = a.y * b.y; result.z = a.z * b.z; return result; } inline v3 &operator *= (v3& a, v3 b) { a = a * b; return a; }
inline v3 operator / (v3 a, v3 b) { v3 result; result.x = a.x / b.x; result.y = a.y / b.y; result.z = a.z / b.z; return result; } inline v3 &operator /= (v3& a, v3 b) { a = a / b; return a; }




inline v3 shift(v3 v, f32 s) {
    v3 result;
    result.x += s;
    result.y += s;
    result.z += s;
    return result;
}

inline v3 mul(v3 a, v3 b) {
    v3 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    return result;
}

inline v3 square(v3 v) {
    v3 result;
    result.x = v.x * v.x;
    result.y = v.y * v.y;
    result.z = v.z * v.z;
    return result;
}

inline v3 square_root(v3 v) {
    v3 result;
    result.x = square_root(v.x);
    result.y = square_root(v.y);
    result.z = square_root(v.z);
    return result;
}

inline v3 lerp(v3 a, v3 b, f32 t) {
    return a * (1.0f - t) + b * t;
}

inline f32 dot(v3 a, v3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline v3 cross(v3 a, v3 b) {
    return vec3(
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    );
}

inline v3 reflect(v3 a, v3 b) {
    return a - 2.0f * dot(a, b) * b;
}

inline f32 length_sq(v3 v) {
    f32 result = dot(v, v);
    return result;
}

inline f32 length(v3 v) {
    f32 result = square_root(dot(v, v));
    return result;
}

inline v3 normalize(v3 v) {
    v3 result = v / length(v);
    return result;
}

inline v3 normalize_or_zero(v3 v) {
    v3 result = {};

    f32 l_sq = length_sq(v);
    if (l_sq > square(0.0001f)) {
        result = v * (1.0f / square_root(l_sq));
    }
    return result;
}

inline v3 min(v3 a, v3 b) {
    v3 result;
    result.x = min(a.x, b.x);
    result.y = min(a.y, b.y);
    result.z = min(a.z, b.z);
    return result;
}

inline v3 min(v3 a, f32 b) {
    v3 result;
    result.x = min(a.x, b);
    result.y = min(a.y, b);
    result.z = min(a.z, b);
    return result;
}

inline v3 min(f32 a, v3 b) {
    v3 result;
    result.x = min(a, b.x);
    result.y = min(a, b.y);
    result.z = min(a, b.z);
    return result;
}

inline v3 max(v3 a, v3 b) {
    v3 result;
    result.x = max(a.x, b.x);
    result.y = max(a.y, b.y);
    result.z = max(a.z, b.z);
    return result;
}

inline v3 max(v3 a, f32 b) {
    v3 result;
    result.x = max(a.x, b);
    result.y = max(a.y, b);
    result.z = max(a.z, b);
    return result;
}

inline v3 max(f32 a, v3 b) {
    v3 result;
    result.x = max(a, b.x);
    result.y = max(a, b.y);
    result.z = max(a, b.z);
    return result;
}

inline v3 clamp(v3 n, v3 lo, v3 hi) {
    v3 result;
    result.x = max(lo.x, min(hi.x, n.x));
    result.y = max(lo.y, min(hi.y, n.y));
    result.z = max(lo.z, min(hi.z, n.z));
    return result;
}

inline v3 clamp(v3 n, f32 lo, v3 hi) {
    v3 result;
    result.x = max(lo, min(hi.x, n.x));
    result.y = max(lo, min(hi.y, n.y));
    result.z = max(lo, min(hi.z, n.z));
    return result;
}

inline v3 clamp(v3 n, v3 lo, f32 hi) {
    v3 result;
    result.x = max(lo.x, min(hi, n.x));
    result.y = max(lo.y, min(hi, n.y));
    result.z = max(lo.z, min(hi, n.z));
    return result;
}

inline v3 clamp(v3 n, f32 lo, f32 hi) {
    v3 result;
    result.x = max(lo, min(hi, n.x));
    result.y = max(lo, min(hi, n.y));
    result.z = max(lo, min(hi, n.z));
    return result;
}





inline v3 get_dim(Rect3 rect) {
    v3 result = rect.max - rect.min;
    return result;
}

inline v3 get_min_corner(Rect3 rect) {
    v3 result = rect.min;
    return result;
}

inline v3 get_max_corner(Rect3 rect) {
    v3 result = rect.max;
    return result;
}

inline v3 get_center(Rect3 rect) {
    v3 result = 0.5f * (rect.min + rect.max);
    return result;
}

inline Rect3 rect_min_max(v3 min, v3 max) {
    Rect3 result;
    result.min = min;
    result.max = max;
    return result;
}

inline Rect3 rect_center_half_dim(v3 center, v3 half_dim) {
    Rect3 result;
    result.min = center - half_dim;
    result.max = center + half_dim;
    return result;
}

inline Rect3 rect_center_dim(v3 center, v3 dim) {
    Rect3 result;
    result.min = center - dim * 0.5f;
    result.max = center + dim * 0.5f;
    return result;
}

inline Rect3 rect_min_dim(v3 min, v3 dim) {
    Rect3 result;
    result.min = min;
    result.max = min + dim;
    return result;
}

inline Rect3 multiply_dimensions(Rect3 rect, v3 mul) {
    Rect3 result;
    result.min = rect.min * mul;
    result.max = rect.max * mul;
    return result;
}

inline Rect3 grow_by_radius(Rect3 rect, v3 r_dim) {
    Rect3 result;
    result.min = rect.min - r_dim;
    result.max = rect.max + r_dim;
    return result;
}

inline Rect3 grow_by_diameter(Rect3 rect, v3 d_dim) {
    Rect3 result;
    result.min = rect.min - d_dim * 0.5f;
    result.max = rect.max + d_dim * 0.5f;
    return result;
}

inline Rect3 offset(Rect3 a, v3 offset) {
    Rect3 result;
    result.min = a.min + offset;
    result.max = a.max + offset;
    return result;
}

inline b32 is_in_rect(Rect3 rect, v3 test) {
    b32 result = (test.x >= rect.min.x && test.x < rect.max.x) &&
                  (test.y >= rect.min.y && test.y < rect.max.y) &&
                  (test.z >= rect.min.z && test.z < rect.max.z);
    return result;
}

inline b32 rect_contained_in_rect(Rect3 outer, Rect3 inner) {
    b32 result = (outer.min.x >= inner.min.x && outer.max.x <= inner.max.x) &&
                  (outer.min.y >= inner.min.y && outer.max.y <= inner.max.y) &&
                  (outer.min.z >= inner.min.z && outer.max.z <= inner.max.z);
    return result;
}

inline b32 rects_intersect(Rect3 a, Rect3 b) {
    b32 result = !(b.max.x <= a.min.x ||
                    b.min.x >= a.max.x ||
                    b.max.y <= a.min.y ||
                    b.min.y >= a.max.y ||
                    b.max.z <= a.min.z ||
                    b.min.z >= a.max.z);
    return result;
}

inline v3 get_barycentric(Rect3 rect, v3 p) {
    v3 result;
    result.x = safe_ratio_0(p.x - rect.min.x, rect.max.x - rect.min.x);
    result.y = safe_ratio_0(p.y - rect.min.y, rect.max.y - rect.min.y);
    result.z = safe_ratio_0(p.z - rect.min.z, rect.max.z - rect.min.z);
    return result;
}

inline Rect2 to_rect2_xy(Rect3 rect) {
    Rect2 result;
    result.min = rect.min.xy;
    result.max = rect.max.xy;
    return result;
}





inline v4 vec4(f32 x, f32 y, f32 z, f32 w) {
    v4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return result;
}

inline v4 vec4(f32 s) {
    v4 result;
    result.x = s;
    result.y = s;
    result.z = s;
    result.w = s;
    return result;
}

inline v4 vec4(v2 v2, f32 z, f32 w) {
    v4 result;
    result.x = v2.x;
    result.y = v2.y;
    result.z = z;
    result.w = w;
    return result;
}

inline v4 vec4(f32 x, v2 v2, f32 w) {
    v4 result;
    result.x = x;
    result.y = v2.x;
    result.z = v2.y;
    result.w = w;
    return result;
}

inline v4 vec4(f32 x, f32 y, v2 v2) {
    v4 result;
    result.x = x;
    result.y = y;
    result.z = v2.x;
    result.w = v2.y;
    return result;
}

inline v4 vec4(v2 a, v2 b) {
    v4 result;
    result.x = a.x;
    result.y = a.y;
    result.z = b.x;
    result.w = b.y;
    return result;
}

inline v4 vec4(v3 v3, f32 w) {
    v4 result;
    result.x = v3.x;
    result.y = v3.y;
    result.z = v3.z;
    result.w = w;
    return result;
}

inline v4 vec4(f32 x, v3 v3) {
    v4 result;
    result.x = x;
    result.y = v3.x;
    result.z = v3.y;
    result.w = v3.z;
    return result;
}

inline b32 any(v4 v) {
    b32 result = v.x && v.y && v.z && v.w;
    return result;
}

inline v4 operator - (v4 a) {
    v4 result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    result.w = -a.w;
    return result;
}
# 1122 "./math.h"
inline v4 operator + (v4 a, v4 b) { v4 result; result.x = a.x + b.x; result.y = a.y + b.y; result.z = a.z + b.z; result.w = a.w + b.w; return result; } inline v4 &operator += (v4& a, v4 b) { a = a + b; return a; }
inline v4 operator - (v4 a, v4 b) { v4 result; result.x = a.x - b.x; result.y = a.y - b.y; result.z = a.z - b.z; result.w = a.w - b.w; return result; } inline v4 &operator -= (v4& a, v4 b) { a = a - b; return a; }
inline v4 operator * (v4 a, f32 b) { v4 result; result.x = a.x * b; result.y = a.y * b; result.z = a.z * b; result.w = a.w * b; return result; } inline v4 operator * (f32 a, v4 b) { v4 result; result.x = a * b.x; result.y = a * b.y; result.z = a * b.z; result.w = a * b.w; return result; } inline v4 &operator *= (v4& a, f32 b) { a = a * b; return a; }
inline v4 operator / (v4 a, f32 b) { v4 result; result.x = a.x / b; result.y = a.y / b; result.z = a.z / b; result.w = a.w / b; return result; } inline v4 operator / (f32 a, v4 b) { v4 result; result.x = a / b.x; result.y = a / b.y; result.z = a / b.z; result.w = a / b.w; return result; } inline v4 &operator /= (v4& a, f32 b) { a = a / b; return a; }
inline v4 operator * (v4 a, v4 b) { v4 result; result.x = a.x * b.x; result.y = a.y * b.y; result.z = a.z * b.z; result.w = a.w * b.w; return result; } inline v4 &operator *= (v4& a, v4 b) { a = a * b; return a; }
inline v4 operator / (v4 a, v4 b) { v4 result; result.x = a.x / b.x; result.y = a.y / b.y; result.z = a.z / b.z; result.w = a.w / b.w; return result; } inline v4 &operator /= (v4& a, v4 b) { a = a / b; return a; }




inline v4 mul(v4 a, v4 b) {
    v4 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    result.w = a.w * b.w;
    return result;
}

inline v4 square(v4 v) {
    v4 result;
    result.x = v.x * v.x;
    result.y = v.y * v.y;
    result.z = v.z * v.z;
    result.w = v.w * v.w;
    return result;
}

inline v4 lerp(v4 a, v4 b, f32 t) {
    return a * (1.0f - t) + b * t;
}

inline f32 dot(v4 a, v4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline v4 reflect(v4 a, v4 b) {
    return a - 2.0f * dot(a, b) * b;
}

inline f32 length_sq(v4 v) {
    f32 result = dot(v, v);
    return result;
}

inline f32 length(v4 v) {
    f32 result = square_root(dot(v, v));
    return result;
}

inline v4 min(v4 a, v4 b) {
    v4 result;
    result.x = min(a.x, b.x);
    result.y = min(a.y, b.y);
    result.z = min(a.z, b.z);
    result.w = min(a.w, b.w);
    return result;
}

inline v4 min(v4 a, f32 b) {
    v4 result;
    result.x = min(a.x, b);
    result.y = min(a.y, b);
    result.z = min(a.z, b);
    result.w = min(a.w, b);
    return result;
}

inline v4 min(f32 a, v4 b) {
    v4 result;
    result.x = min(a, b.x);
    result.y = min(a, b.y);
    result.z = min(a, b.z);
    result.w = min(a, b.w);
    return result;
}

inline v4 max(v4 a, v4 b) {
    v4 result;
    result.x = max(a.x, b.x);
    result.y = max(a.y, b.y);
    result.z = max(a.z, b.z);
    result.w = max(a.w, b.w);
    return result;
}

inline v4 max(v4 a, f32 b) {
    v4 result;
    result.x = max(a.x, b);
    result.y = max(a.y, b);
    result.z = max(a.z, b);
    result.w = max(a.w, b);
    return result;
}

inline v4 max(f32 a, v4 b) {
    v4 result;
    result.x = max(a, b.x);
    result.y = max(a, b.y);
    result.z = max(a, b.z);
    result.w = max(a, b.w);
    return result;
}

inline v4 clamp(v4 n, v4 lo, v4 hi) {
    v4 result;
    result.x = max(lo.x, min(hi.x, n.x));
    result.y = max(lo.y, min(hi.y, n.y));
    result.z = max(lo.z, min(hi.z, n.z));
    result.w = max(lo.w, min(hi.w, n.w));
    return result;
}

inline v4 clamp(v4 n, f32 lo, v4 hi) {
    v4 result;
    result.x = max(lo, min(hi.x, n.x));
    result.y = max(lo, min(hi.y, n.y));
    result.z = max(lo, min(hi.z, n.z));
    result.w = max(lo, min(hi.w, n.w));
    return result;
}

inline v4 clamp(v4 n, v4 lo, f32 hi) {
    v4 result;
    result.x = max(lo.x, min(hi, n.x));
    result.y = max(lo.y, min(hi, n.y));
    result.z = max(lo.z, min(hi, n.z));
    result.w = max(lo.w, min(hi, n.w));
    return result;
}

inline v4 clamp(v4 n, f32 lo, f32 hi) {
    v4 result;
    result.x = max(lo, min(hi, n.x));
    result.y = max(lo, min(hi, n.y));
    result.z = max(lo, min(hi, n.z));
    result.w = max(lo, min(hi, n.w));
    return result;
}

inline v3 srgb_to_linear(v3 c) {
    v3 result;

    result = square(c);

    return result;
}

inline v3 linear_to_srgb(v3 c) {
    v3 result;

    result = square_root(c);

    return result;
}

inline v4 srgb_to_linear(v4 c) {
    v4 result;

    result.rgb = srgb_to_linear(c.rgb);
    result.a = c.a;

    return result;
}

inline v4 linear_to_srgb(v4 c) {
    v4 result;

    result.rgb = linear_to_srgb(c.rgb);
    result.a = c.a;

    return result;
}


inline v4 srgb255_to_linear1(v4 c) {
    v4 result;

    result.rgb = srgb_to_linear(c.rgb * (1.0f/255.0f));
    result.a = c.a / 255.0f;

    return result;
}

inline v4 linear1_to_srgb255(v4 c) {
    v4 result;

    result.rgb = linear_to_srgb(c.rgb) * 255.0f;
    result.a = c.a * 255.0f;

    return result;
}
# 5 "./pulsar_platform_bridge.h" 2
# 1 "./file_io.h" 1



typedef struct EntireFile {
    size_t size;
    void* data;
} EntireFile;
# 6 "./pulsar_platform_bridge.h" 2
# 1 "./pulsar_asset_pack_file_format.h" 1



struct SoundID { u32 value; };
struct ImageID { u32 value; };
struct FontID { u32 value; };
struct MidiID { u32 value; };
struct SoundtrackID { u32 value; };

enum AssetType {
    AssetType_Unknown,

    AssetType_Image,
    AssetType_Sound,
    AssetType_Font,
    AssetType_Midi,

    AssetType_Soundtrack,
};

enum PixelFormat {
    PixelFormat_BGRA8,
    PixelFormat_A8,
};






struct PackedImage {
    PixelFormat pixel_format; u32 w, h; v2 align;;




};





struct PackedSound {
    u32 channel_count; u32 sample_count;;




};
# 60 "./pulsar_asset_pack_file_format.h"
struct PackedFont {
    u32 first_codepoint; u32 one_past_last_codepoint; u32 size; f32 whitespace_width; f32 ascent; f32 descent; f32 line_gap;;






};
# 78 "./pulsar_asset_pack_file_format.h"
struct PackedMidi {
    u32 ticks_per_second; u32 beats_per_minute; u16 time_signature_numerator; u16 time_signature_denominator; u32 event_count;;




};





struct PackedSoundtrack {
    SoundID sound; u32 midi_track_count;;




};

enum MidiEventType {
    MidiEvent_NoteOn,
    MidiEvent_NoteOff,
};
# 110 "./pulsar_asset_pack_file_format.h"
struct MidiEvent {
    u32 absolute_time_in_ticks; u8 channel; u8 type; u8 note_value; u8 velocity;;
};

struct PackedAsset {
    u64 data_offset;
    u32 name_offset;
    AssetType type;
    union {
        PackedSound sound;
        PackedImage image;
        PackedFont font;
        PackedMidi midi;
        PackedSoundtrack soundtrack;
    };
};


struct AssetPackHeader {
    u32 magic_value;


    u32 version;

    u32 asset_count;
    u32 asset_catalog;
    u32 asset_name_store;
    u32 asset_data;
};
# 7 "./pulsar_platform_bridge.h" 2
# 1 "./pulsar_render_commands.h" 1



struct Image {
    union { PackedImage packed_image; struct { PixelFormat pixel_format; u32 w, h; v2 align; }; };
    void* pixels;
    void* handle;
};

struct Transform2D {
    v2 offset;
    v2 rotation_arm;
    v2 sweep;
    f32 scale;
};

inline Transform2D default_transform2d() {
    Transform2D result = {};
    result.rotation_arm = vec2(1, 0);
    result.scale = 1.0f;
    return result;
}

inline Transform2D transform2d(v2 offset, v2 sweep = vec2(0, 0)) {
    Transform2D result = default_transform2d();
    result.offset = offset;
    result.sweep = sweep;
    return result;
}

enum ShapeType {
    Shape_Polygon,
    Shape_Circle,
    Shape_Rectangle,
};

struct Shape2D {
    ShapeType type;
    union {

        struct {
            u32 vert_count;
            v2* vertices;
        };


        struct {
            f32 radius;
        };


        struct {
            Rect2 rect;
        };
    };
};

inline Shape2D polygon(u32 vert_count, v2* vertices) {
    Shape2D result = {};
    result.type = Shape_Polygon;
    result.vert_count = vert_count;
    result.vertices = vertices;
    return result;
}

inline Shape2D circle(f32 radius) {
    Shape2D result = {};
    result.type = Shape_Circle;
    result.radius = radius;
    return result;
}

inline Shape2D rectangle(Rect2 rect) {
    Shape2D result = {};
    result.type = Shape_Rectangle;
    result.rect = rect;
    return result;
}

enum RenderCommandType {
    RenderCommand_Clear,
    RenderCommand_Shape,
    RenderCommand_Image,
};

struct RenderCommandHeader {
    u8 type;
};

struct RenderCommandClear {
    v4 color;
};

struct RenderCommandShape {

    Transform2D transform;
    Shape2D shape;
    v4 color;
};

struct RenderCommandImage {
    Image* image;
    v2 p;
    v4 color;
};
# 8 "./pulsar_platform_bridge.h" 2


typedef EntireFile PlatformReadEntireFile(char* file_name);


typedef b32 PlatformWriteEntireFile(char* file_name, u32 size, void* data);


typedef void* PlatformAllocateMemory(size_t size);


typedef void PlatformDeallocateMemory(void* memory);


typedef void* PlatformAllocateTexture(u32 w, u32 h, void* data, PixelFormat format);


typedef void PlatformDeallocateTexture(void* handle);

struct PlatformAPI {
    PlatformReadEntireFile* read_entire_file;
    PlatformWriteEntireFile* write_entire_file;
    PlatformAllocateMemory* allocate;
    PlatformDeallocateMemory* deallocate;
    PlatformAllocateTexture* allocate_texture;
    PlatformDeallocateTexture* deallocate_texture;
};




struct GameMemory {
    b32 initialized;

    size_t permanent_storage_size;
    void* permanent_storage;
    size_t transient_storage_size;
    void* transient_storage;

    PlatformAPI platform_api;
};

struct GameSoundOutputBuffer {
    u32 channel_count;
    u32 sample_rate;
    u32 sample_count;
    s16* samples;
};

struct GameButtonState {
    b32 is_down;
    u32 half_transition_count;
};

enum GameInputMouseButton {
    PlatformMouseButton_Left,
    PlatformMouseButton_Middle,
    PlatformMouseButton_Right,
    PlatformMouseButton_Extended0,
    PlatformMouseButton_Extended1,
    PlatformMouseButton_Count,
};

struct GameController {
    union {
        GameButtonState buttons[8];
        struct {
            GameButtonState move_up;
            GameButtonState move_down;
            GameButtonState move_left;
            GameButtonState move_right;

            GameButtonState action_up;
            GameButtonState action_down;
            GameButtonState action_left;
            GameButtonState action_right;
        };
    };
};

struct GameRenderCommands {
    u32 width, height;

    u32 command_buffer_size;
    u32 command_buffer_used;
    u8* command_buffer;
};



struct GameInput {
    b32 in_focus, focus_changed;
    f32 frame_dt;

    GameController controller;

    u32 mouse_x, mouse_y, mouse_z;
    GameButtonState mouse_buttons[PlatformMouseButton_Count];

    GameButtonState debug_fkeys[13];

    b32 quit_requested;
};

inline b32 was_pressed(GameButtonState button) {
    b32 result = (button.half_transition_count > 1) || (button.is_down && button.half_transition_count > 0);
    return result;
}

inline b32 was_released(GameButtonState button) {
    b32 result = (button.half_transition_count > 1) || (!button.is_down && button.half_transition_count > 0);
    return result;
}


typedef void GameUpdateAndRender(GameMemory* memory, GameInput* input, GameRenderCommands* render_commands);




typedef void GameGetSound(GameMemory* memory, GameSoundOutputBuffer* sound_buffer);
# 21 "./pulsar_main.h" 2

# 1 "./string.h" 1



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
    result.len = cstr_length(cstr);
    result.data = cstr;
    return result;
}

inline String string_from_two_pointers(char* ptr1, char* ptr2) {


    String result;
    result.len = (size_t)(ptr2 - ptr1);
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
    (void)( (!!(end >= start)) || (_wassert(L"end >= start", L"./string.h", (unsigned)(153)), 0) );
    (void)( (!!(end < source->len)) || (_wassert(L"end < source->len", L"./string.h", (unsigned)(154)), 0) );

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
    while (is_whitespace(peek(string))) {
        advance(string);
    }

    char* start = string->data;
    while (!is_whitespace(peek(string))) {
        advance(string);
    }
    char* end = string->data;

    while (is_whitespace(peek(string))) {
        advance(string);
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
# 23 "./pulsar_main.h" 2

# 1 "./opengl.h" 1



static const GLuint GL_FRAMEBUFFER_SRGB = 0x8DB9;
static const GLuint GL_SRGB8_ALPHA8 = 0x8C43;
static const GLuint GL_NUM_EXTENSIONS = 0x821D;
static const GLuint GL_SHADING_LANGUAGE_VERSION = 0x8B8C;

static const GLuint GL_MULTISAMPLE_ARB = 0x809D;
static const GLuint GL_SAMPLE_ALPHA_TO_COVERAGE_ARB = 0x809E;
static const GLuint GL_SAMPLE_ALPHA_TO_ONE_ARB = 0x809F;
static const GLuint GL_SAMPLE_COVERAGE_ARB = 0x80A0;





typedef GLubyte* WINAPI GL_FUNCTION_glGetStringi(GLenum name, GLuint index); static GL_FUNCTION_glGetStringi* glGetStringi;;

struct OpenGLInfo {
    b32 modern_context;

    char* vendor;
    char* renderer;
    char* version;
    char* shading_language_version;
    char* extensions;

    GLuint default_internal_texture_format;


    b32 GL_EXT_texture_sRGB;
    b32 GL_EXT_framebuffer_sRGB;
    b32 GL_ARB_framebuffer_sRGB;
    b32 GL_ARB_multisample;
};
# 25 "./pulsar_main.h" 2

# 1 "./pulsar_assets.h" 1



struct Sound {
    using_struct(PackedSound, packed_sound);
    s16* samples;
};


struct Image;

struct Font {
    using_struct(PackedFont, packed_font);
    ImageID* glyph_table;
    f32* kerning_table;
};

struct MidiTrack {
    using_struct(PackedMidi, packed_midi);
    MidiEvent* events;
};

struct Soundtrack {
    using_struct(PackedSoundtrack, packed_soundtrack);
    MidiID* midi_tracks;
};

struct Asset {
    char* name;
    AssetType type;
    union {
        Sound sound;
        Image image;
        Font font;
        MidiTrack midi_track;
        Soundtrack soundtrack;
    };
};

struct Assets {
    u32 asset_count;
    Asset* asset_catalog;

    u8* asset_data;
};
# 27 "./pulsar_main.h" 2
# 1 "./pulsar_audio_mixer.h" 1



enum PlaybackFlag {
    Playback_Looping = 0x1,
};

struct PlayingSound {
    f32 current_volume[2];
    u32 samples_played;

    u32 flags;

    Sound* sound;

    PlayingSound* next;
};

struct AudioMixer {
    MemoryArena* arena;

    f32 master_volume[2];

    PlayingSound* first_playing_sound;
    PlayingSound* first_free_playing_sound;
};
# 28 "./pulsar_main.h" 2



enum GameMode {
    GameMode_Ingame,
    GameMode_Editor,
};

enum EntityFlag {
    EntityFlag_Moveable = 0x1,
    EntityFlag_Collides = 0x2,
    EntityFlag_OnGround = 0x4,
};

enum EntityType {
    EntityType_Null,

    EntityType_Player,
    EntityType_Wall,
    EntityType_SoundtrackPlayer,
};

struct EntityID { u32 value; };

struct Entity {
    EntityType type;
    v2 p;
    v2 dp;
    v2 ddp;

    Entity* sticking_entity;
    v2 sticking_dp;

    b32 was_on_ground;
    f32 surface_friction;

    f32 movement_t;
    f32 off_ground_timer;
    f32 friction_of_last_touched_surface;

    u32 midi_note;
    v2 midi_test_target;

    b32 soundtrack_has_been_played;
    SoundtrackID soundtrack_id;
    u32 playback_flags;

    u32 flags;

    v4 color;

    Shape2D collision;
};

struct PlayingMidi {
    union {
        PlayingMidi* next;
        PlayingMidi* next_free;
    };



    PlayingSound* sync_sound;

    u32 tick_timer;
    u32 event_index;
    MidiTrack* track;

    u32 flags;
};


struct Level {
    String name;

    u32 entity_count;
    Entity entities[8192];
};

struct EditorState {
    b32 initialized;

    MemoryArena* arena;

    Level* active_level;

    GameRenderCommands* render_commands;
    Assets* assets;
    Font* font;
};

struct ActiveMidiEvent {
    union { MidiEvent midi_event; struct { u32 absolute_time_in_ticks; u8 channel; u8 type; u8 note_value; u8 velocity; }; };;
    f32 dt_left;
};

struct GameState {
    MemoryArena permanent_arena;
    MemoryArena transient_arena;

    EditorState* editor_state;

    GameMode game_mode;

    AudioMixer audio_mixer;
    Assets assets;

    Sound* test_music;
    Sound* test_sound;
    Image* test_image;
    Soundtrack* test_soundtrack;

    PlayingMidi* first_playing_midi;
    PlayingMidi* first_free_playing_midi;

    u32 midi_event_buffer_count;
    ActiveMidiEvent midi_event_buffer[256];

    Level* active_level;

    u32 entity_count;
    Entity entities[8192];

    Shape2D player_collision;

    u32 sound_timer;
};

static PlatformAPI platform;
# 2 "pulsar_main.cpp" 2

# 1 "./pulsar_assets.cpp" 1
static void load_assets(Assets* assets, MemoryArena* arena, char* file_name) {



    EntireFile asset_file = platform.read_entire_file(file_name);
    if (asset_file.size > 0) {
        AssetPackHeader* header = (AssetPackHeader*) asset_file.data;
        (void)( (!!(header->magic_value == (((u32)('p') << 0) | ((u32)('l') << 8) | ((u32)('a') << 16) | ((u32)('f') << 24)))) || (_wassert(L"header->magic_value == ASSET_PACK_CODE('p', 'l', 'a', 'f')", L"./pulsar_assets.cpp", (unsigned)(8)), 0) );
        (void)( (!!(header->version == 0)) || (_wassert(L"header->version == 0", L"./pulsar_assets.cpp", (unsigned)(9)), 0) );

        assets->asset_count = header->asset_count;
        assets->asset_catalog = (Asset*)push_size_(arena, sizeof(Asset) * (assets->asset_count), no_clear());

        char* name_store = (char*) header + header->asset_name_store;
        PackedAsset* source_catalog = (PackedAsset*) ((u8*) header + header->asset_catalog);
        assets->asset_data = (u8*) header + header->asset_data;

        for (u32 asset_index = 0; asset_index < assets->asset_count; asset_index++) {
            PackedAsset* source_asset = source_catalog + asset_index;
            Asset* dest_asset = assets->asset_catalog + asset_index;

            dest_asset->name = name_store + source_asset->name_offset;


            dest_asset->type = source_asset->type;





            switch (source_asset->type) {
                case AssetType_Image: {
                    Image* image = &dest_asset->image;
                    image->packed_image = source_asset->image;
                    image->pixels = assets->asset_data + source_asset->data_offset;
                    image->handle = platform.allocate_texture(image->w, image->h, image->pixels, image->pixel_format);
                } break;

                case AssetType_Sound: {
                    Sound* sound = &dest_asset->sound;
                    sound->packed_sound = source_asset->sound;
                    sound->samples = (s16*) (assets->asset_data + source_asset->data_offset);
                } break;

                case AssetType_Font: {
                    Font* font = &dest_asset->font;
                    font->packed_font = source_asset->font;
                    u32 glyph_count = font->one_past_last_codepoint - font->first_codepoint;
                    font->glyph_table = (ImageID*) (assets->asset_data + source_asset->data_offset);
                    font->kerning_table = (f32*) (font->glyph_table + sizeof(ImageID)*glyph_count);
                } break;

                case AssetType_Midi: {
                    MidiTrack* midi_track = &dest_asset->midi_track;
                    midi_track->packed_midi = source_asset->midi;
                    midi_track->events = (MidiEvent*) (assets->asset_data + source_asset->data_offset);
                } break;

                case AssetType_Soundtrack: {
                    Soundtrack* soundtrack = &dest_asset->soundtrack;
                    soundtrack->packed_soundtrack = source_asset->soundtrack;
                    soundtrack->midi_tracks = (MidiID*) (assets->asset_data + source_asset->data_offset);
                } break;
            }
        }
    } else {

        (void)( (!!(!"Invalid Code Path")) || (_wassert(L"!\"Invalid Code Path\"", L"./pulsar_assets.cpp", (unsigned)(68)), 0) );;
    }
}

inline u32 get_asset_id_by_name(Assets* assets, char* name, AssetType asset_type = AssetType_Unknown) {
    u32 result = 0;

    for (u32 asset_index = 1; asset_index < assets->asset_count; asset_index++) {
        Asset* asset = assets->asset_catalog + asset_index;
        if (asset->name && strings_are_equal(asset->name, name)) {
            if (asset_type == AssetType_Unknown || asset->type == asset_type) {
                result = asset_index;
            }
        }
    }

    return result;
}

inline ImageID get_image_id_by_name(Assets* assets, char* name) {
    ImageID result = { get_asset_id_by_name(assets, name, AssetType_Image) };
    return result;
}

inline SoundID get_sound_id_by_name(Assets* assets, char* name) {
    SoundID result = { get_asset_id_by_name(assets, name, AssetType_Sound) };
    return result;
}

inline FontID get_font_id_by_name(Assets* assets, char* name) {
    FontID result = { get_asset_id_by_name(assets, name, AssetType_Font) };
    return result;
}

inline SoundtrackID get_soundtrack_id_by_name(Assets* assets, char* name) {
    SoundtrackID result = { get_asset_id_by_name(assets, name, AssetType_Soundtrack) };
    return result;
}

inline Asset* get_asset(Assets* assets, u32 asset_id) {
    (void)( (!!(asset_id < assets->asset_count)) || (_wassert(L"asset_id < assets->asset_count", L"./pulsar_assets.cpp", (unsigned)(108)), 0) );
    Asset* result = assets->asset_catalog + asset_id;
    return result;
}

inline Sound* get_sound(Assets* assets, SoundID id) {
    Asset* asset = get_asset(assets, id.value);
    Sound* result = 0;
    if (asset->type == AssetType_Sound) {
        result = &asset->sound;
    } else {
        (void)( (!!(!"Invalid Code Path")) || (_wassert(L"!\"Invalid Code Path\"", L"./pulsar_assets.cpp", (unsigned)(119)), 0) );;
    }

    return result;
}

inline Sound* get_sound_by_name(Assets* assets, char* name) {
    Sound* result = 0;

    u32 asset_id = get_asset_id_by_name(assets, name);
    if (asset_id) {
        result = get_sound(assets, { asset_id });
    }

    return result;
}

inline Image* get_image(Assets* assets, ImageID id) {
    Asset* asset = get_asset(assets, id.value);
    Image* result = 0;
    if (asset->type == AssetType_Image) {
        result = &asset->image;
    } else {
        (void)( (!!(!"Invalid Code Path")) || (_wassert(L"!\"Invalid Code Path\"", L"./pulsar_assets.cpp", (unsigned)(142)), 0) );;
    }

    return result;
}

inline Image* get_image_by_name(Assets* assets, char* name) {
    Image* result = 0;

    u32 asset_id = get_asset_id_by_name(assets, name);
    if (asset_id) {
        result = get_image(assets, { asset_id });
    }

    return result;
}

inline Font* get_font(Assets* assets, FontID id) {
    Asset* asset = get_asset(assets, id.value);
    Font* result = 0;
    if (asset->type == AssetType_Font) {
        result = &asset->font;
    } else {
        (void)( (!!(!"Invalid Code Path")) || (_wassert(L"!\"Invalid Code Path\"", L"./pulsar_assets.cpp", (unsigned)(165)), 0) );;
    }

    return result;
}

inline Font* get_font_by_name(Assets* assets, char* name) {
    Font* result = 0;

    u32 asset_id = get_asset_id_by_name(assets, name);
    if (asset_id) {
        result = get_font(assets, { asset_id });
    }

    return result;
}

inline ImageID get_glyph_id_for_codepoint(Font* font, u32 codepoint) {
    u32 glyph_table_index = codepoint - font->first_codepoint;
    u32 glyph_count = font->one_past_last_codepoint - font->first_codepoint;
    ImageID result = { 0 };
    if (codepoint >= font->first_codepoint && glyph_table_index < glyph_count) {
        result = font->glyph_table[glyph_table_index];
    }
    return result;
}

inline f32 get_advance_for_codepoint_pair(Font* font, u32 c1, u32 c2) {
    u32 g1 = c1 - font->first_codepoint;
    u32 g2 = c2 - font->first_codepoint;
    u32 glyph_count = font->one_past_last_codepoint - font->first_codepoint;
    f32 result = font->kerning_table[glyph_count*g2 + g1];
    return result;
}

inline f32 get_baseline(Font* font) {
    f32 result = font->descent;
    return result;
}

inline f32 get_baseline_from_top(Font* font) {
    f32 result = font->ascent;
    return result;
}

inline f32 get_line_spacing(Font* font) {
    f32 result = font->ascent + font->descent + font->line_gap;
    return result;
}

inline MidiTrack* get_midi(Assets* assets, MidiID id) {
    Asset* asset = get_asset(assets, id.value);
    MidiTrack* result = 0;
    if (asset->type == AssetType_Midi) {
        result = &asset->midi_track;
    } else {
        (void)( (!!(!"Invalid Code Path")) || (_wassert(L"!\"Invalid Code Path\"", L"./pulsar_assets.cpp", (unsigned)(221)), 0) );;
    }

    return result;
}

inline MidiTrack* get_midi_by_name(Assets* assets, char* name) {
    MidiTrack* result = 0;

    u32 asset_id = get_asset_id_by_name(assets, name);
    if (asset_id) {
        result = get_midi(assets, { asset_id });
    }

    return result;
}

inline Soundtrack* get_soundtrack(Assets* assets, SoundtrackID id) {
    Asset* asset = get_asset(assets, id.value);
    Soundtrack* result = 0;
    if (asset->type == AssetType_Soundtrack) {
        result = &asset->soundtrack;
    } else {
        (void)( (!!(!"Invalid Code Path")) || (_wassert(L"!\"Invalid Code Path\"", L"./pulsar_assets.cpp", (unsigned)(244)), 0) );;
    }

    return result;
}

inline Soundtrack* get_soundtrack_by_name(Assets* assets, char* name) {
    Soundtrack* result = 0;

    u32 asset_id = get_asset_id_by_name(assets, name);
    if (asset_id) {
        result = get_soundtrack(assets, { asset_id });
    }

    return result;
}
# 4 "pulsar_main.cpp" 2
# 1 "./pulsar_audio_mixer.cpp" 1
static void initialize_audio_mixer(AudioMixer* mixer, MemoryArena* permanent_arena) {
    mixer->arena = permanent_arena;
    mixer->first_playing_sound = 0;
    mixer->first_free_playing_sound = 0;
    mixer->master_volume[0] = 1.0f;
    mixer->master_volume[1] = 1.0f;
}

static PlayingSound* play_sound(AudioMixer* mixer, Sound* sound, u32 flags = 0) {
    if (!mixer->first_free_playing_sound) {
        mixer->first_free_playing_sound = (PlayingSound*)push_size_(mixer->arena, sizeof(PlayingSound));
        mixer->first_free_playing_sound->next = 0;
    }

    PlayingSound* playing_sound = mixer->first_free_playing_sound;
    mixer->first_free_playing_sound = playing_sound->next;

    playing_sound->samples_played = 0;
    playing_sound->current_volume[0] = 0.5f;
    playing_sound->current_volume[1] = 0.5f;
    playing_sound->sound = sound;
    playing_sound->flags = flags;

    playing_sound->next = mixer->first_playing_sound;
    mixer->first_playing_sound = playing_sound;

    return playing_sound;
}

static void change_volume(PlayingSound* sound, v2 target_volume) {
    sound->current_volume[0] = target_volume.e[0];
    sound->current_volume[1] = target_volume.e[1];
}



inline void change_balance(PlayingSound* sound, f32 amplitude, f32 pan) {
    f32 pow = square_root(2.0f) / 2.0f;
    f32 theta = deg_to_rad(pan);
    v2 volume = pow * vec2(my_cos(theta), my_sin(theta));
    change_volume(sound, amplitude * volume);
}

inline void stop_all_sounds(AudioMixer* mixer) {
    PlayingSound* last_playing_sound = mixer->first_playing_sound;
    while (last_playing_sound->next) {
        last_playing_sound = last_playing_sound->next;
    }
    last_playing_sound->next = mixer->first_free_playing_sound;
    mixer->first_free_playing_sound = mixer->first_playing_sound;
    mixer->first_playing_sound = 0;
}

static void output_playing_sounds(AudioMixer* mixer, GameSoundOutputBuffer* sound_buffer, MemoryArena* temp_arena) {
# 72 "./pulsar_audio_mixer.cpp"
    if (sound_buffer->sample_count > 0) {
        TemporaryMemory mixer_memory = begin_temporary_memory(temp_arena);

        u32 sample_count = sound_buffer->sample_count;

        f32* float_channel0 = (f32*)push_size_(temp_arena, sizeof(f32) * (sample_count));
        f32* float_channel1 = (f32*)push_size_(temp_arena, sizeof(f32) * (sample_count));

        f32 seconds_per_sample = 1.0f / sound_buffer->sample_rate;


        for (PlayingSound** playing_sound_ptr = &mixer->first_playing_sound; *playing_sound_ptr;) {
            PlayingSound* playing_sound = *playing_sound_ptr;
            b32 sound_finished = false;

            if (!playing_sound->sound) {
                *playing_sound_ptr = playing_sound->next;
                playing_sound->next = mixer->first_free_playing_sound;
                mixer->first_free_playing_sound = playing_sound;
            }

            u32 total_samples_to_mix = sound_buffer->sample_count;
            f32* dest0 = (f32*)float_channel0;
            f32* dest1 = (f32*)float_channel1;
            f32* dest[2] = { dest0, dest1 };
            while ((total_samples_to_mix > 0) && !sound_finished) {
                Sound* sound = playing_sound->sound;
                if (sound) {
                    v2 volume;
                    volume.e[0] = mixer->master_volume[0]*playing_sound->current_volume[0];
                    volume.e[1] = mixer->master_volume[1]*playing_sound->current_volume[1];

                    if (playing_sound->flags & Playback_Looping) {
                        if (playing_sound->samples_played >= sound->sample_count) {
                            playing_sound->samples_played -= sound->sample_count;
                        }
                    }

                    (void)( (!!(playing_sound->samples_played >= 0 && playing_sound->samples_played < sound->sample_count)) || (_wassert(L"playing_sound->samples_played >= 0 && playing_sound->samples_played < sound->sample_count", L"./pulsar_audio_mixer.cpp", (unsigned)(110)), 0) );

                    u32 samples_to_mix = total_samples_to_mix;
                    u32 samples_remaining_in_sound = sound->sample_count - playing_sound->samples_played;

                    if (!(playing_sound->flags & Playback_Looping)) {
                        if (samples_to_mix > samples_remaining_in_sound) {
                            samples_to_mix = samples_remaining_in_sound;
                        }
                    }

                    u32 begin_sample_position = playing_sound->samples_played;
                    u32 end_sample_position = begin_sample_position + samples_to_mix;
                    for (u32 sample_index = begin_sample_position; sample_index < end_sample_position; sample_index++) {
                        u32 wrapped_sample_index = sample_index;
                        if (wrapped_sample_index >= sound->sample_count) {
                            wrapped_sample_index -= sound->sample_count;
                        }

                        for (u32 channel = 0; channel < sound->channel_count; channel++) {
                            f32 sample_value = (f32)(sound->samples + sound->sample_count*channel)[wrapped_sample_index];
                            *dest[channel]++ += volume.e[channel]*sample_value;
                        }
                    }

                    playing_sound->samples_played = end_sample_position;

                    (void)( (!!(total_samples_to_mix >= samples_to_mix)) || (_wassert(L"total_samples_to_mix >= samples_to_mix", L"./pulsar_audio_mixer.cpp", (unsigned)(137)), 0) );
                    total_samples_to_mix -= samples_to_mix;

                    if (!(playing_sound->flags & Playback_Looping)) {
                        if (samples_to_mix >= samples_remaining_in_sound) {
                            sound_finished = true;
                        }
                    }
                } else {
                    break;
                }

                if (sound_finished) {
                    *playing_sound_ptr = playing_sound->next;
                    playing_sound->next = mixer->first_free_playing_sound;
                    mixer->first_free_playing_sound = playing_sound;
                } else {
                    playing_sound_ptr = &playing_sound->next;
                }
            }
        }



        f32* source0 = float_channel0;
        f32* source1 = float_channel1;
        s16* sample_out = sound_buffer->samples;
        for (u32 sample_index = 0; sample_index < sample_count; sample_index++) {
            *sample_out++ = (s16) (*source0++);
            *sample_out++ = (s16) (*source1++);
        }

        end_temporary_memory(mixer_memory);
    }
}
# 5 "pulsar_main.cpp" 2
# 1 "./pulsar_render_commands.cpp" 1

inline void* push_render_command_(GameRenderCommands* commands, RenderCommandType type, u32 render_command_size) {
    void* result = 0;
    if (commands->command_buffer_used + (render_command_size + sizeof(RenderCommandHeader)) < commands->command_buffer_size) {
        RenderCommandHeader* header = (RenderCommandHeader*) (commands->command_buffer + commands->command_buffer_used);
        header->type = (u8) type;
        commands->command_buffer_used += sizeof(*header);

        result = (commands->command_buffer + commands->command_buffer_used);
        commands->command_buffer_used += render_command_size;
    } else {
        (void)( (!!(!"Invalid Code Path")) || (_wassert(L"!\"Invalid Code Path\"", L"./pulsar_render_commands.cpp", (unsigned)(12)), 0) );;
    }
    return result;
}

inline RenderCommandClear* push_clear(GameRenderCommands* commands, v4 color) {
    RenderCommandClear* result = (RenderCommandClear*) push_render_command_(commands, RenderCommand_Clear, sizeof(RenderCommandClear));
    result->color = color;
    return result;
}

inline RenderCommandImage* push_image(GameRenderCommands* commands, Image* image, v2 p, v4 color = vec4(1, 1, 1, 1)) {
    RenderCommandImage* result = (RenderCommandImage*) push_render_command_(commands, RenderCommand_Image, sizeof(RenderCommandImage));
    result->image = image;
    result->p = p;
    result->color = color;
    return result;
}

inline RenderCommandShape* push_shape(GameRenderCommands* commands, Transform2D transform, Shape2D shape, v4 color = vec4(1, 1, 1, 1)) {
    RenderCommandShape* result = (RenderCommandShape*) push_render_command_(commands, RenderCommand_Shape, sizeof(RenderCommandShape));
    result->transform = transform;
    result->shape = shape;
    result->color = color;
    return result;
}
# 6 "pulsar_main.cpp" 2

# 1 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\stdarg.h" 1 3
# 30 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\stdarg.h" 3
typedef __builtin_va_list va_list;
# 48 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\stdarg.h" 3
typedef __builtin_va_list __gnuc_va_list;
# 8 "pulsar_main.cpp" 2



# 1 "./stb_sprintf.h" 1
# 165 "./stb_sprintf.h"
# 1 "C:\\Program Files\\LLVM\\lib\\clang\\8.0.0\\include\\stddef.h" 1 3
# 166 "./stb_sprintf.h" 2




typedef char *STBSP_SPRINTFCB(char *buf, void *user, int len);





static int stbsp_vsprintf(char *buf, char const *fmt, va_list va);
static int stbsp_vsnprintf(char *buf, int count, char const *fmt, va_list va);
static int stbsp_sprintf(char *buf, char const *fmt, ...);
static int stbsp_snprintf(char *buf, int count, char const *fmt, ...);

static int stbsp_vsprintfcb(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va);
static void stbsp_set_separators(char comma, char period);
# 224 "./stb_sprintf.h"
static signed int stbsp__real_to_str(char const **start, unsigned int *len, char *out, signed int *decimal_pos, double value, unsigned int frac_digits);
static signed int stbsp__real_to_parts(signed __int64 *bits, signed int *expo, double value);



static char stbsp__period = '.';
static char stbsp__comma = ',';
static struct
{
   short temp;
   char pair[201];
} stbsp__digitpair =
{
  0,
   "00010203040506070809101112131415161718192021222324"
   "25262728293031323334353637383940414243444546474849"
   "50515253545556575859606162636465666768697071727374"
   "75767778798081828384858687888990919293949596979899"
};

static void stbsp_set_separators(char pcomma, char pperiod)
{
   stbsp__period = pperiod;
   stbsp__comma = pcomma;
}
# 264 "./stb_sprintf.h"
static void stbsp__lead_sign(unsigned int fl, char *sign)
{
   sign[0] = 0;
   if (fl & 128) {
      sign[0] = 1;
      sign[1] = '-';
   } else if (fl & 4) {
      sign[0] = 1;
      sign[1] = ' ';
   } else if (fl & 2) {
      sign[0] = 1;
      sign[1] = '+';
   }
}

static int stbsp_vsprintfcb(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va)
{
   static char hex[] = "0123456789abcdefxp";
   static char hexu[] = "0123456789ABCDEFXP";
   char *bf;
   char const *f;
   int tlen = 0;

   bf = buf;
   f = fmt;
   for (;;) {
      signed int fw, pr, tz;
      unsigned int fl;
# 322 "./stb_sprintf.h"
      for (;;) {
         while (((unsigned __int64)f) & 3) {
         schk1:
            if (f[0] == '%')
               goto scandd;
         schk2:
            if (f[0] == 0)
               goto endfmt;
            { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
            *bf++ = f[0];
            ++f;
         }
         for (;;) {



            unsigned int v, c;
            v = *(unsigned int *)f;
            c = (~v) & 0x80808080;
            if (((v ^ 0x25252525) - 0x01010101) & c)
               goto schk1;
            if ((v - 0x01010101) & c)
               goto schk2;
            if (callback)
               if ((512 - (int)(bf - buf)) < 4)
                  goto schk1;
# 356 "./stb_sprintf.h"
            {
                *(unsigned int *)bf = v;
            }
            bf += 4;
            f += 4;
         }
      }
   scandd:

      ++f;


      fw = 0;
      pr = -1;
      fl = 0;
      tz = 0;


      for (;;) {
         switch (f[0]) {

         case '-':
            fl |= 1;
            ++f;
            continue;

         case '+':
            fl |= 2;
            ++f;
            continue;

         case ' ':
            fl |= 4;
            ++f;
            continue;

         case '#':
            fl |= 8;
            ++f;
            continue;

         case '\'':
            fl |= 64;
            ++f;
            continue;

         case '$':
            if (fl & 256) {
               if (fl & 2048) {
                  fl |= 4096;
               } else {
                  fl |= 2048;
               }
            } else {
               fl |= 256;
            }
            ++f;
            continue;

         case '_':
            fl |= 1024;
            ++f;
            continue;

         case '0':
            fl |= 16;
            ++f;
            goto flags_done;
         default: goto flags_done;
         }
      }
   flags_done:


      if (f[0] == '*') {
         fw = __builtin_va_arg(va, unsigned int);
         ++f;
      } else {
         while ((f[0] >= '0') && (f[0] <= '9')) {
            fw = fw * 10 + f[0] - '0';
            f++;
         }
      }

      if (f[0] == '.') {
         ++f;
         if (f[0] == '*') {
            pr = __builtin_va_arg(va, unsigned int);
            ++f;
         } else {
            pr = 0;
            while ((f[0] >= '0') && (f[0] <= '9')) {
               pr = pr * 10 + f[0] - '0';
               f++;
            }
         }
      }


      switch (f[0]) {

      case 'h':
         fl |= 512;
         ++f;
         break;

      case 'l':
         fl |= ((sizeof(long) == 8) ? 32 : 0);
         ++f;
         if (f[0] == 'l') {
            fl |= 32;
            ++f;
         }
         break;

      case 'j':
         fl |= (sizeof(size_t) == 8) ? 32 : 0;
         ++f;
         break;

      case 'z':
         fl |= (sizeof(ptrdiff_t) == 8) ? 32 : 0;
         ++f;
         break;
      case 't':
         fl |= (sizeof(ptrdiff_t) == 8) ? 32 : 0;
         ++f;
         break;

      case 'I':
         if ((f[1] == '6') && (f[2] == '4')) {
            fl |= 32;
            f += 3;
         } else if ((f[1] == '3') && (f[2] == '2')) {
            f += 3;
         } else {
            fl |= ((sizeof(void *) == 8) ? 32 : 0);
            ++f;
         }
         break;
      default: break;
      }


      switch (f[0]) {

         char num[512];
         char lead[8];
         char tail[8];
         char *s;
         char const *h;
         unsigned int l, n, cs;
         unsigned __int64 n64;

         double fv;

         signed int dp;
         char const *sn;

      case 's':

         s = __builtin_va_arg(va, char *);
         if (s == 0)
            s = (char *)"null";

         sn = s;
         for (;;) {
            if ((((unsigned __int64)sn) & 3) == 0)
               break;
         lchk:
            if (sn[0] == 0)
               goto ld;
            ++sn;
         }
         n = 0xffffffff;
         if (pr >= 0) {
            n = (unsigned int)(sn - s);
            if (n >= (unsigned int)pr)
               goto ld;
            n = ((unsigned int)(pr - n)) >> 2;
         }
         while (n) {
            unsigned int v = *(unsigned int *)sn;
            if ((v - 0x01010101) & (~v) & 0x80808080UL)
               goto lchk;
            sn += 4;
            --n;
         }
         goto lchk;
      ld:

         l = (unsigned int)(sn - s);

         if (l > (unsigned int)pr)
            l = pr;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;

         goto scopy;

      case 'c':

         s = num + 512 - 1;
         *s = (char)__builtin_va_arg(va, int);
         l = 1;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         goto scopy;

      case 'n':
      {
         int *d = __builtin_va_arg(va, int *);
         *d = tlen + (int)(bf - buf);
      } break;
# 595 "./stb_sprintf.h"
      case 'A':
      case 'a':
         h = (f[0] == 'A') ? hexu : hex;
         fv = __builtin_va_arg(va, double);
         if (pr == -1)
            pr = 6;

         if (stbsp__real_to_parts((signed __int64 *)&n64, &dp, fv))
            fl |= 128;

         s = num + 64;

         stbsp__lead_sign(fl, lead);

         if (dp == -1023)
            dp = (n64) ? -1022 : 0;
         else
            n64 |= (((unsigned __int64)1) << 52);
         n64 <<= (64 - 56);
         if (pr < 15)
            n64 += ((((unsigned __int64)8) << 56) >> (pr * 4));






         lead[1 + lead[0]] = '0';
         lead[2 + lead[0]] = 'x';
         lead[0] += 2;

         *s++ = h[(n64 >> 60) & 15];
         n64 <<= 4;
         if (pr)
            *s++ = stbsp__period;
         sn = s;


         n = pr;
         if (n > 13)
            n = 13;
         if (pr > (signed int)n)
            tz = pr - n;
         pr = 0;
         while (n--) {
            *s++ = h[(n64 >> 60) & 15];
            n64 <<= 4;
         }


         tail[1] = h[17];
         if (dp < 0) {
            tail[2] = '-';
            dp = -dp;
         } else
            tail[2] = '+';
         n = (dp >= 1000) ? 6 : ((dp >= 100) ? 5 : ((dp >= 10) ? 4 : 3));
         tail[0] = (char)n;
         for (;;) {
            tail[n] = '0' + dp % 10;
            if (n <= 3)
               break;
            --n;
            dp /= 10;
         }

         dp = (int)(s - sn);
         l = (int)(s - (num + 64));
         s = num + 64;
         cs = 1 + (3 << 24);
         goto scopy;

      case 'G':
      case 'g':
         h = (f[0] == 'G') ? hexu : hex;
         fv = __builtin_va_arg(va, double);
         if (pr == -1)
            pr = 6;
         else if (pr == 0)
            pr = 1;

         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, (pr - 1) | 0x80000000))
            fl |= 128;


         n = pr;
         if (l > (unsigned int)pr)
            l = pr;
         while ((l > 1) && (pr) && (sn[l - 1] == '0')) {
            --pr;
            --l;
         }


         if ((dp <= -4) || (dp > (signed int)n)) {
            if (pr > (signed int)l)
               pr = l - 1;
            else if (pr)
               --pr;
            goto doexpfromg;
         }

         if (dp > 0) {
            pr = (dp < (signed int)l) ? l - dp : 0;
         } else {
            pr = -dp + ((pr > (signed int)l) ? (signed int) l : pr);
         }
         goto dofloatfromg;

      case 'E':
      case 'e':
         h = (f[0] == 'E') ? hexu : hex;
         fv = __builtin_va_arg(va, double);
         if (pr == -1)
            pr = 6;

         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, pr | 0x80000000))
            fl |= 128;
      doexpfromg:
         tail[0] = 0;
         stbsp__lead_sign(fl, lead);
         if (dp == 0x7000) {
            s = (char *)sn;
            cs = 0;
            pr = 0;
            goto scopy;
         }
         s = num + 64;

         *s++ = sn[0];

         if (pr)
            *s++ = stbsp__period;


         if ((l - 1) > (unsigned int)pr)
            l = pr + 1;
         for (n = 1; n < l; n++)
            *s++ = sn[n];

         tz = pr - (l - 1);
         pr = 0;

         tail[1] = h[0xe];
         dp -= 1;
         if (dp < 0) {
            tail[2] = '-';
            dp = -dp;
         } else
            tail[2] = '+';



         n = (dp >= 100) ? 5 : 4;

         tail[0] = (char)n;
         for (;;) {
            tail[n] = '0' + dp % 10;
            if (n <= 3)
               break;
            --n;
            dp /= 10;
         }
         cs = 1 + (3 << 24);
         goto flt_lead;

      case 'f':
         fv = __builtin_va_arg(va, double);
      doafloat:

         if (fl & 256) {
            double divisor;
            divisor = 1000.0f;
            if (fl & 2048)
               divisor = 1024.0;
            while (fl < 0x4000000) {
               if ((fv < divisor) && (fv > -divisor))
                  break;
               fv /= divisor;
               fl += 0x1000000;
            }
         }
         if (pr == -1)
            pr = 6;

         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, pr))
            fl |= 128;
      dofloatfromg:
         tail[0] = 0;
         stbsp__lead_sign(fl, lead);
         if (dp == 0x7000) {
            s = (char *)sn;
            cs = 0;
            pr = 0;
            goto scopy;
         }
         s = num + 64;


         if (dp <= 0) {
            signed int i;

            *s++ = '0';
            if (pr)
               *s++ = stbsp__period;
            n = -dp;
            if ((signed int)n > pr)
               n = pr;
            i = n;
            while (i) {
               if ((((unsigned __int64)s) & 3) == 0)
                  break;
               *s++ = '0';
               --i;
            }
            while (i >= 4) {
               *(unsigned int *)s = 0x30303030;
               s += 4;
               i -= 4;
            }
            while (i) {
               *s++ = '0';
               --i;
            }
            if ((signed int)(l + n) > pr)
               l = pr - n;
            i = l;
            while (i) {
               *s++ = *sn++;
               --i;
            }
            tz = pr - (n + l);
            cs = 1 + (3 << 24);
         } else {
            cs = (fl & 64) ? ((600 - (unsigned int)dp) % 3) : 0;
            if ((unsigned int)dp >= l) {

               n = 0;
               for (;;) {
                  if ((fl & 64) && (++cs == 4)) {
                     cs = 0;
                     *s++ = stbsp__comma;
                  } else {
                     *s++ = sn[n];
                     ++n;
                     if (n >= l)
                        break;
                  }
               }
               if (n < (unsigned int)dp) {
                  n = dp - n;
                  if ((fl & 64) == 0) {
                     while (n) {
                        if ((((unsigned __int64)s) & 3) == 0)
                           break;
                        *s++ = '0';
                        --n;
                     }
                     while (n >= 4) {
                        *(unsigned int *)s = 0x30303030;
                        s += 4;
                        n -= 4;
                     }
                  }
                  while (n) {
                     if ((fl & 64) && (++cs == 4)) {
                        cs = 0;
                        *s++ = stbsp__comma;
                     } else {
                        *s++ = '0';
                        --n;
                     }
                  }
               }
               cs = (int)(s - (num + 64)) + (3 << 24);
               if (pr) {
                  *s++ = stbsp__period;
                  tz = pr;
               }
            } else {

               n = 0;
               for (;;) {
                  if ((fl & 64) && (++cs == 4)) {
                     cs = 0;
                     *s++ = stbsp__comma;
                  } else {
                     *s++ = sn[n];
                     ++n;
                     if (n >= (unsigned int)dp)
                        break;
                  }
               }
               cs = (int)(s - (num + 64)) + (3 << 24);
               if (pr)
                  *s++ = stbsp__period;
               if ((l - dp) > (unsigned int)pr)
                  l = pr + dp;
               while (n < l) {
                  *s++ = sn[n];
                  ++n;
               }
               tz = pr - (l - dp);
            }
         }
         pr = 0;


         if (fl & 256) {
            char idx;
            idx = 1;
            if (fl & 1024)
               idx = 0;
            tail[0] = idx;
            tail[1] = ' ';
            {
               if (fl >> 24) {
                  if (fl & 2048)
                     tail[idx + 1] = "_KMGT"[fl >> 24];
                  else
                     tail[idx + 1] = "_kMGT"[fl >> 24];
                  idx++;

                  if (fl & 2048 && !(fl & 4096)) {
                     tail[idx + 1] = 'i';
                     idx++;
                  }
                  tail[0] = idx;
               }
            }
         };

      flt_lead:

         l = (unsigned int)(s - (num + 64));
         s = num + 64;
         goto scopy;


      case 'B':
      case 'b':
         h = (f[0] == 'B') ? hexu : hex;
         lead[0] = 0;
         if (fl & 8) {
            lead[0] = 2;
            lead[1] = '0';
            lead[2] = h[0xb];
         }
         l = (8 << 4) | (1 << 8);
         goto radixnum;

      case 'o':
         h = hexu;
         lead[0] = 0;
         if (fl & 8) {
            lead[0] = 1;
            lead[1] = '0';
         }
         l = (3 << 4) | (3 << 8);
         goto radixnum;

      case 'p':
         fl |= (sizeof(void *) == 8) ? 32 : 0;
         pr = sizeof(void *) * 2;
         fl &= ~16;


      case 'X':
      case 'x':
         h = (f[0] == 'X') ? hexu : hex;
         l = (4 << 4) | (4 << 8);
         lead[0] = 0;
         if (fl & 8) {
            lead[0] = 2;
            lead[1] = '0';
            lead[2] = h[16];
         }
      radixnum:

         if (fl & 32)
            n64 = __builtin_va_arg(va, unsigned __int64);
         else
            n64 = __builtin_va_arg(va, unsigned int);

         s = num + 512;
         dp = 0;

         tail[0] = 0;
         if (n64 == 0) {
            lead[0] = 0;
            if (pr == 0) {
               l = 0;
               cs = (((l >> 4) & 15)) << 24;
               goto scopy;
            }
         }

         for (;;) {
            *--s = h[n64 & ((1 << (l >> 8)) - 1)];
            n64 >>= (l >> 8);
            if (!((n64) || ((signed int)((num + 512) - s) < pr)))
               break;
            if (fl & 64) {
               ++l;
               if ((l & 15) == ((l >> 4) & 15)) {
                  l &= ~15;
                  *--s = stbsp__comma;
               }
            }
         };

         cs = (unsigned int)((num + 512) - s) + ((((l >> 4) & 15)) << 24);

         l = (unsigned int)((num + 512) - s);

         goto scopy;

      case 'u':
      case 'i':
      case 'd':

         if (fl & 32) {
            signed __int64 i64 = __builtin_va_arg(va, signed __int64);
            n64 = (unsigned __int64)i64;
            if ((f[0] != 'u') && (i64 < 0)) {
               n64 = (unsigned __int64)-i64;
               fl |= 128;
            }
         } else {
            signed int i = __builtin_va_arg(va, signed int);
            n64 = (unsigned int)i;
            if ((f[0] != 'u') && (i < 0)) {
               n64 = (unsigned int)-i;
               fl |= 128;
            }
         }


         if (fl & 256) {
            if (n64 < 1024)
               pr = 0;
            else if (pr == -1)
               pr = 1;
            fv = (double)(signed __int64)n64;
            goto doafloat;
         }



         s = num + 512;
         l = 0;

         for (;;) {

            char *o = s - 8;
            if (n64 >= 100000000) {
               n = (unsigned int)(n64 % 100000000);
               n64 /= 100000000;
            } else {
               n = (unsigned int)n64;
               n64 = 0;
            }
            if ((fl & 64) == 0) {
               do {
                  s -= 2;
                  *(unsigned short *)s = *(unsigned short *)&stbsp__digitpair.pair[(n % 100) * 2];
                  n /= 100;
               } while (n);
            }
            while (n) {
               if ((fl & 64) && (l++ == 3)) {
                  l = 0;
                  *--s = stbsp__comma;
                  --o;
               } else {
                  *--s = (char)(n % 10) + '0';
                  n /= 10;
               }
            }
            if (n64 == 0) {
               if ((s[0] == '0') && (s != (num + 512)))
                  ++s;
               break;
            }
            while (s != o)
               if ((fl & 64) && (l++ == 3)) {
                  l = 0;
                  *--s = stbsp__comma;
                  --o;
               } else {
                  *--s = '0';
               }
         }

         tail[0] = 0;
         stbsp__lead_sign(fl, lead);


         l = (unsigned int)((num + 512) - s);
         if (l == 0) {
            *--s = '0';
            l = 1;
         }
         cs = l + (3 << 24);
         if (pr < 0)
            pr = 0;

      scopy:

         if (pr < (signed int)l)
            pr = l;
         n = pr + lead[0] + tail[0] + tz;
         if (fw < (signed int)n)
            fw = n;
         fw -= n;
         pr -= l;


         if ((fl & 1) == 0) {
            if (fl & 16)
            {
               pr = (fw > pr) ? fw : pr;
               fw = 0;
            } else {
               fl &= ~64;
            }
         }


         if (fw + pr) {
            signed int i;
            unsigned int c;


            if ((fl & 1) == 0)
               while (fw > 0) {
                  i = fw; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
                  fw -= i;
                  while (i) {
                     if ((((unsigned __int64)bf) & 3) == 0)
                        break;
                     *bf++ = ' ';
                     --i;
                  }
                  while (i >= 4) {
                     *(unsigned int *)bf = 0x20202020;
                     bf += 4;
                     i -= 4;
                  }
                  while (i) {
                     *bf++ = ' ';
                     --i;
                  }
                  { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
               }


            sn = lead + 1;
            while (lead[0]) {
               i = lead[0]; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
               lead[0] -= (char)i;
               while (i) {
                  *bf++ = *sn++;
                  --i;
               }
               { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
            }


            c = cs >> 24;
            cs &= 0xffffff;
            cs = (fl & 64) ? ((unsigned int)(c - ((pr + cs) % (c + 1)))) : 0;
            while (pr > 0) {
               i = pr; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
               pr -= i;
               if ((fl & 64) == 0) {
                  while (i) {
                     if ((((unsigned __int64)bf) & 3) == 0)
                        break;
                     *bf++ = '0';
                     --i;
                  }
                  while (i >= 4) {
                     *(unsigned int *)bf = 0x30303030;
                     bf += 4;
                     i -= 4;
                  }
               }
               while (i) {
                  if ((fl & 64) && (cs++ == c)) {
                     cs = 0;
                     *bf++ = stbsp__comma;
                  } else
                     *bf++ = '0';
                  --i;
               }
               { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
            }
         }


         sn = lead + 1;
         while (lead[0]) {
            signed int i;
            i = lead[0]; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
            lead[0] -= (char)i;
            while (i) {
               *bf++ = *sn++;
               --i;
            }
            { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
         }


         n = l;
         while (n) {
            signed int i;
            i = n; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
            n -= i;
            while (i >= 4) { *(unsigned int *)bf = *(unsigned int *)s; bf += 4; s += 4; i -= 4; }





            while (i) {
               *bf++ = *s++;
               --i;
            }
            { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
         }


         while (tz) {
            signed int i;
            i = tz; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
            tz -= i;
            while (i) {
               if ((((unsigned __int64)bf) & 3) == 0)
                  break;
               *bf++ = '0';
               --i;
            }
            while (i >= 4) {
               *(unsigned int *)bf = 0x30303030;
               bf += 4;
               i -= 4;
            }
            while (i) {
               *bf++ = '0';
               --i;
            }
            { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
         }


         sn = tail + 1;
         while (tail[0]) {
            signed int i;
            i = tail[0]; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
            tail[0] -= (char)i;
            while (i) {
               *bf++ = *sn++;
               --i;
            }
            { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
         }


         if (fl & 1)
            if (fw > 0) {
               while (fw) {
                  signed int i;
                  i = fw; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
                  fw -= i;
                  while (i) {
                     if ((((unsigned __int64)bf) & 3) == 0)
                        break;
                     *bf++ = ' ';
                     --i;
                  }
                  while (i >= 4) {
                     *(unsigned int *)bf = 0x20202020;
                     bf += 4;
                     i -= 4;
                  }
                  while (i--)
                     *bf++ = ' ';
                  { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
               }
            }
         break;

      default:
         s = num + 512 - 1;
         *s = f[0];
         l = 1;
         fw = fl = 0;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         goto scopy;
      }
      ++f;
   }
endfmt:

   if (!callback)
      *bf = 0;
   else
      { { int len = (int)(bf - buf); if ((len + (512 - 1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; };

done:
   return tlen + (int)(bf - buf);
}
# 1332 "./stb_sprintf.h"
static int stbsp_sprintf(char *buf, char const *fmt, ...)
{
   int result;
   va_list va;
   __builtin_va_start(va, fmt);
   result = stbsp_vsprintfcb(0, 0, buf, fmt, va);
   __builtin_va_end(va);
   return result;
}

typedef struct stbsp__context {
   char *buf;
   int count;
   char tmp[512];
} stbsp__context;

static char *stbsp__clamp_callback(char *buf, void *user, int len)
{
   stbsp__context *c = (stbsp__context *)user;

   if (len > c->count)
      len = c->count;

   if (len) {
      if (buf != c->buf) {
         char *s, *d, *se;
         d = c->buf;
         s = buf;
         se = buf + len;
         do {
            *d++ = *s++;
         } while (s < se);
      }
      c->buf += len;
      c->count -= len;
   }

   if (c->count <= 0)
      return 0;
   return (c->count >= 512) ? c->buf : c->tmp;
}

static char * stbsp__count_clamp_callback( char * buf, void * user, int len )
{
   stbsp__context * c = (stbsp__context*)user;

   c->count += len;
   return c->tmp;
}

static int stbsp_vsnprintf( char * buf, int count, char const * fmt, va_list va )
{
   stbsp__context c;
   int l;

   if ( (count == 0) && !buf )
   {
      c.count = 0;

      stbsp_vsprintfcb( stbsp__count_clamp_callback, &c, c.tmp, fmt, va );
      l = c.count;
   }
   else
   {
      if ( count == 0 )
         return 0;

      c.buf = buf;
      c.count = count;

      stbsp_vsprintfcb( stbsp__clamp_callback, &c, stbsp__clamp_callback(0,&c,0), fmt, va );


      l = (int)( c.buf - buf );
      if ( l >= count )
         l = count - 1;
      buf[l] = 0;
   }

   return l;
}

static int stbsp_snprintf(char *buf, int count, char const *fmt, ...)
{
   int result;
   va_list va;
   __builtin_va_start(va, fmt);

   result = stbsp_vsnprintf(buf, count, fmt, va);
   __builtin_va_end(va);

   return result;
}

static int stbsp_vsprintf(char *buf, char const *fmt, va_list va)
{
   return stbsp_vsprintfcb(0, 0, buf, fmt, va);
}
# 1445 "./stb_sprintf.h"
static signed int stbsp__real_to_parts(signed __int64 *bits, signed int *expo, double value)
{
   double d;
   signed __int64 b = 0;


   d = value;

   { int cn; for (cn = 0; cn < 8; cn++) ((char *)&b)[cn] = ((char *)&d)[cn]; };

   *bits = b & ((((unsigned __int64)1) << 52) - 1);
   *expo = (signed int)(((b >> 52) & 2047) - 1023);

   return (signed int)((unsigned __int64) b >> 63);
}

static double const stbsp__bot[23] = {
   1e+000, 1e+001, 1e+002, 1e+003, 1e+004, 1e+005, 1e+006, 1e+007, 1e+008, 1e+009, 1e+010, 1e+011,
   1e+012, 1e+013, 1e+014, 1e+015, 1e+016, 1e+017, 1e+018, 1e+019, 1e+020, 1e+021, 1e+022
};
static double const stbsp__negbot[22] = {
   1e-001, 1e-002, 1e-003, 1e-004, 1e-005, 1e-006, 1e-007, 1e-008, 1e-009, 1e-010, 1e-011,
   1e-012, 1e-013, 1e-014, 1e-015, 1e-016, 1e-017, 1e-018, 1e-019, 1e-020, 1e-021, 1e-022
};
static double const stbsp__negboterr[22] = {
   -5.551115123125783e-018, -2.0816681711721684e-019, -2.0816681711721686e-020, -4.7921736023859299e-021, -8.1803053914031305e-022, 4.5251888174113741e-023,
   4.5251888174113739e-024, -2.0922560830128471e-025, -6.2281591457779853e-026, -3.6432197315497743e-027, 6.0503030718060191e-028, 2.0113352370744385e-029,
   -3.0373745563400371e-030, 1.1806906454401013e-032, -7.7705399876661076e-032, 2.0902213275965398e-033, -7.1542424054621921e-034, -7.1542424054621926e-035,
   2.4754073164739869e-036, 5.4846728545790429e-037, 9.2462547772103625e-038, -4.8596774326570872e-039
};
static double const stbsp__top[13] = {
   1e+023, 1e+046, 1e+069, 1e+092, 1e+115, 1e+138, 1e+161, 1e+184, 1e+207, 1e+230, 1e+253, 1e+276, 1e+299
};
static double const stbsp__negtop[13] = {
   1e-023, 1e-046, 1e-069, 1e-092, 1e-115, 1e-138, 1e-161, 1e-184, 1e-207, 1e-230, 1e-253, 1e-276, 1e-299
};
static double const stbsp__toperr[13] = {
   8388608,
   6.8601809640529717e+028,
   -7.253143638152921e+052,
   -4.3377296974619174e+075,
   -1.5559416129466825e+098,
   -3.2841562489204913e+121,
   -3.7745893248228135e+144,
   -1.7356668416969134e+167,
   -3.8893577551088374e+190,
   -9.9566444326005119e+213,
   6.3641293062232429e+236,
   -5.2069140800249813e+259,
   -5.2504760255204387e+282
};
static double const stbsp__negtoperr[13] = {
   3.9565301985100693e-040, -2.299904345391321e-063, 3.6506201437945798e-086, 1.1875228833981544e-109,
   -5.0644902316928607e-132, -6.7156837247865426e-155, -2.812077463003139e-178, -5.7778912386589953e-201,
   7.4997100559334532e-224, -4.6439668915134491e-247, -6.3691100762962136e-270, -9.436808465446358e-293,
   8.0970921678014997e-317
};
# 1528 "./stb_sprintf.h"
static unsigned __int64 const stbsp__powten[20] = {
   1,
   10,
   100,
   1000,
   10000,
   100000,
   1000000,
   10000000,
   100000000,
   1000000000,
   10000000000ULL,
   100000000000ULL,
   1000000000000ULL,
   10000000000000ULL,
   100000000000000ULL,
   1000000000000000ULL,
   10000000000000000ULL,
   100000000000000000ULL,
   1000000000000000000ULL,
   10000000000000000000ULL
};
# 1592 "./stb_sprintf.h"
static void stbsp__raise_to_power10(double *ohi, double *olo, double d, signed int power)
{
   double ph, pl;
   if ((power >= 0) && (power <= 22)) {
      { double ahi = 0, alo, bhi = 0, blo; signed __int64 bt; ph = d * stbsp__bot[power]; { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bt)[cn] = ((char *)&d)[cn]; }; bt &= ((~(unsigned __int64)0) << 27); { int cn; for (cn = 0; cn < 8; cn++) ((char *)&ahi)[cn] = ((char *)&bt)[cn]; }; alo = d - ahi; { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bt)[cn] = ((char *)&stbsp__bot[power])[cn]; }; bt &= ((~(unsigned __int64)0) << 27); { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bhi)[cn] = ((char *)&bt)[cn]; }; blo = stbsp__bot[power] - bhi; pl = ((ahi * bhi - ph) + ahi * blo + alo * bhi) + alo * blo; };
   } else {
      signed int e, et, eb;
      double p2h, p2l;

      e = power;
      if (power < 0)
         e = -e;
      et = (e * 0x2c9) >> 14;
      if (et > 13)
         et = 13;
      eb = e - (et * 23);

      ph = d;
      pl = 0.0;
      if (power < 0) {
         if (eb) {
            --eb;
            { double ahi = 0, alo, bhi = 0, blo; signed __int64 bt; ph = d * stbsp__negbot[eb]; { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bt)[cn] = ((char *)&d)[cn]; }; bt &= ((~(unsigned __int64)0) << 27); { int cn; for (cn = 0; cn < 8; cn++) ((char *)&ahi)[cn] = ((char *)&bt)[cn]; }; alo = d - ahi; { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bt)[cn] = ((char *)&stbsp__negbot[eb])[cn]; }; bt &= ((~(unsigned __int64)0) << 27); { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bhi)[cn] = ((char *)&bt)[cn]; }; blo = stbsp__negbot[eb] - bhi; pl = ((ahi * bhi - ph) + ahi * blo + alo * bhi) + alo * blo; };
            pl = pl + (d * stbsp__negboterr[eb]);;
         }
         if (et) {
            { double s; s = ph + pl; pl = pl - (s - ph); ph = s; };
            --et;
            { double ahi = 0, alo, bhi = 0, blo; signed __int64 bt; p2h = ph * stbsp__negtop[et]; { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bt)[cn] = ((char *)&ph)[cn]; }; bt &= ((~(unsigned __int64)0) << 27); { int cn; for (cn = 0; cn < 8; cn++) ((char *)&ahi)[cn] = ((char *)&bt)[cn]; }; alo = ph - ahi; { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bt)[cn] = ((char *)&stbsp__negtop[et])[cn]; }; bt &= ((~(unsigned __int64)0) << 27); { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bhi)[cn] = ((char *)&bt)[cn]; }; blo = stbsp__negtop[et] - bhi; p2l = ((ahi * bhi - p2h) + ahi * blo + alo * bhi) + alo * blo; };
            p2l = p2l + (ph * stbsp__negtoperr[et] + pl * stbsp__negtop[et]);;
            ph = p2h;
            pl = p2l;
         }
      } else {
         if (eb) {
            e = eb;
            if (eb > 22)
               eb = 22;
            e -= eb;
            { double ahi = 0, alo, bhi = 0, blo; signed __int64 bt; ph = d * stbsp__bot[eb]; { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bt)[cn] = ((char *)&d)[cn]; }; bt &= ((~(unsigned __int64)0) << 27); { int cn; for (cn = 0; cn < 8; cn++) ((char *)&ahi)[cn] = ((char *)&bt)[cn]; }; alo = d - ahi; { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bt)[cn] = ((char *)&stbsp__bot[eb])[cn]; }; bt &= ((~(unsigned __int64)0) << 27); { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bhi)[cn] = ((char *)&bt)[cn]; }; blo = stbsp__bot[eb] - bhi; pl = ((ahi * bhi - ph) + ahi * blo + alo * bhi) + alo * blo; };
            if (e) {
               { double s; s = ph + pl; pl = pl - (s - ph); ph = s; };
               { double ahi = 0, alo, bhi = 0, blo; signed __int64 bt; p2h = ph * stbsp__bot[e]; { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bt)[cn] = ((char *)&ph)[cn]; }; bt &= ((~(unsigned __int64)0) << 27); { int cn; for (cn = 0; cn < 8; cn++) ((char *)&ahi)[cn] = ((char *)&bt)[cn]; }; alo = ph - ahi; { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bt)[cn] = ((char *)&stbsp__bot[e])[cn]; }; bt &= ((~(unsigned __int64)0) << 27); { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bhi)[cn] = ((char *)&bt)[cn]; }; blo = stbsp__bot[e] - bhi; p2l = ((ahi * bhi - p2h) + ahi * blo + alo * bhi) + alo * blo; };
               p2l = p2l + (stbsp__bot[e] * pl);;
               ph = p2h;
               pl = p2l;
            }
         }
         if (et) {
            { double s; s = ph + pl; pl = pl - (s - ph); ph = s; };
            --et;
            { double ahi = 0, alo, bhi = 0, blo; signed __int64 bt; p2h = ph * stbsp__top[et]; { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bt)[cn] = ((char *)&ph)[cn]; }; bt &= ((~(unsigned __int64)0) << 27); { int cn; for (cn = 0; cn < 8; cn++) ((char *)&ahi)[cn] = ((char *)&bt)[cn]; }; alo = ph - ahi; { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bt)[cn] = ((char *)&stbsp__top[et])[cn]; }; bt &= ((~(unsigned __int64)0) << 27); { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bhi)[cn] = ((char *)&bt)[cn]; }; blo = stbsp__top[et] - bhi; p2l = ((ahi * bhi - p2h) + ahi * blo + alo * bhi) + alo * blo; };
            p2l = p2l + (ph * stbsp__toperr[et] + pl * stbsp__top[et]);;
            ph = p2h;
            pl = p2l;
         }
      }
   }
   { double s; s = ph + pl; pl = pl - (s - ph); ph = s; };
   *ohi = ph;
   *olo = pl;
}





static signed int stbsp__real_to_str(char const **start, unsigned int *len, char *out, signed int *decimal_pos, double value, unsigned int frac_digits)
{
   double d;
   signed __int64 bits = 0;
   signed int expo, e, ng, tens;

   d = value;
   { int cn; for (cn = 0; cn < 8; cn++) ((char *)&bits)[cn] = ((char *)&d)[cn]; };
   expo = (signed int)((bits >> 52) & 2047);
   ng = (signed int)((unsigned __int64) bits >> 63);
   if (ng)
      d = -d;

   if (expo == 2047)
   {
      *start = (bits & ((((unsigned __int64)1) << 52) - 1)) ? "NaN" : "Inf";
      *decimal_pos = 0x7000;
      *len = 3;
      return ng;
   }

   if (expo == 0)
   {
      if ((bits << 1) == 0)
      {
         *decimal_pos = 1;
         *start = out;
         out[0] = '0';
         *len = 1;
         return ng;
      }

      {
         signed __int64 v = ((unsigned __int64)1) << 51;
         while ((bits & v) == 0) {
            --expo;
            v >>= 1;
         }
      }
   }


   {
      double ph, pl;


      tens = expo - 1023;
      tens = (tens < 0) ? ((tens * 617) / 2048) : (((tens * 1233) / 4096) + 1);


      stbsp__raise_to_power10(&ph, &pl, d, 18 - tens);


      { double ahi = 0, alo, vh, t; bits = (signed __int64)ph; vh = (double)bits; ahi = (ph - vh); t = (ahi - ph); alo = (ph - (ahi - t)) - (vh + t); bits += (signed __int64)(ahi + alo + pl); };


      if (((unsigned __int64)bits) >= (1000000000000000000ULL))
         ++tens;
   }


   frac_digits = (frac_digits & 0x80000000) ? ((frac_digits & 0x7ffffff) + 1) : (tens + frac_digits);
   if ((frac_digits < 24)) {
      unsigned int dg = 1;
      if ((unsigned __int64)bits >= stbsp__powten[9])
         dg = 10;
      while ((unsigned __int64)bits >= stbsp__powten[dg]) {
         ++dg;
         if (dg == 20)
            goto noround;
      }
      if (frac_digits < dg) {
         unsigned __int64 r;

         e = dg - frac_digits;
         if ((unsigned int)e >= 24)
            goto noround;
         r = stbsp__powten[e];
         bits = bits + (r / 2);
         if ((unsigned __int64)bits >= stbsp__powten[dg])
            ++tens;
         bits /= r;
      }
   noround:;
   }


   if (bits) {
      unsigned int n;
      for (;;) {
         if (bits <= 0xffffffff)
            break;
         if (bits % 1000)
            goto donez;
         bits /= 1000;
      }
      n = (unsigned int)bits;
      while ((n % 1000) == 0)
         n /= 1000;
      bits = n;
   donez:;
   }


   out += 64;
   e = 0;
   for (;;) {
      unsigned int n;
      char *o = out - 8;

      if (bits >= 100000000) {
         n = (unsigned int)(bits % 100000000);
         bits /= 100000000;
      } else {
         n = (unsigned int)bits;
         bits = 0;
      }
      while (n) {
         out -= 2;
         *(unsigned short *)out = *(unsigned short *)&stbsp__digitpair.pair[(n % 100) * 2];
         n /= 100;
         e += 2;
      }
      if (bits == 0) {
         if ((e) && (out[0] == '0')) {
            ++out;
            --e;
         }
         break;
      }
      while (out != o) {
         *--out = '0';
         ++e;
      }
   }

   *decimal_pos = tens;
   *start = out;
   *len = e;
   return ng;
}
# 12 "pulsar_main.cpp" 2



inline v2 get_furthest_point_along(Transform2D t, Shape2D s, v2 d) {
    v2 result = vec2(0, 0);

    switch (s.type) {
        case Shape_Polygon: {
            if (s.vert_count > 0) {
                f32 best_dist = dot(s.vertices[0], d);
                result = s.vertices[0];
                for (u32 vertex_index = 1; vertex_index < s.vert_count; vertex_index++) {
                    v2 vertex = s.vertices[vertex_index];
                    f32 dist_along_d = dot(vertex, d);
                    if (dist_along_d > best_dist) {
                        result = vertex;
                        best_dist = dist_along_d;
                    }
                }
            }
        } break;

        case Shape_Circle: {
            result = normalize_or_zero(d)*s.radius;
        } break;

        case Shape_Rectangle: {
            (void)( (!!(!"Not Implemented")) || (_wassert(L"!\"Not Implemented\"", L"pulsar_main.cpp", (unsigned)(39)), 0) );;
        } break;
    }

    result = rotate(t.scale*result, t.rotation_arm) + t.offset;

    if (dot(rotate_clockwise(t.sweep, t.rotation_arm), d) > 0) {
        result += t.sweep;
    }

    return result;
}

inline v2 support(Transform2D t1, Shape2D s1, Transform2D t2, Shape2D s2, v2 d) {
    v2 a = get_furthest_point_along(t1, s1, rotate_clockwise(d, t1.rotation_arm));
    v2 b = get_furthest_point_along(t2, s2, -rotate_clockwise(d, t2.rotation_arm));
    v2 p = a - b;
    return p;
}

inline void dbg_draw_shape(Transform2D transform, Shape2D shape, v4 color = vec4(1, 1, 1, 1)) {
    glBegin(GL_LINE_LOOP);
    glColor4fv(color.e);

    u32 circle_quality = 1024;

    switch (shape.type) {
        case Shape_Polygon: {
            for (u32 vertex_index = 0; vertex_index < shape.vert_count; vertex_index++) {
                v2 v = rotate(transform.scale*shape.vertices[vertex_index], transform.rotation_arm) + transform.offset;
                glVertex2fv(v.e);
            }
        } break;

        case Shape_Circle: {
            for (f32 segment_angle = 0; segment_angle < 6.28318530717f; segment_angle += (6.28318530717f / (f32) circle_quality)) {
                v2 v = rotate(transform.scale*vec2(my_sin(segment_angle), my_cos(segment_angle))*shape.radius, transform.rotation_arm) + transform.offset;
                glVertex2fv(v.e);
            }
        } break;
    }

    glEnd();
}

inline void dbg_draw_shape_with_brute_force(Transform2D transform, Shape2D shape, v4 color = vec4(1, 1, 1, 1)) {
    glBegin(GL_LINE_LOOP);
    glColor4fv(color.e);

    f32 quality = 1024.0f;
    v2 previous_p = vec2(3.40282347e+38F, 3.40282347e+38F);
    for (f32 segment_angle = 0; segment_angle < 6.28318530717f; segment_angle += (6.28318530717f / quality)) {
        v2 p = get_furthest_point_along(transform, shape, arm2(segment_angle));
        if (!vectors_equal(p, previous_p)) {
            previous_p = p;
            glVertex2fv(p.e);
        }
    }

    glEnd();
}

inline void dbg_draw_minkowski_difference_with_brute_force(Transform2D t1, Shape2D s1, Transform2D t2, Shape2D s2, v4 color = vec4(1, 1, 1, 1)) {
    glBegin(GL_LINE_LOOP);
    glColor4fv(color.e);

    f32 quality = 4096.0f;
    v2 previous_p = vec2(3.40282347e+38F, 3.40282347e+38F);
    for (f32 segment_angle = 0; segment_angle < 6.28318530717f; segment_angle += (6.28318530717f / quality)) {
        v2 p = support(t1, s1, t2, s2, arm2(segment_angle));
        if (!vectors_equal(p, previous_p)) {
            previous_p = p;
            p += t1.offset;
            glVertex2fv(p.e);
        }
    }

    glEnd();
}

inline v2 triple_product(v2 a, v2 b, v2 c) {

    v2 result = b*dot(c, a) - a*(dot(c, b));
    return result;
}

inline b32 gjk_do_simplex(u32* pc, v2* p, v2* d) {
    b32 result = false;

    v2 a = p[*pc - 1];
    v2 ao = -a;

    if (*pc == 2) {
        v2 b = p[0];
        v2 ab = b - a;
        *d = triple_product(ab, ao, ab);
    } else if (*pc == 3) {
        v2 b = p[1];
        v2 c = p[0];
        v2 ab = b - a;
        v2 ac = c - a;
        v2 ab_perp = triple_product(ac, ab, ab);
        v2 ac_perp = triple_product(ab, ac, ac);
        if (dot(ab_perp, ao) > 0) {
            *pc = 2;
            p[1] = a;
            p[0] = b;
            *d = ab_perp;
        } else {
            if (dot(ac_perp, ao) > 0) {
                *pc = 2;
                p[1] = a;
                p[0] = c;
                *d = ac_perp;
            } else {
                result = true;
            }
        }
    } else {
        (void)( (!!(!"Invalid Code Path")) || (_wassert(L"!\"Invalid Code Path\"", L"pulsar_main.cpp", (unsigned)(158)), 0) );;
    }

    return result;
}

struct EpaPoint {
    v2 p;
    EpaPoint* next;
};

struct EpaEdge {
    EpaPoint* point;
    v2 normal;
    f32 distance;
};

inline EpaEdge epa_find_closest_edge(EpaPoint* p_sentinel, b32 simplex_wound_ccw) {
    EpaEdge result = {};
    result.distance = 3.40282347e+38F;

    for (EpaPoint* p = p_sentinel; p; p = p->next) {
        EpaPoint* next_p = p->next ? p->next : p_sentinel;
        v2 a = p->p;
        v2 b = next_p->p;
        v2 e = b - a;
        v2 n = simplex_wound_ccw ? perp(e) : perp_clockwise(e);
        n = normalize_or_zero(n);
        f32 d = dot(n, a);
        if (d < result.distance) {
            result.point = p;
            result.normal = n;
            result.distance = d;
        }
    }

    return result;
}

struct CollisionInfo {
    v2 vector;
    f32 depth;
};

inline b32 gjk_intersect(Transform2D t1, Shape2D s1, Transform2D t2, Shape2D s2, CollisionInfo* info = 0, MemoryArena* temp_arena = 0) {
    b32 result = false;





    TemporaryMemory temp = begin_temporary_memory(temp_arena);

    u32 pc = 0;
    v2 p[3] = {};
    v2 s = support(t1, s1, t2, s2, vec2(0, 1));

    p[pc++] = s;
    v2 d = -s;
    for (;;) {
        v2 a = support(t1, s1, t2, s2, d);
        if (dot(a, d) <= 0) {
            break;
        }
        p[pc++] = a;
# 231 "pulsar_main.cpp"
        if (gjk_do_simplex(&pc, p, &d)) {
            result = true;
            if (!info || pc == 3) {
                break;
            }
        }
    }




    if (info && result) {

        (void)( (!!(pc == 3)) || (_wassert(L"pc == 3", L"pulsar_main.cpp", (unsigned)(244)), 0) );
        (void)( (!!(temp_arena)) || (_wassert(L"temp_arena", L"pulsar_main.cpp", (unsigned)(245)), 0) );

        b32 simplex_wound_ccw = (((p[1].x - p[0].x)*(p[2].y - p[1].y)) - ((p[1].y - p[0].y)*(p[2].x-p[1].x))) > 0.0f;






        EpaPoint* epa_p = (EpaPoint*)push_size_(temp_arena, sizeof(EpaPoint) * (3), no_clear());
        epa_p[0].p = p[0];
        epa_p[0].next = epa_p + 1;
        epa_p[1].p = p[1];
        epa_p[1].next = epa_p + 2;
        epa_p[2].p = p[2];
        epa_p[2].next = 0;

        f32 epsilon = 1.0e-3f;
        for (;;) {
            EpaEdge e = epa_find_closest_edge(epa_p, simplex_wound_ccw);
            v2 s = support(t1, s1, t2, s2, e.normal);
            f32 d = dot(s, e.normal);
            if ((d - e.distance) < epsilon) {
                info->vector = e.normal;
                info->depth = d;
                break;
            } else {
                EpaPoint* new_p = (EpaPoint*)push_size_(temp_arena, sizeof(EpaPoint), no_clear());
                new_p->p = s;
                new_p->next = e.point->next;
                e.point->next = new_p;
            }
        }
# 288 "pulsar_main.cpp"
    }





    end_temporary_memory(temp);

    return result;
}
# 369 "pulsar_main.cpp"
inline void dbg_draw_arrow(v2 start, v2 end, v4 color = vec4(1, 1, 1, 1)) {
    v2 dir = end - start;
    v2 dir_perp = perp(dir);
    v2 fin = 0.1f*dir_perp;
    v2 neck = start + 0.9f*dir;
    v2 arrow_verts[] = { start, neck, neck + fin, end, neck - fin, neck };
    Shape2D arrow;
    arrow.vert_count = (sizeof(arrow_verts) / sizeof((arrow_verts)[0]));
    arrow.vertices = arrow_verts;
    dbg_draw_shape(default_transform2d(), arrow, color);
}

inline Entity* get_entity(Level* level, EntityID id) {
    (void)( (!!(id.value < level->entity_count)) || (_wassert(L"id.value < level->entity_count", L"pulsar_main.cpp", (unsigned)(382)), 0) );
    Entity* result = level->entities + id.value;
    return result;
}

inline EntityID add_entity(Level* level, EntityType type) {
    (void)( (!!(level->entity_count < (sizeof(level->entities) / sizeof((level->entities)[0])))) || (_wassert(L"level->entity_count < ARRAY_COUNT(level->entities)", L"pulsar_main.cpp", (unsigned)(388)), 0) );
    EntityID entity_id = { level->entity_count++ };
    Entity* entity = get_entity(level, entity_id);

    zero_size(sizeof(*entity), &(*entity));
    entity->type = type;

    return entity_id;
}

inline EntityID add_player(GameState* game_state, Level* level, v2 starting_p) {
    EntityID entity_id = add_entity(level, EntityType_Player);
    Entity* entity = get_entity(level, entity_id);

    entity->p = starting_p;
    entity->collision = game_state->player_collision;
    entity->color = vec4(1, 1, 1, 1);

    entity->flags |= EntityFlag_Moveable|EntityFlag_Collides;

    return entity_id;
}

inline EntityID add_wall(GameState* game_state, Level* level, Rect2 rect) {
    EntityID entity_id = add_entity(level, EntityType_Wall);
    Entity* entity = get_entity(level, entity_id);

    entity->p = get_center(rect);
    entity->color = vec4(1, 1, 1, 1);
    entity->surface_friction = 5.0f;
    entity->midi_test_target = entity->p;

    entity->flags |= EntityFlag_Collides;

    rect = offset(rect, -entity->p);


    v2* verts = (v2*)push_size_(&game_state->permanent_arena, sizeof(v2) * (4));
    verts[0] = { rect.min.x, rect.min.y };
    verts[1] = { rect.max.x, rect.min.y };
    verts[2] = { rect.max.x, rect.max.y };
    verts[3] = { rect.min.x, rect.max.y };
    entity->collision = polygon(4, verts);

    return entity_id;
}

inline EntityID add_soundtrack_player(GameState* game_state, Level* level, SoundtrackID soundtrack_id, u32 playback_flags = Playback_Looping) {
    EntityID entity_id = add_entity(level, EntityType_SoundtrackPlayer);
    Entity* entity = get_entity(level, entity_id);

    entity->soundtrack_id = soundtrack_id;
    entity->playback_flags = playback_flags;
    entity->collision = circle(25.0f);

    return entity_id;
}

inline Level* allocate_level(MemoryArena* arena, char* name) {
    Level* result = (Level*)push_size_(arena, sizeof(Level));


    result->entity_count = 1;

    size_t name_length = cstr_length(name);
    result->name.len = name_length;
    result->name.data = push_string_and_null_terminate(arena, name_length, name);

    return result;
}

inline b32 on_ground(Entity* entity) {
    b32 result = entity->flags & EntityFlag_OnGround;
    return result;
}


inline void move_entity(GameState* game_state, Entity* entity, v2 ddp, f32 dt) {
# 475 "pulsar_main.cpp"
    f32 epsilon = 0.01f;


    f32 gravity = 300.0f;
    ddp.y -= gravity;

    if (on_ground(entity)) {
        ddp.x -= entity->friction_of_last_touched_surface*entity->dp.x;

        f32 off_ground_time = 0.2f;
        if (entity->off_ground_timer < off_ground_time) {
            entity->off_ground_timer += dt;
        } else {
            entity->flags &= ~EntityFlag_OnGround;
        }
    }

    v2 total_delta = 0.5f*ddp*square(dt) + entity->dp*dt;
    entity->dp += ddp*dt;

    entity->friction_of_last_touched_surface = 0.0f;

    f32 t_left = 1.0f;
    v2 delta = t_left*total_delta;

    if (entity->flags & EntityFlag_Collides) {

        for (u32 entity_index = 0; entity_index < game_state->entity_count; entity_index++) {
            Entity* test_entity = game_state->entities + entity_index;
            if (entity != test_entity) {
                entity->friction_of_last_touched_surface = test_entity->surface_friction;
                Transform2D test_t = transform2d(test_entity->p);
                if (test_entity->flags & EntityFlag_Collides) {
                    test_entity->sticking_entity = 0;
                    u32 iteration = 0;
                    Transform2D t = transform2d(entity->p, delta);
                    Transform2D test_t = transform2d(test_entity->p);
                    do {
                        CollisionInfo collision;
                        if (gjk_intersect(t, entity->collision, test_t, test_entity->collision, &collision, &game_state->transient_arena)) {
                            dbg_draw_arrow(entity->p, entity->p + collision.vector*25.0f, vec4(1, 0, 1, 1));
                            if (collision.vector.y < -0.707f) {
                                if (!on_ground(entity)) {



                                    entity->flags |= EntityFlag_OnGround;
                                }
                                entity->off_ground_timer = 0.0f;
                                test_entity->sticking_entity = entity;
                            }
                            f32 depth = collision.depth + epsilon;
                            f32 theta_times_length_of_delta = dot(collision.vector, delta);
                            if (theta_times_length_of_delta == 0) {
                                delta = -collision.vector*depth;
                                break;
                            } else {
                                f32 penetration_along_delta = depth / theta_times_length_of_delta;
                                f32 t_spent = 1.0f - penetration_along_delta;
                                t_left -= t_spent;

                                entity->p += t_spent*delta;
                                entity->dp -= collision.vector*dot(entity->dp, collision.vector);
                                total_delta -= collision.vector*dot(total_delta, collision.vector);
                            }
                        } else {
                            break;
                        }

                        delta = t_left*total_delta;

                        t.offset = entity->p;
                        t.sweep = delta;

                        iteration++;
                    } while (iteration < 4 && t_left > epsilon);
                }
            }
        }
    }

    entity->p += delta;
}

inline PlayingMidi* play_midi(GameState* game_state, MidiTrack* track, u32 flags = 0, PlayingSound* sync_sound = 0) {
    if (!game_state->first_free_playing_midi) {
        game_state->first_free_playing_midi = (PlayingMidi*)push_size_(&game_state->permanent_arena, sizeof(PlayingMidi));
        game_state->first_free_playing_midi->next_free = 0;
    }

    PlayingMidi* playing_midi = game_state->first_free_playing_midi;
    game_state->first_free_playing_midi = playing_midi->next_free;

    playing_midi->next = game_state->first_playing_midi;
    game_state->first_playing_midi = playing_midi;

    zero_size(sizeof(*playing_midi), &(*playing_midi));
    playing_midi->track = track;
    playing_midi->flags = flags;
    playing_midi->sync_sound = sync_sound;

    return playing_midi;
}

inline void play_soundtrack(GameState* game_state, Soundtrack* soundtrack, u32 flags = 0) {
    Sound* sound = get_sound(&game_state->assets, soundtrack->sound);
    PlayingSound* playing_sound = play_sound(&game_state->audio_mixer, sound, flags);
    for (u32 midi_index = 0; midi_index < soundtrack->midi_track_count; midi_index++) {
        MidiTrack* track = get_midi(&game_state->assets, soundtrack->midi_tracks[midi_index]);
        play_midi(game_state, track, flags, playing_sound);
    }
}

static u32 parse_utf8_codepoint(char* input_string, u32* out_codepoint) {
    u32 num_bytes = 0;
    char* at = input_string;
    u32 codepoint = 0;

    if (!at[0]) {

    } else {
        u8 utf8_mask[] = {
            (1 << 7) - 1,
            (1 << 5) - 1,
            (1 << 4) - 1,
            (1 << 3) - 1,
        };
        u8 utf8_matching_value[] = { 0, 0xC0, 0xE0, 0xF0 };
        if ((*at & ~utf8_mask[0]) == utf8_matching_value[0]) {
            num_bytes = 1;
        } else if ((u8)(*at & ~utf8_mask[1]) == utf8_matching_value[1]) {
            num_bytes = 2;
        } else if ((u8)(*at & ~utf8_mask[2]) == utf8_matching_value[2]) {
            num_bytes = 3;
        } else if ((u8)(*at & ~utf8_mask[3]) == utf8_matching_value[3]) {
            num_bytes = 4;
        } else {
            (void)( (!!(!"Invalid Code Path")) || (_wassert(L"!\"Invalid Code Path\"", L"pulsar_main.cpp", (unsigned)(612)), 0) );;
        }

        u32 offset = 6 * (num_bytes - 1);
        for (u32 byte_index = 0; byte_index < num_bytes; byte_index++) {
            if (byte_index == 0) {
                codepoint = (*at & utf8_mask[num_bytes-1]) << offset;
            } else {
                if (*at != 0) {
                    codepoint |= (*at & ((1 << 6) - 1)) << offset;
                } else {


                    (void)( (!!(!"Invalid Code Path")) || (_wassert(L"!\"Invalid Code Path\"", L"pulsar_main.cpp", (unsigned)(625)), 0) );;
                }
            }
            offset -= 6;
            at++;
        }
    }

    *out_codepoint = codepoint;
    return num_bytes;
}

static void editor_draw_text(EditorState* editor, v2 p, v4 color, char* format_string, ...) {
    TemporaryMemory temp = begin_temporary_memory(editor->arena);

    va_list va_args;
    __builtin_va_start(va_args, format_string);

    u32 text_size = stbsp_vsnprintf(0, 0, format_string, va_args) + 1;

    char* text = (char*)push_size_(editor->arena, text_size, align_no_clear(1));
    stbsp_vsnprintf(text, text_size, format_string, va_args);

    __builtin_va_end(va_args);

    v2 at_p = p;
    Font* font = editor->font;
    for (char* at = text; at[0]; at++) {
        if (at[0] == ' ') {
            at_p.x += font->whitespace_width;
        } else {
            ImageID glyph_id = get_glyph_id_for_codepoint(font, at[0]);
            if (glyph_id.value) {
                Image* glyph = get_image(editor->assets, glyph_id);
                push_image(editor->render_commands, glyph, at_p + vec2(2.0f, -2.0f), vec4(0, 0, 0, 0.75f));
                push_image(editor->render_commands, glyph, at_p, color);
                if (at[1] && at[1] != ' ') {
                    at_p.x += get_advance_for_codepoint_pair(font, at[0], at[1]);
                } else {

                    at_p.x += get_advance_for_codepoint_pair(font, at[0], at[0]);
                }
            }
        }
    }

    end_temporary_memory(temp);
}

inline EditorState* allocate_editor_state(GameRenderCommands* render_commands, Assets* assets, MemoryArena* permanent_arena, MemoryArena* transient_arena) {
    EditorState* editor = (EditorState*)push_size_(permanent_arena, sizeof(EditorState));
    editor->render_commands = render_commands;
    editor->assets = assets;
    editor->arena = transient_arena;
    return editor;
}

inline void stop_all_midi_tracks(GameState* game_state) {
    PlayingMidi* last_playing_midi = game_state->first_playing_midi;
    while (last_playing_midi->next) {
        last_playing_midi = last_playing_midi->next;
    }
    last_playing_midi->next = game_state->first_free_playing_midi;
    game_state->first_free_playing_midi = game_state->first_playing_midi;
    game_state->first_playing_midi = 0;
}

inline void switch_gamemode(GameState* game_state, GameMode game_mode) {
    switch (game_mode) {
        case GameMode_Ingame: {
            Level* level = game_state->active_level;
            for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
                game_state->entities[entity_index] = level->entities[entity_index];
            }
            game_state->entity_count = level->entity_count;
        } break;

        case GameMode_Editor: {
            stop_all_sounds(&game_state->audio_mixer);
            game_state->entity_count = 0;
        } break;
    }

    game_state->game_mode = game_mode;
}

static void game_update_and_render(GameMemory* memory, GameInput* input, GameRenderCommands* render_commands) {
    (void)( (!!(memory->permanent_storage_size >= sizeof(GameState))) || (_wassert(L"memory->permanent_storage_size >= sizeof(GameState)", L"pulsar_main.cpp", (unsigned)(712)), 0) );

    platform = memory->platform_api;





    GameState* game_state = (GameState*) memory->permanent_storage;
    if (!memory->initialized) {
        initialize_arena(&game_state->permanent_arena, memory->permanent_storage_size - sizeof(GameState), (u8*) memory->permanent_storage + sizeof(GameState));
        initialize_arena(&game_state->transient_arena, memory->transient_storage_size, memory->transient_storage);

        initialize_audio_mixer(&game_state->audio_mixer, &game_state->permanent_arena);


        load_assets(&game_state->assets, &game_state->transient_arena, "assets.pla");

        game_state->editor_state = allocate_editor_state(render_commands, &game_state->assets, &game_state->permanent_arena, &game_state->transient_arena);

        game_state->test_music = get_sound_by_name(&game_state->assets, "test_music");
        game_state->test_sound = get_sound_by_name(&game_state->assets, "test_sound");
        game_state->test_image = get_image_by_name(&game_state->assets, "test_image");

        v2* square_verts = (v2*)push_size_(&game_state->permanent_arena, sizeof(v2) * (4));
        square_verts[0] = { -10.0f, 0.0f };
        square_verts[1] = { 10.0f, 0.0f };
        square_verts[2] = { 10.0f, 50.0f };
        square_verts[3] = { -10.0f, 50.0f };
        game_state->player_collision = polygon(4, square_verts);

        game_state->active_level = allocate_level(&game_state->permanent_arena, "Default Level");

        add_player(game_state, game_state->active_level, vec2(512, 512));

        SoundtrackID soundtrack_id = get_soundtrack_id_by_name(&game_state->assets, "test_soundtrack");
        add_soundtrack_player(game_state, game_state->active_level, soundtrack_id);

        for (s32 i = 0; i < 12; i++) {
            EntityID wall_index = add_wall(game_state, game_state->active_level, rect_center_dim(vec2(512 + 64*i, 400), vec2(64, 64)));
            Entity* wall = get_entity(game_state->active_level, wall_index);
            wall->midi_note = 60 + i;
        }

        switch_gamemode(game_state, GameMode_Ingame);



        memory->initialized = true;
    }

    u32 width = render_commands->width;
    u32 height = render_commands->height;

    v2 mouse_p = vec2(input->mouse_x, input->mouse_y);
    f32 dt = input->frame_dt;
    f32 rcp_dt = 1.0f / dt;

    push_clear(render_commands, vec4(0.5f, 0.5f, 0.5f, 1.0f));

    if (game_state->game_mode == GameMode_Ingame) {
        game_state->midi_event_buffer_count = 0;
        for (PlayingMidi** playing_midi_ptr = &game_state->first_playing_midi; *playing_midi_ptr;) {
            PlayingMidi* playing_midi = *playing_midi_ptr;

            MidiTrack* track = playing_midi->track;
            if (playing_midi->event_index < track->event_count) {
                if (playing_midi->sync_sound) {
                    (void)( (!!(track->ticks_per_second == 48000)) || (_wassert(L"track->ticks_per_second == 48000", L"pulsar_main.cpp", (unsigned)(780)), 0) );
                    playing_midi->tick_timer = playing_midi->sync_sound->samples_played;
                }

                u32 ticks_for_frame = round_f32_to_u32(((f32) track->ticks_per_second)*dt);
                playing_midi->tick_timer += ticks_for_frame;

                MidiEvent event = track->events[playing_midi->event_index];
                while (event.absolute_time_in_ticks <= playing_midi->tick_timer && playing_midi->event_index < track->event_count) {
                    playing_midi->event_index++;

                    ActiveMidiEvent* active_event = game_state->midi_event_buffer + game_state->midi_event_buffer_count++;
                    active_event->midi_event = event;
                    active_event->dt_left = dt*(1.0f - ((f32) (playing_midi->tick_timer - event.absolute_time_in_ticks) / (f32) ticks_for_frame));

                    (void)( (!!(game_state->midi_event_buffer_count < (sizeof(game_state->midi_event_buffer) / sizeof((game_state->midi_event_buffer)[0])))) || (_wassert(L"game_state->midi_event_buffer_count < ARRAY_COUNT(game_state->midi_event_buffer)", L"pulsar_main.cpp", (unsigned)(795)), 0) );

                    event = track->events[playing_midi->event_index];
                }

                playing_midi->tick_timer = frame_time;
            }

            if (playing_midi->event_index >= track->event_count) {
                (void)( (!!(playing_midi->event_index == track->event_count)) || (_wassert(L"playing_midi->event_index == track->event_count", L"pulsar_main.cpp", (unsigned)(804)), 0) );
                if (playing_midi->flags & Playback_Looping) {
                    playing_midi->event_index = 0;
                    playing_midi->tick_timer = 0;
                    playing_midi_ptr = &playing_midi->next;
                } else {
                    *playing_midi_ptr = playing_midi->next;
                    playing_midi->next = game_state->first_free_playing_midi;
                    game_state->first_free_playing_midi = playing_midi;
                }
            } else {
                playing_midi_ptr = &playing_midi->next;
            }
        }





        for (u32 entity_index = 1; entity_index < game_state->entity_count; entity_index++) {
            Entity* entity = game_state->entities + entity_index;
            (void)( (!!(entity->type != EntityType_Null)) || (_wassert(L"entity->type != EntityType_Null", L"pulsar_main.cpp", (unsigned)(825)), 0) );

            entity->ddp = vec2(0, 0);
            entity->was_on_ground = on_ground(entity);

            entity->sim_dt = dt;

            switch (entity->type) {
                case EntityType_Player: {
                    if (entity->p.x < 0.0f) {
                        entity->p.x += width;
                    } else if (entity->p.x >= width) {
                        entity->p.x -= width;
                    }
                    if (entity->p.y < 0.0f) {
                        entity->p.y += height;
                    } else if (entity->p.y >= height) {
                        entity->p.y -= height;
                    }
                    GameController* controller = &input->controller;
                    f32 move_speed = entity->was_on_ground ? 1000.0f : 200.0f;
                    if (controller->move_left.is_down) {
                        entity->ddp.x -= move_speed;
                    }
                    if (controller->move_right.is_down) {
                        entity->ddp.x += move_speed;
                    }

                    if (entity->was_on_ground && was_pressed(controller->move_up)) {
                        play_sound(&game_state->audio_mixer, game_state->test_sound);
                        entity->ddp.y += 10000.0f;
                        entity->flags &= ~EntityFlag_OnGround;
                    }
                } break;

                case EntityType_Wall: {
                    v2 movement = {};
                    for (u32 event_index = 0; event_index < game_state->midi_event_buffer_count; event_index++) {
                        ActiveMidiEvent event = game_state->midi_event_buffer[event_index];
                        if (event.note_value == entity->midi_note) {
                            entity->sim_dt = event.dt_left;
                            if (event.type == MidiEvent_NoteOn) {
                                entity->midi_test_target.y += 20.0f*(event.note_value - 59);
                            } else if (event.type == MidiEvent_NoteOff) {
                                entity->midi_test_target.y -= 20.0f*(event.note_value - 59);
                            } else {
                                (void)( (!!(!"Invalid Code Path")) || (_wassert(L"!\"Invalid Code Path\"", L"pulsar_main.cpp", (unsigned)(871)), 0) );;
                            }
                        }
                    }
                    movement = 100.0f*(entity->midi_test_target - entity->p);
                    entity->p += entity->sim_dt*movement;
                    entity->dp = movement;
                    entity->movement_t += entity->sim_dt;







                } break;

                case EntityType_SoundtrackPlayer: {
                    if (!entity->soundtrack_has_been_played) {
                        Soundtrack* soundtrack = get_soundtrack(&game_state->assets, entity->soundtrack_id);
                        play_soundtrack(game_state, soundtrack, entity->playback_flags);
                        entity->soundtrack_has_been_played = true;
                    }
                } break;

                default: { (void)( (!!(!"Invalid Default Case")) || (_wassert(L"!\"Invalid Default Case\"", L"pulsar_main.cpp", (unsigned)(896)), 0) ); } break;;
            }
        }

        if (was_pressed(input->debug_fkeys[1])) {
            switch_gamemode(game_state, GameMode_Editor);
        }
    } else if (game_state->game_mode == GameMode_Editor) {
        EditorState* editor = game_state->editor_state;

        if (!editor->initialized) {
            editor->active_level = game_state->active_level;
            editor->font = get_font_by_name(editor->assets, "editor_font");

            editor->initialized = true;
        }

        Level* level = editor->active_level;
        editor_draw_text(editor, vec2(4.0f, height - get_baseline_from_top(editor->font)), vec4(1, 1, 1, 1), "Editing level '%s'", level->name.data);

        if (was_pressed(input->debug_fkeys[1])) {
            switch_gamemode(game_state, GameMode_Ingame);
        }
    }





    u32 active_entity_count = 0;
    Entity* active_entities = 0;
    if (game_state->game_mode == GameMode_Ingame) {
        active_entity_count = game_state->entity_count;
        active_entities = game_state->entities;
    } else if (game_state->game_mode == GameMode_Editor) {
        active_entity_count = game_state->active_level->entity_count;
        active_entities = game_state->active_level->entities;
    } else {
        (void)( (!!(!"Invalid Code Path")) || (_wassert(L"!\"Invalid Code Path\"", L"pulsar_main.cpp", (unsigned)(934)), 0) );;
    }

    for (u32 entity_index = 1; entity_index < active_entity_count; entity_index++) {
        Entity* entity = active_entities + entity_index;
        (void)( (!!(entity->type != EntityType_Null)) || (_wassert(L"entity->type != EntityType_Null", L"pulsar_main.cpp", (unsigned)(939)), 0) );

        if (game_state->game_mode == GameMode_Ingame) {
            opengl_rectangle(rect_center_dim(entity->p + entity->sticking_dp, vec2(2, 2)), vec4(1, 0, 0, 1));
            if (entity->flags & EntityFlag_Moveable) {
                move_entity(game_state, entity, entity->ddp, entity->sim_dt);
            }

            if (entity->was_on_ground && !on_ground(entity)) {

                entity->sticking_dp = vec2(0, 0);
            }
        }

        if (on_ground(entity)) {
            entity->color = vec4(0, 0, 1, 1);
        }

        Transform2D transform = default_transform2d();
        transform.offset = entity->p;
        push_shape(render_commands, transform, entity->collision, entity->color);

        entity->color = vec4(1, 1, 1, 1);
    }

    check_arena(&game_state->permanent_arena);
    check_arena(&game_state->transient_arena);
}

static void game_get_sound(GameMemory* memory, GameSoundOutputBuffer* sound_buffer) {
    (void)( (!!(memory->initialized)) || (_wassert(L"memory->initialized", L"pulsar_main.cpp", (unsigned)(969)), 0) );
    GameState* game_state = (GameState*) memory->permanent_storage;

    output_playing_sounds(&game_state->audio_mixer, sound_buffer, &game_state->transient_arena);
}
