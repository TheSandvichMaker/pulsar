#ifndef PLATFORM_BRIDGE_H
#define PLATFORM_BRIDGE_H

#include "pulsar_code_generator.h"
#include "pulsar_generated_pre_headers.h"

#include "math.h"
#include "file_io.h"
#include "pulsar_memory.h"
#include "pulsar_asset_pack_file_format.h"

struct PlatformFileHandle {
    b32 no_errors;
    void* handle;
};

#define PLATFORM_READ_ENTIRE_FILE(name) EntireFile name(char* file_name, Allocator allocator)
typedef PLATFORM_READ_ENTIRE_FILE(PlatformReadEntireFile);

#define PLATFORM_WRITE_ENTIRE_FILE(name) b32 name(char* file_name, u32 size, void* data)
typedef PLATFORM_WRITE_ENTIRE_FILE(PlatformWriteEntireFile);

#if 0
#define PLATFORM_OPEN_FILE(name) PlatformFileHandle name(char* file_name)
typedef PLATFORM_OPEN_FILE(PlatformOpenFile);

#define PLATFORM_READ_FROM_FILE(name) void* name(PlatformFileHandle* handle, size_t offset, size_t amount)
typedef PLATFORM_READ_FROM_FILE(PlatformReadFromFile);

#define PLATFORM_WRITE_TO_FILE(name) void name(PlatformFileHandle* handle, size_t offset, size_t amount, void* data)
typedef PLATFORM_WRITE_TO_FILE(PlatformWriteToFile);
#endif

#define PLATFORM_ALLOCATE_MEMORY(name) void* name(size_t size)
typedef PLATFORM_ALLOCATE_MEMORY(PlatformAllocateMemory);

#define PLATFORM_DEALLOCATE_MEMORY(name) void name(void* memory)
typedef PLATFORM_DEALLOCATE_MEMORY(PlatformDeallocateMemory);

#define PLATFORM_ALLOCATE_TEXTURE(name) void* name(u32 w, u32 h, void* data, PixelFormat format)
typedef PLATFORM_ALLOCATE_TEXTURE(PlatformAllocateTexture);

#define PLATFORM_DEALLOCATE_TEXTURE(name) void name(void* handle)
typedef PLATFORM_DEALLOCATE_TEXTURE(PlatformDeallocateTexture);

#define DEBUG_PLATFORM_PRINT(name) void name(char* text)
typedef DEBUG_PLATFORM_PRINT(DebugPlatformPrint);

struct PlatformAPI {
    PlatformReadEntireFile* read_entire_file;
    PlatformWriteEntireFile* write_entire_file;
    PlatformAllocateMemory* allocate;
    PlatformDeallocateMemory* deallocate;
    PlatformAllocateTexture* allocate_texture;
    PlatformDeallocateTexture* deallocate_texture;

#if PULSAR_DEBUG
    DebugPlatformPrint* debug_print;
#endif
};

#if PULSAR_DEBUG
#define FRAME_TIME_HISTORY_LENGTH 120
struct DebugFrameTimeHistory {
    f32 history[FRAME_TIME_HISTORY_LENGTH];
    u32 first_valid_entry;
    u32 valid_entry_count;
};

struct DebugProfilingEvent {
    char* name;
    u64 cycles;
};

struct PlatformDebugInfo {
    DebugFrameTimeHistory* frame_history;

    u32 print_size;
    char* print;
};
#endif

//
// @Note: Game Memory HAS to be initialized with ZEROED memory!
//
//
struct GameMemory {
    b32 initialized;

    size_t permanent_storage_size;
    void* permanent_storage;
    size_t transient_storage_size;
    void* transient_storage;

    PlatformAPI platform_api;

#if PULSAR_DEBUG
    PlatformDebugInfo debug_info;
#endif
};

struct GameSoundOutputBuffer {
    u32 channel_count;
    u32 sample_rate;

    u32 samples_committed; // # of samples the platform has committed to since the last time we were called
    u32 samples_to_write;  // # of samples the platform would like you to write

    s16* samples;
};

struct GameRenderCommands {
    u32 width, height;

