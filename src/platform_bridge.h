#ifndef PLATFORM_BRIDGE_H
#define PLATFORM_BRIDGE_H

#ifndef SND_MATH_TYPES
#define SND_MATH_TYPES
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
#endif

#include "file_io.h"
#include "asset_pack_format.h"

#define PLATFORM_READ_ENTIRE_FILE(name) EntireFile name(char* file_name)
typedef PLATFORM_READ_ENTIRE_FILE(PlatformReadEntireFile);

#define PLATFORM_WRITE_ENTIRE_FILE(name) b32 name(char* file_name, u32 size, void* data)
typedef PLATFORM_WRITE_ENTIRE_FILE(PlatformWriteEntireFile);

#define PLATFORM_ALLOCATE_MEMORY(name) void* name(size_t size)
typedef PLATFORM_ALLOCATE_MEMORY(PlatformAllocateMemory);

#define PLATFORM_DEALLOCATE_MEMORY(name) void name(void* memory)
typedef PLATFORM_DEALLOCATE_MEMORY(PlatformDeallocateMemory);

#define PLATFORM_ALLOCATE_TEXTURE(name) void* name(u32 w, u32 h, void* data, PixelFormat format)
typedef PLATFORM_ALLOCATE_TEXTURE(PlatformAllocateTexture);

#define PLATFORM_DEALLOCATE_TEXTURE(name) void name(void* handle)
typedef PLATFORM_DEALLOCATE_TEXTURE(PlatformDeallocateTexture);

struct PlatformAPI {
    PlatformReadEntireFile* read_entire_file;
    PlatformWriteEntireFile* write_entire_file;
    PlatformAllocateMemory* allocate;
    PlatformDeallocateMemory* deallocate;
    PlatformAllocateTexture* allocate_texture;
    PlatformDeallocateTexture* deallocate_texture;
};

/*
 * NOTE: Game Memory HAS to be initialized with ZEROED memory!
 */
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

#define PLATFORM_KEYBOARD_CONTROLLER 0

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

#define GAME_UPDATE_AND_RENDER(name) void name(GameMemory* memory, GameInput* input, u32 width, u32 height)
typedef GAME_UPDATE_AND_RENDER(GameUpdateAndRender);

// @Note: At the moment, this has to be a very fast function. Not more than ~1ms.
// @TODO: Reduce the pressure on this function's performance by measuring it / asking about it.
#define GAME_GET_SOUND(name) void name(GameMemory* memory, GameSoundOutputBuffer* sound_buffer)
typedef GAME_GET_SOUND(GameGetSound);

#endif /* PLATFORM_BRIDGE_H */
