/* Big List of @TODOs

 * Find (lower latency) alternative to DirectSound, WASAPI?
 * Decide on asset loading strategy (pack files?)

*/

#include <windows.h>
#include <dsound.h>
// #include <mmdeviceapi.h>
// #include <audioclient.h>
#include <gl/gl.h>
#include <stdio.h>

#include "common.h"
#include "memory_arena.h"
#include "platform_bridge.h"

#include "string.h"
#include "math.h"
#include "file_io.h"

#include "opengl.h"
#include "win32_opengl.h"

#include "opengl.cpp"
#include "win32_opengl.cpp"

global WglInfo wgl_info;
global OpenGLInfo opengl_info;

#include "game_main.cpp"

global b32 running;
global b32 in_focus;
global b32 focus_changed;

internal PLATFORM_ALLOCATE_MEMORY(win32_allocate_memory) {
    void* result = VirtualAlloc(NULL, size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    return result;
}

internal PLATFORM_DEALLOCATE_MEMORY(win32_deallocate_memory) {
    if (memory) {
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

internal PLATFORM_READ_ENTIRE_FILE(win32_read_entire_file) {
    EntireFile result = {};

    HANDLE file_handle = CreateFileA(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

    if (file_handle != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER file_size;
        if (GetFileSizeEx(file_handle, &file_size)) {
            u32 file_size32 = safe_truncate_u64u32(file_size.QuadPart);
            result.data = VirtualAlloc(NULL, file_size32, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            if (result.data) {
                DWORD bytes_read;
                if (ReadFile(file_handle, result.data, file_size32, &bytes_read, 0) && file_size32 == bytes_read) {
                    result.size = bytes_read;
                } else {
                    win32_deallocate_memory(result.data);
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

    return result;
}

internal b32 win32_write_entire_file(char* file_name, u32 size, void* data) {
    b32 result = false;

    HANDLE file_handle = CreateFileA(file_name, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, NULL, NULL);

    if (file_handle != INVALID_HANDLE_VALUE) {
        DWORD bytes_written;
        if (WriteFile(file_handle, data, size, &bytes_written, NULL)) {
            // File read successfully
            result = (bytes_written == size);
        } else {
            // TODO: Logging
        }

        CloseHandle(file_handle);
    } else {
        // TODO: Logging
    }

    return result;
}
typedef HRESULT WINAPI Typedef_DirectSoundCreate(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter);

internal LPDIRECTSOUNDBUFFER win32_init_dsound(HWND window, u32 sample_rate, u32 buffer_size) {
    HMODULE dsound_lib = LoadLibraryA("dsound.dll");

    LPDIRECTSOUNDBUFFER secondary_buffer = 0;
    if (dsound_lib) {
        Typedef_DirectSoundCreate* dsound_create = (Typedef_DirectSoundCreate*)GetProcAddress(dsound_lib, "DirectSoundCreate");

        LPDIRECTSOUND dsound;
        if (dsound_create && SUCCEEDED(dsound_create(NULL, &dsound, NULL))) {
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
                        // @Note: Success
                    } else {
                        // @TODO: What to do in failure cases?
                    }
                } else {
                    // @TODO: What to do in failure cases?
                }
            } else {
                // @TODO: What to do in failure cases?
            }

            DSBUFFERDESC buffer_description = { sizeof(buffer_description) };
            buffer_description.dwFlags = DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_GLOBALFOCUS;
            buffer_description.dwBufferBytes = buffer_size;
            buffer_description.lpwfxFormat = &wave_format;

            if (SUCCEEDED(dsound->CreateSoundBuffer(&buffer_description, &secondary_buffer, NULL))) {
                // @Note: Success
            }
        } else {
            // @TODO: What to do in failure cases?
        }
    } else {
        // @TODO: What to do in failure cases?
    }

    return secondary_buffer;
}

struct Win32SoundOutput {
    LPDIRECTSOUNDBUFFER buffer;
    u32 sample_rate;
    u32 running_sample_index;
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
            output->running_sample_index++;
        }

        dest_sample = cast(s16*) region2;
        u32 region2_sample_count = region2_size / output->bytes_per_sample;
        for (u32 sample_index = 0; sample_index < region2_sample_count; sample_index++) {
            *dest_sample++ = *source_sample++; // L
            *dest_sample++ = *source_sample++; // R
            output->running_sample_index++;
        }

        output->buffer->Unlock(region1, region1_size, region2, region2_size);
    }
}

internal void win32_fill_sound_buffer_with_test_tone(Win32SoundOutput* output) {
    VOID* region1;
    DWORD region1_size;
    VOID* region2;
    DWORD region2_size;
    if (SUCCEEDED(output->buffer->Lock(0, output->buffer_size, &region1, &region1_size, &region2, &region2_size, NULL))) {
        assert(region2_size == 0);

        s16* dest_sample = cast(s16*) region1;
        u32 sample_count = output->buffer_size / output->bytes_per_sample;
        for (u32 sample_index = 0; sample_index < sample_count; sample_index++) {
            s16 test_tone = ((sample_index / 64) & 1) == 0 ? 8192 : -8192;
            *dest_sample++ = test_tone; // L
            *dest_sample++ = test_tone; // R
        }

        output->buffer->Unlock(region1, region1_size, region2, region2_size);
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
    if (in_focus) {
        if (new_state->is_down != is_down) {
            new_state->is_down = is_down;
            new_state->half_transition_count++;
        }
    }
}

LRESULT CALLBACK win32_window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    LRESULT result = NULL;

    switch (message) {
        case WM_CLOSE:
        case WM_QUIT: {
            running = false;
        } break;

        case WM_DESTROY: {
            // @Note: I'm not sure under which circumstances this happens.
            running = false;
        } break;

        default: {
            result = DefWindowProcA(window, message, w_param, l_param);
        } break;
    }

    return result;
}

internal void win32_handle_remaining_messages(GameInput* input) {
    MSG message;
    while (PeekMessageA(&message, NULL, NULL, NULL, PM_REMOVE)) {
        switch (message.message) {
            case WM_CLOSE:
            case WM_QUIT: {
                running = false;
            } break;

            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP: {
                u32 vk_code = (u32)message.wParam;
                b32 was_down = (message.lParam & (1 << 30));
                b32 is_down = !(message.lParam & (1 << 31));
                b32 alt_is_down = (message.lParam & (1 << 29));

                if (was_down == is_down) break;

                switch (vk_code) {
                    case VK_ESCAPE: {
                        running = false;
                    } break;

                    case VK_F1:  { win32_process_keyboard_message(&input->debug_fkeys[1],  is_down); } break;
                    case VK_F2:  { win32_process_keyboard_message(&input->debug_fkeys[2],  is_down); } break;
                    case VK_F3:  { win32_process_keyboard_message(&input->debug_fkeys[3],  is_down); } break;
                    case VK_F4: {
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

                    case 'W': { win32_process_keyboard_message(&input->controller.move_up, is_down); } break;
                    case 'A': { win32_process_keyboard_message(&input->controller.move_left, is_down); } break;
                    case 'S': { win32_process_keyboard_message(&input->controller.move_down, is_down); } break;
                    case 'D': { win32_process_keyboard_message(&input->controller.move_right, is_down); } break;

                    case VK_UP: { win32_process_keyboard_message(&input->controller.action_up, is_down); } break;
                    case VK_LEFT: { win32_process_keyboard_message(&input->controller.action_left, is_down); } break;
                    case VK_DOWN: { win32_process_keyboard_message(&input->controller.action_down, is_down); } break;
                    case VK_RIGHT: { win32_process_keyboard_message(&input->controller.action_right, is_down); } break;

                    case VK_RETURN: {
                        if (is_down && alt_is_down) {
                            if (message.hwnd) {
                                win32_toggle_fullscreen(message.hwnd);
                            }
                        }
                    } break;
                }
            } break;

#if 0
            @TODO: This seems like a lot of work when the GetKeyState version works just as well.
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP: {
                u32 mk_code = (u32)message.wParam;

                case MK_LBUTTON: { win32_process_keyboard_message(&input->mouse_buttons[PlatformMouseButton_Left], is_down); } break;
                case MK_RBUTTON: { win32_process_keyboard_message(&input->mouse_buttons[PlatformMouseButton_Right], is_down); } break;
                case MK_MBUTTON: { win32_process_keyboard_message(&input->mouse_buttons[PlatformMouseButton_Middle], is_down); } break;
                case MK_XBUTTON1: { win32_process_keyboard_message(&input->mouse_buttons[PlatformMouseButton_Extended0], is_down); } break;
                case MK_XBUTTON2: { win32_process_keyboard_message(&input->mouse_buttons[PlatformMouseButton_Extended1], is_down); } break;
            } break;
#endif

            default: {
                TranslateMessage(&message);
                DispatchMessageA(&message);
            } break;
        }
    }
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE previous_instance, LPSTR command_line, int show_code) {
    WNDCLASSA window_class = {};
    window_class.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    window_class.lpfnWndProc = win32_window_proc;
    window_class.hInstance = instance;
    window_class.hCursor = LoadCursorA(NULL, IDC_ARROW);
    window_class.hbrBackground = cast(HBRUSH) GetStockObject(BLACK_BRUSH);
    window_class.lpszClassName = "Win32WindowClass";

    if (RegisterClassA(&window_class)) {
        HWND window = CreateWindowA(
            window_class.lpszClassName,
            "Win32 Test Window",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT,
            NULL, NULL,
            instance,
            NULL
        );

        if (window) {
            ShowWindow(window, show_code);

            HDC window_dc = GetDC(window);
            HGLRC glrc = wgl_opengl_init(window_dc, &wgl_info, &opengl_info);

            u32 monitor_refresh_rate = GetDeviceCaps(window_dc, VREFRESH);
            if (monitor_refresh_rate <= 1) {
                monitor_refresh_rate = 60;
            }

            f32 game_update_rate = (f32)monitor_refresh_rate;

            Win32SoundOutput sound_output = {};
            sound_output.sample_rate = 48000;
            sound_output.bytes_per_sample = 16;
            sound_output.bytes_per_sample = sizeof(s16)*2;
            sound_output.buffer_size = sound_output.sample_rate*sound_output.bytes_per_sample;
            sound_output.safety_bytes = cast(u32) (0.5f*(cast(f32) (sound_output.sample_rate*sound_output.bytes_per_sample) / game_update_rate));

            sound_output.buffer = win32_init_dsound(window, sound_output.sample_rate, sound_output.buffer_size);

            GameSoundOutputBuffer sound_buffer = {};
            sound_buffer.channel_count = 2;
            sound_buffer.sample_rate = sound_output.sample_rate;
            sound_buffer.samples = cast(s16*) win32_allocate_memory(sound_output.buffer_size);

            win32_clear_sound_buffer(&sound_output);
            sound_output.buffer->Play(0, 0, DSBPLAY_LOOPING);

            b32 sound_is_valid = false;

            size_t permanent_storage_size = MEGABYTES(512);
            size_t transient_storage_size = GIGABYTES(1);
            void* permanent_storage = win32_allocate_memory(permanent_storage_size + transient_storage_size);
            void* transient_storage = cast(u8*) permanent_storage + permanent_storage_size;

            GameMemory game_memory = {};
            game_memory.permanent_storage_size = permanent_storage_size;
            game_memory.permanent_storage = permanent_storage;
            game_memory.transient_storage_size = transient_storage_size;
            game_memory.transient_storage = transient_storage;

            game_memory.platform_api.read_entire_file = win32_read_entire_file;
            game_memory.platform_api.write_entire_file = win32_write_entire_file;
            game_memory.platform_api.allocate = win32_allocate_memory;
            game_memory.platform_api.deallocate = win32_deallocate_memory;

            GameInput old_input_ = {};
            GameInput new_input_ = {};
            GameInput* old_input = &old_input_;
            GameInput* new_input = &new_input_;

            running = true;
            while (running) {
                RECT window_rect;
                GetClientRect(window, &window_rect);

                u32 width = window_rect.right - window_rect.left;
                u32 height = window_rect.bottom - window_rect.top;

                //
                // Input
                //

                b32 new_in_focus = (GetForegroundWindow() == window);
                focus_changed = new_in_focus != in_focus;
                in_focus = new_in_focus;

                new_input->frame_dt = 1.0f / game_update_rate;
                new_input->in_focus = in_focus;
                new_input->focus_changed = focus_changed;

                GameController* old_controller = &old_input->controller;
                GameController* new_controller = &new_input->controller;
                for (u32 button_index = 0; button_index < ARRAY_COUNT(new_controller->buttons); button_index++) {
                    new_controller->buttons[button_index].is_down = old_controller->buttons[button_index].is_down;
                    new_controller->buttons[button_index].half_transition_count = 0;
                }

                POINT mouse_position;
                GetCursorPos(&mouse_position);
                ScreenToClient(window, &mouse_position);
                new_input->mouse_x = mouse_position.x;
                new_input->mouse_y = (height - 1) - mouse_position.y;
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

                win32_handle_remaining_messages(new_input);

                //
                // Game Update and Render
                //

                game_update_and_render(&game_memory, new_input, width, height);

                //
                // Game Sound
                //

                DWORD play_cursor, write_cursor;
                if (SUCCEEDED(sound_output.buffer->GetCurrentPosition(&play_cursor, &write_cursor))) {
                    if (!sound_is_valid) {
                        sound_is_valid = true;
                        sound_output.running_sample_index = write_cursor / sound_output.bytes_per_sample;
                    }
                    DWORD byte_to_lock = (sound_output.running_sample_index*sound_output.bytes_per_sample) % sound_output.buffer_size;

                    DWORD expected_sound_bytes_per_frame = cast(DWORD) ((cast(f32) sound_output.sample_rate * cast(f32) sound_output.bytes_per_sample) / game_update_rate);
                    DWORD expected_frame_boundary_byte = play_cursor + expected_sound_bytes_per_frame;

                    DWORD safe_write_cursor = write_cursor;
                    if (safe_write_cursor < play_cursor) {
                        safe_write_cursor += sound_output.buffer_size;
                    }
                    assert(safe_write_cursor >= play_cursor);
                    safe_write_cursor += sound_output.safety_bytes;

                    // @TODO: This is likely never true under DirectSound
                    b32 low_latency = safe_write_cursor < expected_frame_boundary_byte;

                    DWORD target_cursor = 0;
                    if (low_latency) {
                        // assert(!"It happened!");
                        target_cursor = (expected_frame_boundary_byte + expected_sound_bytes_per_frame);
                    } else {
                        target_cursor = (write_cursor + expected_sound_bytes_per_frame + sound_output.safety_bytes);
                    }
                    target_cursor %= sound_output.buffer_size;

                    DWORD bytes_to_write = 0;
                    if (byte_to_lock > target_cursor) {
                        bytes_to_write = sound_output.buffer_size - byte_to_lock + target_cursor;
                    } else {
                        bytes_to_write = target_cursor - byte_to_lock;
                    }

                    sound_buffer.sample_count = bytes_to_write / sound_output.bytes_per_sample;
                    bytes_to_write = sound_buffer.sample_count*sound_output.bytes_per_sample;

                    game_get_sound(&game_memory, &sound_buffer);

                    win32_fill_sound_buffer(&sound_output, byte_to_lock, bytes_to_write, &sound_buffer);
                }

                //
                // VBlank
                // @TODO: Handle frame timing when vsync is not available / enabled.
                //

                SwapBuffers(window_dc);

                //

                GameInput* temp = new_input;
                new_input = old_input;
                old_input = temp;
            }
        } else {
            // @TODO: Some kind of logging?
            INVALID_CODE_PATH;
        }
    }

    return 0;
}
