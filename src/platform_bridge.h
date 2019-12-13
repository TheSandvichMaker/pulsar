#ifndef PLATFORM_BRIDGE_H
#define PLATFORM_BRIDGE_H

#include "file_io.h"

#define PLATFORM_READ_ENTIRE_FILE(name) EntireFile name(char* file_name)
typedef PLATFORM_READ_ENTIRE_FILE(PlatformReadEntireFile);

#define PLATFORM_WRITE_ENTIRE_FILE(name) b32 name(char* file_name, u32 size, void* data)
typedef PLATFORM_WRITE_ENTIRE_FILE(PlatformWriteEntireFile);

#define PLATFORM_ALLOCATE_MEMORY(name) void* name(size_t size)
typedef PLATFORM_ALLOCATE_MEMORY(PlatformAllocateMemory);

#define PLATFORM_DEALLOCATE_MEMORY(name) void name(void* memory)
typedef PLATFORM_DEALLOCATE_MEMORY(PlatformDeallocateMemory);

struct PlatformAPI {
    PlatformReadEntireFile* read_entire_file;
    PlatformWriteEntireFile* write_entire_file;
    PlatformAllocateMemory* allocate;
    PlatformDeallocateMemory* deallocate;
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

#define MAX_CONTROLLER_COUNT 5
struct GameController {
    b32 is_connected;

    union {
        GameButtonState buttons[4];
        struct {
            GameButtonState move_up;
            GameButtonState move_down;
            GameButtonState move_left;
            GameButtonState move_right;
        };
    };
};

#define PLATFORM_KEYBOARD_CONTROLLER 0

struct GameInput {
    b32 in_focus, focus_changed;
    f32 frame_dt;

    GameController controllers[MAX_CONTROLLER_COUNT];

    u32 mouse_x, mouse_y, mouse_z;
    GameButtonState mouse_buttons[PlatformMouseButton_Count];

    GameButtonState debug_fkeys[13];

    b32 quit_requested;
};

inline GameController* get_controller(GameInput* input, u32 controller_index) {
    assert(controller_index < ARRAY_COUNT(input->controllers));
    return input->controllers + controller_index;
}

#define GAME_UPDATE_AND_RENDER(name) void name(GameMemory* memory, GameInput* input, u32 width, u32 height)
typedef GAME_UPDATE_AND_RENDER(GameUpdateAndRender);

// @Note: At the moment, this has to be a very fast function. Not more than ~1ms.
// @TODO: Reduce the pressure on this function's performance by measuring it / asking about it.
#define GAME_GET_SOUND(name) void name(GameMemory* memory, GameSoundOutputBuffer* sound_buffer)
typedef GAME_GET_SOUND(GameGetSound);

#endif /* PLATFORM_BRIDGE_H */
