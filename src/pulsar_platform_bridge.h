#ifndef PLATFORM_BRIDGE_H
#define PLATFORM_BRIDGE_H

#include "pulsar_code_generator.h"
#include "pulsar_generated_pre_headers.h"

#include "math.h"
#include "file_io.h"
#include "pulsar_asset_pack_file_format.h"
#include "pulsar_render_commands.h"

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

struct GameRenderCommands {
    u32 width, height;

    u32 command_buffer_size;
    u32 command_buffer_used;
    u8* command_buffer;
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

struct GameInput {
    b32 in_focus, focus_changed;
    f32 frame_dt;

    GameController controller;

    u32 mouse_x, mouse_y, mouse_z;
    GameButtonState mouse_buttons[PlatformMouseButton_Count];

    // @Note: All these keys are just for dev purposes
    GameButtonState debug_fkeys[13];
    union {
        GameButtonState editor_keys[6];
        struct {
            GameButtonState ctrl, shift, alt, space, del, escape;
        };
    };
    GameButtonState keys[26];

    b32 quit_requested;
};

inline GameButtonState get_key(GameInput* input, char key) {
    assert(key >= 'A' && key <= 'Z');
    GameButtonState result = input->keys[key - 'A'];
    return result;
}

inline b32 was_pressed(GameButtonState button) {
    b32 result = (button.half_transition_count > 1) || (button.is_down && button.half_transition_count > 0);
    return result;
}

inline b32 was_released(GameButtonState button) {
    b32 result = (button.half_transition_count > 1) || (!button.is_down && button.half_transition_count > 0);
    return result;
}

#define GAME_UPDATE_AND_RENDER(name) void name(GameMemory* memory, GameInput* input, GameRenderCommands* render_commands)
typedef GAME_UPDATE_AND_RENDER(GameUpdateAndRender);

// @Note: At the moment, this has to be a very fast function. Not more than ~1ms.
// @TODO: Reduce the pressure on this function's performance by measuring it / asking about it.
#define GAME_GET_SOUND(name) void name(GameMemory* memory, GameSoundOutputBuffer* sound_buffer)
typedef GAME_GET_SOUND(GameGetSound);

#define GAME_POST_RENDER(name) void name(GameMemory* memory, GameInput* input, GameRenderCommands* render_commands)
typedef GAME_POST_RENDER(GamePostRender);

#endif /* PLATFORM_BRIDGE_H */
