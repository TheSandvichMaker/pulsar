// @TODO: Use large pages with VirtualAlloc

#define ASSERT_ON_LOG_ERROR 0

#include <windows.h>
#include <xinput.h>
#include <dsound.h>
#include <gl/gl.h>

#include <stdio.h>

#define STB_SPRINTF_STATIC 1
#define STB_SPRINTF_IMPLEMENTATION 1
#include "external/stb_sprintf.h"

#include "pulsar_common.h"
#include "pulsar_platform_bridge.h"

#include "pulsar_opengl.h"
#include "win32_opengl.h"

#include "file_io.cpp"
#include "pulsar_sort.cpp"

#include "pulsar_shapes.h"
#include "pulsar_render_commands.h"

#include "pulsar_opengl.cpp"
#include "win32_opengl.cpp"

global WglInfo wgl_info;
global OpenGLInfo opengl_info;

// @TODO: Have a good policy about shared code
inline b32 parse_config(GameConfig* config, String in_file);

#include "pulsar_main.cpp"

#define WIN32_LOG_MEMORY_CHUNK_SIZE MEGABYTES(1)
struct Win32LogMemory {
    Win32LogMemory* next;
    size_t used;
    char memory[WIN32_LOG_MEMORY_CHUNK_SIZE];
};

struct Win32State {
    b32 running;
    b32 in_focus;
    b32 focus_changed;
    b32 cursor_shown;
    b32 cursor_is_in_window;

    MemoryArena platform_arena;

    u64 last_config_write_time;
    String config_file;
    GameConfig config;

    b32 xinput_valid;
    b32 dsound_valid;

    b32 log_file_valid;
    b32 log_memory_valid;

    u32 unread_log_messages;
    u32 unread_infos;
    u32 unread_warnings;
    u32 unread_errors;

    PlatformLogMessage* most_recent_log_message;
    Win32LogMemory* first_log_memory;

    HANDLE log_file;
};

global Win32State win32_state;

internal PLATFORM_ALLOCATE_MEMORY(win32_allocate_memory) {
    void* result = VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    return result;
}