    u32 command_buffer_size;
    u32 sort_entry_count;
    u32 first_command;
    // @Note: The command buffer starts with sort_entry_count sort entries, then it contains
    // render commands starting at first_command up until command_buffer_size
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

introspect() enum PlatformKeyCode {
    PKC_LButton        = 0x1,
    PKC_RButton        = 0x2,
    PKC_Cancel         = 0x3,
    PKC_MButton        = 0x4,
    PKC_XButton1       = 0x5,
    PKC_XButton2       = 0x6,
    PKC_Back           = 0x8,
    PKC_Tab            = 0x9,
    PKC_Clear          = 0xC,
    PKC_Return         = 0xD,
    PKC_Shift          = 0x10,
    PKC_Control        = 0x11,
    PKC_Alt            = 0x12,
    PKC_Pause          = 0x13,
    PKC_CapsLock       = 0x14,
    PKC_Kana           = 0x15,
    // PKC_Hangul         = 0x15,
    PKC_Junja          = 0x17,
    PKC_Final          = 0x18,
    // PKC_Hanja          = 0x19,
    PKC_Kanji          = 0x19,
    PKC_Escape         = 0x1B,
    PKC_Convert        = 0x1C,
    PKC_NonConvert     = 0x1D,
    PKC_Accept         = 0x1E,
    PKC_ModeChange     = 0x1F,
    PKC_Space          = 0x20,
    PKC_PageUp         = 0x21,
    PKC_PageDown       = 0x22,
    PKC_End            = 0x23,
    PKC_Home           = 0x24,
    PKC_Left           = 0x25,
    PKC_Up             = 0x26,
    PKC_Right          = 0x27,
    PKC_Down           = 0x28,
    PKC_Select         = 0x29,
    PKC_Print          = 0x2A,
    PKC_Execute        = 0x2B,
    PKC_PrintScreen    = 0x2C,
    PKC_Insert         = 0x2D,
    PKC_Delete         = 0x2E,
    PKC_Help           = 0x2F,
    /* 0x30 - 0x39: ascii numerals */
    /* 0x3A - 0x40: undefined */
    /* 0x41 - 0x5A: ascii alphabet */
    PKC_LSys           = 0x5B,
    PKC_RSys           = 0x5C,
    PKC_Apps           = 0x5D,
    PKC_Sleep          = 0x5f,
    PKC_Numpad0        = 0x60,
    PKC_Numpad1        = 0x61,
    PKC_Numpad2        = 0x62,
    PKC_Numpad3        = 0x63,
    PKC_Numpad4        = 0x64,
    PKC_Numpad5        = 0x65,
    PKC_Numpad6        = 0x66,
    PKC_Numpad7        = 0x67,
    PKC_Numpad8        = 0x68,
    PKC_Numpad9        = 0x69,
    PKC_Multiply       = 0x6A,
    PKC_Add            = 0x6B,
    PKC_Separator      = 0x6C,
    PKC_Subtract       = 0x6D,
    PKC_Decimal        = 0x6E,
    PKC_Divide         = 0x6f,
    PKC_F1             = 0x70,
    PKC_F2             = 0x71,
    PKC_F3             = 0x72,
    PKC_F4             = 0x73,
    PKC_F5             = 0x74,
    PKC_F6             = 0x75,
    PKC_F7             = 0x76,
    PKC_F8             = 0x77,
    PKC_F9             = 0x78,
    PKC_F10            = 0x79,
    PKC_F11            = 0x7A,
    PKC_F12            = 0x7B,
    PKC_F13            = 0x7C,
    PKC_F14            = 0x7D,
    PKC_F15            = 0x7E,
    PKC_F16            = 0x7F,
    PKC_F17            = 0x80,
    PKC_F18            = 0x81,
    PKC_F19            = 0x82,
    PKC_F20            = 0x83,
    PKC_F21            = 0x84,
    PKC_F22            = 0x85,
    PKC_F23            = 0x86,
    PKC_F24            = 0x87,
    PKC_Numlock        = 0x90,
    PKC_Scroll         = 0x91,
    PKC_LShift         = 0xA0,
    PKC_RShift         = 0xA1,
    PKC_LControl       = 0xA2,
    PKC_RControl       = 0xA3,
    PKC_LAlt           = 0xA4,
    PKC_RAlt           = 0xA5,
    /* 0xA6 - 0xAC: browser keys, not sure what's up with that */
    PKC_VolumeMute     = 0xAD,
    PKC_VolumeDown     = 0xAE,
    PKC_VolumeUp       = 0xAF,
    PKC_MediaNextTrack = 0xB0,
    PKC_MediaPrevTrack = 0xB1,
    /* 0xB5 - 0xB7: "launch" keys, not sure what's up with that */
    PKC_Oem1           = 0xBA, // misc characters, us standard: ';:'
    PKC_Plus           = 0xBB,
    PKC_Comma          = 0xBC,
    PKC_Minus          = 0xBD,
    PKC_Period         = 0xBE,
    PKC_Oem2           = 0xBF, // misc characters, us standard: '/?'
    PKC_Oem3           = 0xC0, // misc characters, us standard: '~'
    /* 0xC1 - 0xDA: reserved / unassigned */
    /* 0xDB - 0xF5: more miscellanious OEM codes I'm ommitting for now */
    /* 0xF6 - 0xF9: keys I've never heard of */
    PKC_Play           = 0xFA,
    PKC_Zoom           = 0xFB,
    PKC_OemClear       = 0xFE,
};

// @Note: The platform input event buffer is for when you want to do text input, basically. It's cleared each frame, so
// this is the maximum number of key down events that can be logged in a single frame.
#define PLATFORM_INPUT_EVENT_BUFFER_SIZE 128
struct GameInput {
    b32 in_focus, focus_changed;
    f32 update_rate, frame_dt;

    // @Note: quit_requested is from the game to the platform, not the other way around.
    b32 quit_requested;

    GameController controller;

    u32 mouse_x, mouse_y, mouse_z;
    GameButtonState mouse_buttons[PlatformMouseButton_Count];

    // @Note: All these keys are just for dev purposes
    GameButtonState debug_fkeys[13];
    union {
        GameButtonState editor_keys[7];
        struct {
            GameButtonState ctrl, shift, alt, space, del, escape, tilde;
        };
    };
    GameButtonState keys[26];

    b32 event_mode;
    u32 event_count;
    u32 event_buffer[PLATFORM_INPUT_EVENT_BUFFER_SIZE];
};

inline PlatformKeyCode decode_input_event(u32 event, char* ascii = 0) {
    PlatformKeyCode result = cast(PlatformKeyCode) (event & 0xFF);
    if (ascii) *ascii = cast(char) ((event >> 8) & 0xFF);
    return result;
}

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

#define GAME_GET_SOUND(name) void name(GameMemory* memory, GameSoundOutputBuffer* sound_buffer)
typedef GAME_GET_SOUND(GameGetSound);

#define GAME_POST_RENDER(name) void name(GameMemory* memory, GameInput* input, GameRenderCommands* render_commands)
typedef GAME_POST_RENDER(GamePostRender);

#endif /* PLATFORM_BRIDGE_H */