internal PLATFORM_DEALLOCATE_MEMORY(win32_deallocate_memory) {
    if (memory) {
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

#define win32_log_print(log_level, format_string, ...) win32_log_print_internal(log_level, __FILE__, __FUNCTION__, __LINE__, format_string, ##__VA_ARGS__)
internal PLATFORM_LOG_PRINT(win32_log_print_internal) {
#if ASSERT_ON_LOG_ERROR
    if (log_level == LogLevel_Error) {
        assert(!"Asserted on LogLevel_Error");
    }
#endif

    va_list va_args;
    va_start(va_args, format_string);

    s32 text_size = stbsp_vsnprintf(0, 0, format_string, va_args);
    s32 null_terminated_text_size = text_size + 1;

    if (text_size > 0) {
        Win32LogMemory* memory = win32_state.first_log_memory;
        if (!memory || memory->used + null_terminated_text_size > WIN32_LOG_MEMORY_CHUNK_SIZE) {
            Win32LogMemory* new_memory = push_struct(&win32_state.platform_arena, Win32LogMemory, no_clear());

            new_memory->next = win32_state.first_log_memory;
            win32_state.first_log_memory = new_memory;

            new_memory->used = 0;

            memory = new_memory;
        }

        char* text_location = memory->memory + memory->used;

        s32 printed_size = stbsp_vsnprintf(text_location, null_terminated_text_size, format_string, va_args);
        if (printed_size == text_size) {
            memory->used += null_terminated_text_size;

            PlatformLogMessage* message = push_struct(&win32_state.platform_arena, PlatformLogMessage);

            message->next = win32_state.most_recent_log_message;
            win32_state.most_recent_log_message = message;

            message->file        = file;
            message->function    = function;
            message->text        = text_location;
            message->text_length = text_size;
            message->line        = line;
            message->level       = log_level;

            win32_state.unread_log_messages++;
            switch (message->level) {
                case LogLevel_Info: { win32_state.unread_infos++; } break;
                case LogLevel_Warn: { win32_state.unread_warnings++; } break;
                case LogLevel_Error: { win32_state.unread_errors++; } break;
            }
        } else {
            INVALID_CODE_PATH;
        }
    }

    va_end(va_args);
}

internal PLATFORM_GET_MOST_RECENT_LOG_MESSAGE(win32_get_most_recent_log_message) {
    PlatformLogMessage* result = win32_state.most_recent_log_message;
    win32_state.unread_log_messages = 0;
    win32_state.unread_infos = 0;
    win32_state.unread_warnings = 0;
    win32_state.unread_errors = 0;
    return result;
}

internal PLATFORM_GET_UNREAD_LOG_MESSAGES(win32_get_unread_log_messages) {
    u32 result = win32_state.unread_log_messages;
    if (infos) *infos = win32_state.unread_infos;
    if (warnings) *warnings = win32_state.unread_warnings;
    if (errors) *errors = win32_state.unread_errors;
    return result;
}

internal PLATFORM_ALLOCATE_TEXTURE(win32_allocate_texture) {
    GLuint handle = opengl_load_texture(&opengl_info, w, h, data);
    assert(sizeof(handle) <= sizeof(void*));
    return cast(void*) handle;
}

internal PLATFORM_DEALLOCATE_TEXTURE(win32_deallocate_texture) {
    opengl_unload_texture(cast(GLuint) handle);
}

internal PLATFORM_READ_ENTIRE_FILE(win32_read_entire_file) {
    EntireFile result = {};

    HANDLE file_handle = CreateFileA(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

    if (file_handle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER file_size;
        if (GetFileSizeEx(file_handle, &file_size)) {
            u32 file_size32 = safe_truncate_u64_u32(file_size.QuadPart);
            result.data = allocate(allocator, file_size32, no_clear());
            if (result.data) {
                DWORD bytes_read;
                if (ReadFile(file_handle, result.data, file_size32, &bytes_read, 0) && file_size32 == bytes_read) {
                    result.size = bytes_read;
                } else {
                    deallocate(allocator, result.data);
                    result.data = (void*)0;
                }
            } else {
                // TODO: Logging
            }
        } else {
            // TODO: Logging
        }

        CloseHandle(file_handle);
    } else {
        // TODO: Logging
    }

    if (file_handle == INVALID_HANDLE_VALUE || !result.size) {
        win32_log_print(LogLevel_Error, "Failed to read file '%s'", file_name);
    }

    return result;
}

internal PLATFORM_WRITE_ENTIRE_FILE(win32_write_entire_file) {
    b32 result = false;

    HANDLE file_handle = CreateFileA(file_name, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, NULL, NULL);

    if (file_handle != INVALID_HANDLE_VALUE) {
        DWORD bytes_written;
        if (WriteFile(file_handle, data, size, &bytes_written, NULL)) {
            // File written successfully
            result = (bytes_written == size);
        } else {
            win32_log_print(LogLevel_Error, "Failed to write file '%s'", file_name);
        }

        CloseHandle(file_handle);
    } else {
        win32_log_print(LogLevel_Error, "Failed to create file '%s'", file_name);
    }

    return result;
}

global s64 perf_count_frequency;
inline void win32_initialize_perf_counter() {
    LARGE_INTEGER perf_count_frequency_result;
    QueryPerformanceFrequency(&perf_count_frequency_result);
    perf_count_frequency = perf_count_frequency_result.QuadPart;
}

inline LARGE_INTEGER win32_get_clock() {
    LARGE_INTEGER result;
    QueryPerformanceCounter(&result);
    return result;
}

inline f32 win32_get_seconds_elapsed(LARGE_INTEGER start, LARGE_INTEGER end) {
    f32 result = cast(f32) (end.QuadPart - start.QuadPart) / cast(f32) perf_count_frequency;
    return result;
}

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_GET_STATE(XInputGetState_t); // @TODO: I really don't know what my convention is for types of functions, because different casing makes my usual style not work
typedef X_INPUT_SET_STATE(XInputSetState_t);

X_INPUT_GET_STATE(XInputGetStateStub) {
    return ERROR_DEVICE_NOT_CONNECTED;
}

X_INPUT_SET_STATE(XInputSetStateStub) {
    return ERROR_DEVICE_NOT_CONNECTED;
}

enum XInputVersion {
    XInput1_4,
    XInput1_3,
    XInput9_1_0,
};

struct XInputState {
    XInputVersion version;
    XInputGetState_t* GetState;
    XInputSetState_t* SetState;
};

internal b32 win32_init_xinput(XInputState* xinput_state) {
    b32 result = false;

    xinput_state->GetState = XInputGetStateStub;
    xinput_state->SetState = XInputSetStateStub;

    HMODULE xinput_lib = LoadLibraryA("xinput1_4.dll");
    if (xinput_lib) {
        xinput_state->version = XInput1_4;
    }

    if (!xinput_lib) {
        xinput_lib = LoadLibraryA("xinput1_3.dll");
        if (xinput_lib) {
            xinput_state->version = XInput1_3;
        }
    }

    if (!xinput_lib) {
        xinput_lib = LoadLibraryA("xinput9_1_0.dll");
        if (xinput_lib) {
            xinput_state->version = XInput9_1_0;
        }
    }

    if (xinput_lib) {
        result = true;
        xinput_state->GetState = cast(XInputGetState_t*) GetProcAddress(xinput_lib, "XInputGetState");
        xinput_state->SetState = cast(XInputSetState_t*) GetProcAddress(xinput_lib, "XInputSetState");
        win32_log_print(LogLevel_Info, "XInput: Successfully loaded XInput %s",
            xinput_state->version == XInput1_4 ? "1.4" :
            xinput_state->version == XInput1_3 ? "1.3" :
            xinput_state->version == XInput9_1_0 ? "9.1.0" :
            "(unknown version)"
        );
    } else {
        win32_log_print(LogLevel_Error, "Failed to load XInput");
    }

    return result;
}

typedef HRESULT WINAPI DirectSoundCreate_t(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter);

internal LPDIRECTSOUNDBUFFER win32_init_dsound(HWND window, u32 sample_rate, u32 buffer_size) {
    HMODULE dsound_lib = LoadLibraryA("dsound.dll");

    LPDIRECTSOUNDBUFFER secondary_buffer = 0;
    if (dsound_lib) {
        DirectSoundCreate_t* DirectSoundCreate = cast(DirectSoundCreate_t*) GetProcAddress(dsound_lib, "DirectSoundCreate");

        LPDIRECTSOUND dsound;
        if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(NULL, &dsound, NULL))) {
            WAVEFORMATEX wave_format = {};
            wave_format.wFormatTag = WAVE_FORMAT_PCM;
            wave_format.nChannels = 2;
            wave_format.nSamplesPerSec = sample_rate;
            wave_format.wBitsPerSample = 16;
            wave_format.nBlockAlign = (wave_format.nChannels*wave_format.wBitsPerSample) / 8;
            wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec*wave_format.nBlockAlign;

            if (SUCCEEDED(dsound->SetCooperativeLevel(window, DSSCL_PRIORITY))) {
                DSBUFFERDESC buffer_description = { sizeof(buffer_description) };
                buffer_description.dwFlags = DSBCAPS_PRIMARYBUFFER;

                LPDIRECTSOUNDBUFFER primary_buffer;
                if (SUCCEEDED(dsound->CreateSoundBuffer(&buffer_description, &primary_buffer, NULL))) {
                    if (SUCCEEDED(primary_buffer->SetFormat(&wave_format))) {
                        /* Success */
                    } else {
                        win32_log_print(LogLevel_Error, "DirectSound: Failed to set requested wave format");
                    }
                } else {
                    win32_log_print(LogLevel_Error, "DirectSound: Failed to create primary buffer");
                }
            } else {
                win32_log_print(LogLevel_Error, "DirectSound: Failed to set cooperative level to DSSCL_PRIORITY");
            }

            DSBUFFERDESC buffer_description = { sizeof(buffer_description) };
            buffer_description.dwFlags = DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_GLOBALFOCUS;
            buffer_description.dwBufferBytes = buffer_size;
            buffer_description.lpwfxFormat = &wave_format;

            if (SUCCEEDED(dsound->CreateSoundBuffer(&buffer_description, &secondary_buffer, NULL))) {
                win32_state.dsound_valid = true;
                win32_log_print(LogLevel_Info, "DirectSound: Successfully initialized");
            } else {
                win32_log_print(LogLevel_Error, "DirectSound: Failed to create secondary buffer");
            }
        } else {
            win32_log_print(LogLevel_Error, "DirectSound: Failed to load DirectSoundCreate");
        }
    } else {
        win32_log_print(LogLevel_Error, "DirectSound: Failed to load library");
    }

    if (!win32_state.dsound_valid) {
        win32_log_print(LogLevel_Error, "DirectSound: Failed to load DirectSound. Sound playback unavailable");
    }

    return secondary_buffer;
}

struct Win32SoundOutput {
    LPDIRECTSOUNDBUFFER buffer;
    u32 sample_rate;
    u32 bytes_per_sample;
    u32 buffer_size;
    u32 safety_bytes;
};

internal void win32_fill_sound_buffer(Win32SoundOutput* output, DWORD byte_to_lock, DWORD bytes_to_write, GameSoundOutputBuffer* source) {
    VOID* region1;
    DWORD region1_size;
    VOID* region2;
    DWORD region2_size;
    if (SUCCEEDED(output->buffer->Lock(byte_to_lock, bytes_to_write, &region1, &region1_size, &region2, &region2_size, NULL))) {
        s16* source_sample = source->samples;

        s16* dest_sample = cast(s16*) region1;
        u32 region1_sample_count = region1_size / output->bytes_per_sample;
        for (u32 sample_index = 0; sample_index < region1_sample_count; sample_index++) {
            *dest_sample++ = *source_sample++; // L
            *dest_sample++ = *source_sample++; // R
        }

        dest_sample = cast(s16*) region2;
        u32 region2_sample_count = region2_size / output->bytes_per_sample;
        for (u32 sample_index = 0; sample_index < region2_sample_count; sample_index++) {
            *dest_sample++ = *source_sample++; // L
            *dest_sample++ = *source_sample++; // R
        }

        output->buffer->Unlock(region1, region1_size, region2, region2_size);
    } else {
        INVALID_CODE_PATH;
    }
}

internal void win32_clear_sound_buffer(Win32SoundOutput* output) {
    VOID* region1;
    DWORD region1_size;
    VOID* region2;
    DWORD region2_size;
    if (SUCCEEDED(output->buffer->Lock(0, output->buffer_size, &region1, &region1_size, &region2, &region2_size, NULL))) {
        u8* dest_sample = cast(u8*) region1;
        for (DWORD byte_index = 0; byte_index < region1_size; byte_index++) {
            *dest_sample++ = 0;
        }
        dest_sample = cast(u8*) region2;
        for (DWORD byte_index = 0; byte_index < region2_size; byte_index++) {
            *dest_sample++ = 0;
        }
    }

    output->buffer->Unlock(region1, region1_size, region2, region2_size);
}

global WINDOWPLACEMENT global_window_position = { sizeof(global_window_position) };

internal void win32_toggle_fullscreen(HWND window) {
    // Raymond Chen's fullscreen toggle recipe:
    // http://blogs.msdn.com/b/oldnewthing/archive/2010/04/12/9994016.aspx
    DWORD style = GetWindowLong(window, GWL_STYLE);
    if (style & WS_OVERLAPPEDWINDOW) {
        MONITORINFO mi = { sizeof(mi) };
        if (GetWindowPlacement(window, &global_window_position) && GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi)) {
            SetWindowLong(window, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(
                window,
                HWND_TOP,
                mi.rcMonitor.left, mi.rcMonitor.top,
                mi.rcMonitor.right - mi.rcMonitor.left,
                mi.rcMonitor.bottom - mi.rcMonitor.top,
                SWP_NOOWNERZORDER|SWP_FRAMECHANGED
            );
        }
    } else {
        SetWindowLong(window, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(window, &global_window_position);
        SetWindowPos(window, NULL, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_FRAMECHANGED);
    }
}

internal void win32_process_keyboard_message(GameButtonState* new_state, b32 is_down) {
    if (is_down != new_state->is_down) {
        new_state->is_down = is_down;
        new_state->half_transition_count++;
    }
}

internal f32 win32_process_stick_axis(SHORT raw, SHORT deadzone) {
    f32 magnitude = 0.0f;
    if (raw > deadzone) {
        magnitude = cast(f32) (raw - deadzone) / cast(f32) (32767 - deadzone);
    } else if (raw < -deadzone) {
        magnitude = cast(f32) (raw + deadzone) / cast(f32) (32768 - deadzone);
    }
    return magnitude;
}

internal void win32_process_xinput_digital_button(GameButtonState* old_state, GameButtonState* new_state, WORD button_state, DWORD button_bit) {
    b32 is_down = (button_bit == (button_state & button_bit)) ? 1 : 0;
    if (is_down != new_state->is_down) {
        new_state->is_down = is_down;
        new_state->half_transition_count++;
    }
}

LRESULT CALLBACK win32_window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    LRESULT result = NULL;

    switch (message) {
        case WM_CLOSE:
        case WM_QUIT: {
            win32_state.running = false;
        } break;

        case WM_DESTROY: {
            // @Note: I'm not sure under which circumstances this happens.
            win32_state.running = false;
        } break;

        default: {
            result = DefWindowProcA(window, message, w_param, l_param);
        } break;
    }

    return result;
}

internal void win32_handle_remaining_messages(GameInput* input, BYTE* keyboard_state) {
    MSG message;
    while (PeekMessageA(&message, NULL, NULL, NULL, PM_REMOVE)) {
        switch (message.message) {
            case WM_CLOSE:
            case WM_QUIT: {
                input->quit_requested = true;
            } break;

            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP: {
                u32 vk_code = cast(u32) message.wParam;
                b32 was_down = (message.lParam & (1 << 30));
                b32 is_down = !(message.lParam & (1 << 31));
                b32 alt_is_down = (message.lParam & (1 << 29));

                switch (vk_code) {
                    case VK_OEM_3: { win32_process_keyboard_message(&input->tilde, is_down); } break;

                    case VK_F1:  { win32_process_keyboard_message(&input->debug_fkeys[1],  is_down); } break;
                    case VK_F2:  { win32_process_keyboard_message(&input->debug_fkeys[2],  is_down); } break;
                    case VK_F3:  { win32_process_keyboard_message(&input->debug_fkeys[3],  is_down); } break;
                    case VK_F4:  {
                        win32_process_keyboard_message(&input->debug_fkeys[4], is_down);
                        if (alt_is_down) {
                            input->quit_requested = true;
                        }
                    } break;
                    case VK_F5:  { win32_process_keyboard_message(&input->debug_fkeys[5],  is_down); } break;
                    case VK_F6:  { win32_process_keyboard_message(&input->debug_fkeys[6],  is_down); } break;
                    case VK_F7:  { win32_process_keyboard_message(&input->debug_fkeys[7],  is_down); } break;
                    case VK_F8:  { win32_process_keyboard_message(&input->debug_fkeys[8],  is_down); } break;
                    case VK_F9:  { win32_process_keyboard_message(&input->debug_fkeys[9],  is_down); } break;
                    case VK_F10: { win32_process_keyboard_message(&input->debug_fkeys[10], is_down); } break;
                    case VK_F11: { win32_process_keyboard_message(&input->debug_fkeys[11], is_down); } break;
                    case VK_F12: { win32_process_keyboard_message(&input->debug_fkeys[12], is_down); } break;
                }

                if (is_down && vk_code == VK_RETURN && alt_is_down && message.hwnd) {
                    win32_toggle_fullscreen(message.hwnd);
                } else if (input->event_mode) {
                    if (is_down) {
                        // @Note: first 8 bits: PKC code (which I've made identical to VK codes, so other platforms will get to have fun converting, but not us win32 folk)
                        assert(vk_code <= 0xFF);
                        u32 packed_event = vk_code;

                        // @Note: second 8 bits: Ascii translation
                        // @TODO: Unicode, keyboard layouts, that kind of robust stuff
                        u32 scan_code = (message.lParam >> 16) & 0xFFFF;
                        WORD ascii_char;
                        if (ToAscii(vk_code, scan_code, keyboard_state, &ascii_char, 0) == 1) {
                            packed_event |= (ascii_char & 0xFF) << 8;
                        }

                        input->event_buffer[input->event_count++] = packed_event;
                    }
                } else {
                    // @TODO: Handle key repeats for these?
                    if (vk_code >= 'A' && vk_code <= 'Z') {
                        win32_process_keyboard_message(&input->keys[vk_code - 'A'], is_down);
                    }

                    if (was_down == is_down) break;

                    GameController* keyboard_controller = &input->controllers[0];

                    GameConfig* config = &win32_state.config;
                    if (vk_code == config->up       || vk_code == config->alternate_up      ) win32_process_keyboard_message(&keyboard_controller->move_up,    is_down);
                    if (vk_code == config->left     || vk_code == config->alternate_left    ) win32_process_keyboard_message(&keyboard_controller->move_left,  is_down);
                    if (vk_code == config->down     || vk_code == config->alternate_down    ) win32_process_keyboard_message(&keyboard_controller->move_down,  is_down);
                    if (vk_code == config->right    || vk_code == config->alternate_right   ) win32_process_keyboard_message(&keyboard_controller->move_right, is_down);
                    if (vk_code == config->jump     || vk_code == config->alternate_jump    ) win32_process_keyboard_message(&keyboard_controller->jump,       is_down);
                    if (vk_code == config->interact || vk_code == config->alternate_interact) win32_process_keyboard_message(&keyboard_controller->interact,   is_down);

                    switch (vk_code) {
                        case VK_ESCAPE:  { win32_process_keyboard_message(&keyboard_controller->start, is_down); } break;
                        case VK_SPACE:   { win32_process_keyboard_message(&input->space, is_down); } break;
                        case VK_MENU:    { win32_process_keyboard_message(&input->alt,   is_down); } break;
                        case VK_SHIFT:   { win32_process_keyboard_message(&input->shift, is_down); } break;
                        case VK_CONTROL: { win32_process_keyboard_message(&input->ctrl,  is_down); } break;
                        case VK_DELETE:  { win32_process_keyboard_message(&input->del,   is_down); } break;
                    }
                }
            } break;

            default: {
                TranslateMessage(&message);
                DispatchMessageA(&message);
            } break;
        }
    }
}

internal void win32_output_image(GameRenderCommands* commands, HDC window_dc) {
    TemporaryMemory temp = begin_temporary_memory(&win32_state.platform_arena);

    SortEntry* sort_entries = cast(SortEntry*) commands->command_buffer;
    SortEntry* sort_temp_space = push_array(&win32_state.platform_arena, commands->sort_entry_count, SortEntry, no_clear());

    radix_sort(commands->sort_entry_count, sort_entries, sort_temp_space);

    opengl_render_commands(commands);
    SwapBuffers(window_dc);

    end_temporary_memory(temp);
}

inline b32 parse_config(GameConfig* config, String in_file) {
    b32 no_errors = true;

    String file = in_file;
    while (chars_left(&file)) {
        String line      = trim_spaces(advance_line(&file));

        if (!line.len || peek(line) == '#') {
            continue;
        }

        String key       = advance_legal_identifier(&line);
        String value     = trim_spaces(advance_to(&line, '#'));

        b32 found_matching_member = false;
        for (u32 member_index = 0; member_index < members_count(GameConfig) && !found_matching_member; member_index++) {
            MemberDefinition* member = members_of(GameConfig) + member_index;
            void** member_ptr = member_ptr(config, member);

            String member_name = wrap_string(member->name_length, member->name);

            if (strings_are_equal(member_name, key)) {
                found_matching_member = true;
                b32 successful_parse = true;

                if (member->flags & MetaMemberFlag_IsPointer) {
                    assert(!"I don't support any types that are pointers!!!");
                }

                switch (member->type) {
                    case meta_type(b32): {
                        if (strings_are_equal(value, string_literal("true"), StringMatch_CaseInsenitive) || strings_are_equal(value, string_literal("1"))) {
                            *(cast(b32*) member_ptr) = true;
                        } else if (strings_are_equal(value, string_literal("false"), StringMatch_CaseInsenitive) || strings_are_equal(value, string_literal("0"))) {
                            *(cast(b32*) member_ptr) = false;
                        } else {
                            successful_parse = false;
                        }
                    } break;

                    case meta_type(s8):
                    case meta_type(s16):
                    case meta_type(s32):
                    case meta_type(s64): {
                        s64 result = 0;
                        if (parse_s64(&value, &result)) {
                            switch (member->type) {
                                case meta_type(s8 ): { *(cast(s8 *) member_ptr) = cast(s8 ) CLAMP(result,  INT8_MIN,  INT8_MAX); } break;
                                case meta_type(s16): { *(cast(s16*) member_ptr) = cast(s16) CLAMP(result, INT16_MIN, INT16_MAX); } break;
                                case meta_type(s32): { *(cast(s32*) member_ptr) = cast(s32) CLAMP(result, INT32_MIN, INT32_MAX); } break;
                                case meta_type(s64): { *(cast(s64*) member_ptr) = result; } break;
                                default: { successful_parse = false; } break;
                            }
                        } else {
                            successful_parse = false;
                        }
                    } break;

                    case meta_type(u8):
                    case meta_type(u16):
                    case meta_type(u32):
                    case meta_type(u64): {
                        u64 result = 0;
                        if (parse_u64(&value, &result)) {
                            switch (member->type) {
                                case meta_type(u8 ): { *(cast(u8 *) member_ptr) = cast(u8 ) MIN(result,  UINT8_MAX); } break;
                                case meta_type(u16): { *(cast(u16*) member_ptr) = cast(u16) MIN(result, UINT16_MAX); } break;
                                case meta_type(u32): { *(cast(u32*) member_ptr) = cast(u32) MIN(result, UINT32_MAX); } break;
                                case meta_type(u64): { *(cast(u64*) member_ptr) = result; } break;
                                default: { successful_parse = false; } break;
                            }
                        } else if (member->type == meta_type(u8) || member->type == meta_type(char)) {
                            if (value.len != 1) {
                                log_print(LogLevel_Warn, "Found multiple characters in character value. Ignoring all but the first");
                            }
                            *(cast(char*) member_ptr) = peek(value);
                        } else {
                            successful_parse = false;
                        }
                    } break;

                    case meta_type(f32):
                    case meta_type(f64): {
                        f64 result = 0;
                        if (parse_f64(&value, &result)) {
                            switch (member->type) {
                                case meta_type(f32): { *(cast(f32*) member_ptr) = cast(f32) clamp(result, cast(f64) -FLT_MAX, cast(f64) FLT_MAX); } break;
                                case meta_type(f64): { *(cast(f64*) member_ptr) = result; } break;
                                default: { successful_parse = false; } break;
                            }
                        } else {
                            successful_parse = false;
                        }
                    } break;

                    case meta_type(String): {
                        *(cast(String*) member_ptr) = value;
                    } break;
                }

                if (!successful_parse) {
                    no_errors = false;
                    win32_log_print(LogLevel_Error, "Could not parse value '%.*s' for key '%.*s' of type '%s'",
                        PRINTF_STRING(value),
                        PRINTF_STRING(key),
                        meta_type_name(member->type)
                    );
                }
            }
        }

        if (!found_matching_member) {
            no_errors = false;
            win32_log_print(LogLevel_Error, "Found no matching config member for key '%.*s'", PRINTF_STRING(key));
        }
    }

    return no_errors;
}

inline void handle_config_file(char* config_file_name) {
    b32 should_read_config = !win32_state.config_file.len;

    WIN32_FILE_ATTRIBUTE_DATA data;
    if (GetFileAttributesExA(config_file_name, GetFileExInfoStandard, &data)) {
        u64 last_write_time = ULARGE_INTEGER { data.ftLastWriteTime.dwLowDateTime, data.ftLastWriteTime.dwHighDateTime }.QuadPart;
        if (last_write_time > win32_state.last_config_write_time) {
            should_read_config = true;
            win32_state.last_config_write_time = last_write_time;
        }
    }

    if (should_read_config) {
        if (win32_state.config_file.len) {
            deallocate(allocator(malloc_allocator, 0), win32_state.config_file.data);
            win32_state.config_file.len = 0;
        }

        win32_state.config_file = read_text_file(config_file_name, allocator(malloc_allocator, 0));
        if (win32_state.config_file.len) {
            if (parse_config(&win32_state.config, win32_state.config_file)) {
                win32_log_print(LogLevel_Info, "Config file '%s' loaded successfully", config_file_name);
            } else {
                win32_log_print(LogLevel_Error, "Config file '%s' loaded with errors", config_file_name);
            }
        } else {
            win32_log_print(LogLevel_Error, "Could not open config file '%s'", config_file_name);
        }
    }
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE previous_instance, LPSTR command_line, int show_code) {
    win32_initialize_perf_counter();

    win32_state.cursor_shown = true;

    size_t platform_storage_size = MEGABYTES(32);
    void* platform_storage = win32_allocate_memory(platform_storage_size);

    initialize_arena(&win32_state.platform_arena, platform_storage_size, platform_storage);

#if 0
    win32_state.log_file = CreateFileA("test_log.txt", GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, NULL, NULL);
    if (win32_state.log_file != INVALID_HANDLE_VALUE) {
        win32_state.log_file_valid = true;
    }
#endif

    char* config_file_name = "pulsar_config.pcf";
    handle_config_file(config_file_name);

    HCURSOR arrow_cursor = LoadCursorA(NULL, IDC_ARROW);

    WNDCLASSA window_class = {};
    window_class.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    window_class.lpfnWndProc = win32_window_proc;
    window_class.hInstance = instance;
    window_class.hCursor = NULL;
    window_class.hbrBackground = cast(HBRUSH) GetStockObject(BLACK_BRUSH);
    window_class.lpszClassName = "PulsarWindowClass";

    if (RegisterClassA(&window_class)) {
        HWND window = CreateWindowA(
            window_class.lpszClassName,
            "Pulsar",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT,
            NULL, NULL,
            instance,
            NULL
        );

        if (window) {
            if (win32_state.config.start_fullscreen) {
                win32_toggle_fullscreen(window);
            }

            SetCursor(arrow_cursor);
            ShowWindow(window, show_code);

            HDC window_dc = GetDC(window);
            HGLRC glrc = wgl_opengl_init(window_dc, &wgl_info, &opengl_info, win32_state.config.msaa_count);

            u32 monitor_refresh_rate = GetDeviceCaps(window_dc, VREFRESH);
            if (monitor_refresh_rate <= 1) {
                monitor_refresh_rate = 60;
            }

            f32 game_update_rate = cast(f32) monitor_refresh_rate;

            XInputState xinput_state;
            win32_state.xinput_valid = win32_init_xinput(&xinput_state);

            Win32SoundOutput sound_output = {};
            sound_output.sample_rate = 48000;
            sound_output.bytes_per_sample = 16;
            sound_output.bytes_per_sample = sizeof(s16)*2;
            sound_output.buffer_size = sound_output.sample_rate*sound_output.bytes_per_sample;

            sound_output.buffer = win32_init_dsound(window, sound_output.sample_rate, sound_output.buffer_size);

            GameSoundOutputBuffer sound_buffer = {};
            sound_buffer.channel_count = 2;
            sound_buffer.sample_rate = sound_output.sample_rate;
            sound_buffer.samples = cast(s16*) win32_allocate_memory(sound_output.buffer_size);

            if (win32_state.dsound_valid) {
                win32_clear_sound_buffer(&sound_output);
                sound_output.buffer->Play(0, 0, DSBPLAY_LOOPING);
            }

            GameRenderCommands render_commands = {};
            render_commands.command_buffer_size = MEGABYTES(win32_state.config.command_buffer_size_mb);
            render_commands.command_buffer = cast(u8*) win32_allocate_memory(render_commands.command_buffer_size);

            b32 sound_is_valid = false;

            size_t permanent_storage_size = MEGABYTES(cast(u64) win32_state.config.permanent_storage_size_mb);
            size_t transient_storage_size = MEGABYTES(cast(u64) win32_state.config.transient_storage_size_mb);
            void* permanent_storage = win32_allocate_memory(permanent_storage_size + transient_storage_size);
            void* transient_storage = cast(u8*) permanent_storage + permanent_storage_size;

            win32_log_print(LogLevel_Info, "Command Buffer Size:    %uMB", render_commands.command_buffer_size / 1024 / 1024);
            win32_log_print(LogLevel_Info, "Permanent Storage Size: %uMB", permanent_storage_size / 1024 / 1024);
            win32_log_print(LogLevel_Info, "Transient Storage Size: %uMB", transient_storage_size / 1024 / 1024);

            GameMemory game_memory = {};
            game_memory.permanent_storage_size = permanent_storage_size;
            game_memory.permanent_storage = permanent_storage;
            game_memory.transient_storage_size = transient_storage_size;
            game_memory.transient_storage = transient_storage;

            game_memory.platform_api.read_entire_file = win32_read_entire_file;
            game_memory.platform_api.write_entire_file = win32_write_entire_file;
            game_memory.platform_api.allocate = win32_allocate_memory;
            game_memory.platform_api.deallocate = win32_deallocate_memory;
            game_memory.platform_api.allocate_texture = win32_allocate_texture;
            game_memory.platform_api.deallocate_texture = win32_deallocate_texture;
            game_memory.platform_api.log_print = win32_log_print_internal;
            game_memory.platform_api.get_most_recent_log_message = win32_get_most_recent_log_message;
            game_memory.platform_api.get_unread_log_messages = win32_get_unread_log_messages;

            GameInput old_input_ = {};
            GameInput new_input_ = {};
            GameInput* old_input = &old_input_;
            GameInput* new_input = &new_input_;

            LARGE_INTEGER start_counter = win32_get_clock();
            f32 last_frame_time = 0.0f;
            u32 last_render_commands = 0;
            b32 last_frame_time_is_valid = false;

            DWORD previous_padded_write_cursor = 0;

            BYTE keyboard_state[256];

            win32_state.running = true;
            while (win32_state.running) {
                if (last_frame_time_is_valid) {
                    DebugFrameTimeHistory* frame_history = &global_debug_info.frame_history;
                    u32 frame_index = 0;
                    if (frame_history->valid_entry_count < ARRAY_COUNT(frame_history->history)) {
                        frame_index = frame_history->first_valid_entry + frame_history->valid_entry_count++;
                    } else {
                        frame_index = ++frame_history->first_valid_entry + frame_history->valid_entry_count;
                    }
                    frame_history->first_valid_entry %= ARRAY_COUNT(frame_history->history);
                    frame_index %= ARRAY_COUNT(frame_history->history);
                    DebugFrameInfo* frame = frame_history->history + frame_index;
                    frame->time = last_frame_time;
                    frame->render_commands = last_render_commands;
                }

                handle_config_file(config_file_name);
                game_memory.config = win32_state.config;

                //
                // DirectSound settings that can be tweaked while the game is running
                //

                // @Note: sound_latency_ms defines how long game_get_sound can take without producing any audio glitches (and is your audio latency, of course)
                f32 sound_latency_ms = win32_state.config.directsound_latency_ms;
                sound_output.safety_bytes = cast(u32) (sound_output.sample_rate*sound_output.bytes_per_sample*sound_latency_ms*0.001f);

                // @Note: sound_overdraw_frames defines (@TODO: should define - I haven't tested it properly) the number of frames you can miss before the game starts having audible
                // audio glitches.
                u32 sound_safety_frames = win32_state.config.directsound_safety_frames;
                u32 max_dsound_byte_overdraw = sound_output.bytes_per_sample*cast(u32) (sound_output.sample_rate*((1 + cast(f32) sound_safety_frames)*(1.0f / game_update_rate)));

                //
                // Rendering setup
                //

                RECT window_rect;
                GetClientRect(window, &window_rect);

                u32 width = window_rect.right - window_rect.left;
                u32 height = window_rect.bottom - window_rect.top;

                render_commands.sort_entry_count = 0;
                render_commands.first_command = render_commands.command_buffer_size;
                render_commands.width = width;
                render_commands.height = height;

                //
                // Input
                //

                {
                    b32 new_in_focus = (GetForegroundWindow() == window);
                    win32_state.focus_changed = new_in_focus != win32_state.in_focus;
                    win32_state.in_focus = new_in_focus;
                }

                new_input->event_mode = old_input->event_mode;
                new_input->update_rate = game_update_rate;
                new_input->frame_dt = (1.0f / new_input->update_rate)*win32_state.config.simulation_rate;
                new_input->in_focus = win32_state.in_focus;
                new_input->focus_changed = win32_state.focus_changed;
                new_input->show_cursor = old_input->show_cursor;

                for (u32 controller_index = 0; controller_index < ARRAY_COUNT(new_input->controllers); controller_index++) {
                    GameController* old_controller = &old_input->controllers[controller_index];
                    GameController* new_controller = &new_input->controllers[controller_index];
                    for (u32 button_index = 0; button_index < ARRAY_COUNT(new_controller->buttons); button_index++) {
                        new_controller->buttons[button_index].is_down = old_controller->buttons[button_index].is_down;
                        new_controller->buttons[button_index].half_transition_count = 0;
                    }
                }

                for (u32 fkey_index = 0; fkey_index < ARRAY_COUNT(new_input->debug_fkeys); fkey_index++) {
                    new_input->debug_fkeys[fkey_index].is_down = old_input->debug_fkeys[fkey_index].is_down;
                    new_input->debug_fkeys[fkey_index].half_transition_count = 0;
                }

                for (u32 key_index = 0; key_index < ARRAY_COUNT(new_input->keys); key_index++) {
                    new_input->keys[key_index].is_down = old_input->keys[key_index].is_down;
                    new_input->keys[key_index].half_transition_count = 0;
                }

                for (u32 key_index = 0; key_index < ARRAY_COUNT(new_input->editor_keys); key_index++) {
                    new_input->editor_keys[key_index].is_down = old_input->editor_keys[key_index].is_down;
                    new_input->editor_keys[key_index].half_transition_count = 0;
                }

                POINT mouse_position;
                GetCursorPos(&mouse_position);
                ScreenToClient(window, &mouse_position);
                new_input->mouse_x = CLAMP(mouse_position.x, 0, cast(s32) width);
                new_input->mouse_y = CLAMP((height - 1) - mouse_position.y, 0, cast(s32) height);
                new_input->mouse_z = 0; // @TODO: Mousewheel support

                DWORD win32_button_id[PlatformMouseButton_Count] = {
                    VK_LBUTTON,
                    VK_MBUTTON,
                    VK_RBUTTON,
                    VK_XBUTTON1,
                    VK_XBUTTON2,
                };

                for (u32 button_index = 0; button_index < ARRAY_COUNT(new_input->mouse_buttons); button_index++) {
                    new_input->mouse_buttons[button_index].is_down = old_input->mouse_buttons[button_index].is_down;
                    new_input->mouse_buttons[button_index].half_transition_count = 0;

                    win32_process_keyboard_message(&new_input->mouse_buttons[button_index], (GetKeyState(win32_button_id[button_index]) & (1 << 15)) != 0);
                }

                new_input->event_count = 0;
                GetKeyboardState(keyboard_state);
                win32_handle_remaining_messages(new_input, keyboard_state);

                if (win32_state.xinput_valid) {
                    for (u32 controller_index = 0; controller_index < XUSER_MAX_COUNT; controller_index++) {
                        GameController* old_controller = &old_input->controllers[1 + controller_index];
                        GameController* new_controller = &new_input->controllers[1 + controller_index];
                        XINPUT_STATE controller_state;
                        if (ERROR_SUCCESS == xinput_state.GetState(controller_index, &controller_state)) {
                            XINPUT_GAMEPAD* pad = &controller_state.Gamepad;
                            f32 stick_x = win32_process_stick_axis(pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                            f32 stick_y = win32_process_stick_axis(pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

                            f32 stick_threshold = 0.5f;

                            WORD move_left  = ((pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) ? 1 : 0) || stick_x < -stick_threshold;
                            WORD move_right = ((pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? 1 : 0) || stick_x >  stick_threshold;
                            WORD move_up    = ((pad->wButtons & XINPUT_GAMEPAD_DPAD_UP   ) ? 1 : 0) || stick_y >  stick_threshold;
                            WORD move_down  = ((pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) ? 1 : 0) || stick_y < -stick_threshold;

                            win32_process_xinput_digital_button(&old_controller->move_left,  &new_controller->move_left,  move_left,  1);
                            win32_process_xinput_digital_button(&old_controller->move_right, &new_controller->move_right, move_right, 1);
                            win32_process_xinput_digital_button(&old_controller->move_up,    &new_controller->move_up,    move_up,    1);
                            win32_process_xinput_digital_button(&old_controller->move_down,  &new_controller->move_down,  move_down,  1);

                            win32_process_xinput_digital_button(&old_controller->jump, &new_controller->jump, pad->wButtons, XINPUT_GAMEPAD_A);
                            win32_process_xinput_digital_button(&old_controller->interact, &new_controller->interact, pad->wButtons, XINPUT_GAMEPAD_A);

                            win32_process_xinput_digital_button(&old_controller->start, &new_controller->start, pad->wButtons, XINPUT_GAMEPAD_START);
                            win32_process_xinput_digital_button(&old_controller->back, &new_controller->back, pad->wButtons, XINPUT_GAMEPAD_BACK);
                        } else {
                            // Not connected
                        }
                    }
                }

                GameController* aggregate_controller = &new_input->controller;
                zero_struct(*aggregate_controller);

                for (u32 controller_index = 0; controller_index < ARRAY_COUNT(new_input->controllers); controller_index++) {
                    GameController* controller = &new_input->controllers[controller_index];
                    for (u32 button_index = 0; button_index < ARRAY_COUNT(controller->buttons); button_index++) {
                        aggregate_controller->buttons[button_index].is_down |= controller->buttons[button_index].is_down;
                        aggregate_controller->buttons[button_index].half_transition_count += controller->buttons[button_index].half_transition_count;
                    }
                }

                //
                // Game Update and Render
                //

                game_update_and_render(&game_memory, new_input, &render_commands);

                //
                // Game Sound
                //

                if (win32_state.dsound_valid) {
                    DWORD play_cursor, write_cursor;
                    if (SUCCEEDED(sound_output.buffer->GetCurrentPosition(&play_cursor, &write_cursor))) {
                        DWORD padded_write_cursor = (write_cursor + sound_output.safety_bytes) % sound_output.buffer_size;

                        if (!sound_is_valid) {
                            previous_padded_write_cursor = padded_write_cursor;
                            sound_is_valid = true;
                        }

                        DWORD bytes_to_write;
                        {
                            DWORD unwrapped_play_cursor = play_cursor;
                            if (unwrapped_play_cursor < padded_write_cursor) {
                                unwrapped_play_cursor += sound_output.buffer_size;
                            }

                            bytes_to_write = MIN(unwrapped_play_cursor - padded_write_cursor, max_dsound_byte_overdraw);
                        }

                        DWORD samples_committed;
                        {
                            DWORD unwrapped_padded_write_cursor = padded_write_cursor;
                            if (unwrapped_padded_write_cursor < previous_padded_write_cursor) {
                                unwrapped_padded_write_cursor += sound_output.buffer_size;
                            }

                            samples_committed = (unwrapped_padded_write_cursor - previous_padded_write_cursor) / sound_output.bytes_per_sample;
                        }

                        sound_buffer.samples_committed = samples_committed;
                        sound_buffer.samples_to_write  = bytes_to_write / sound_output.bytes_per_sample;

                        game_get_sound(&game_memory, &sound_buffer);

                        DWORD byte_to_lock = padded_write_cursor;

                        win32_fill_sound_buffer(&sound_output, byte_to_lock, bytes_to_write, &sound_buffer);

                        previous_padded_write_cursor = padded_write_cursor;
                    }
                }

                //
                // VBlank
                // @TODO: Handle frame timing when vsync is not available / enabled.
                //

                win32_output_image(&render_commands, window_dc);

                game_post_render(&game_memory, new_input, &render_commands);

                win32_state.config = game_memory.config;

                b32 cursor_state_changed = (new_input->show_cursor != win32_state.cursor_shown);
                b32 cursor_is_in_window  = (mouse_position.x >= window_rect.left && mouse_position.x < window_rect.right ) &&
                                           (mouse_position.y >= window_rect.top  && mouse_position.y < window_rect.bottom);
                cursor_state_changed |= (cursor_is_in_window != win32_state.cursor_is_in_window);

                if (cursor_state_changed) {
                    if (new_input->show_cursor) {
                        SetCursor(arrow_cursor);
                    } else {
                        SetCursor(NULL);
                    }
                    win32_state.cursor_shown        = new_input->show_cursor;
                    win32_state.cursor_is_in_window = cursor_is_in_window;
                }

                last_render_commands = render_commands.sort_entry_count;

                LARGE_INTEGER end_counter = win32_get_clock();
                last_frame_time = win32_get_seconds_elapsed(start_counter, end_counter);
                last_frame_time_is_valid = true;
                start_counter = end_counter;

                if (new_input->quit_requested) {
                    win32_state.running = false;
                } else {
                    GameInput* temp = new_input;
                    new_input = old_input;
                    old_input = temp;
                }
            }
        } else {
            // @TODO: Some kind of logging?
            INVALID_CODE_PATH;
        }
    }

    return 0;
}
