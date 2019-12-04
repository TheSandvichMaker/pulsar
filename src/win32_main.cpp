#include <windows.h>
#include <gl/gl.h>

#include "win32_main.h"

global b32 running;

LRESULT CALLBACK win32_window_proc(
  HWND   window,
  UINT   message,
  WPARAM w_param,
  LPARAM l_param
) {
    LRESULT result = NULL;

    switch (message) {
        case WM_CLOSE:
        case WM_DESTROY:
        case WM_QUIT: {
            running = false;
        } break;

        default: {
            result = DefWindowProcA(window, message, w_param, l_param);
        } break;
    }

    return result;
}

int CALLBACK WinMain(
  HINSTANCE instance,
  HINSTANCE previous_instance,
  LPSTR     command_line,
  int       show_code
) {
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
            HDC window_dc = GetDC(window);

            PIXELFORMATDESCRIPTOR desired_pixel_format = {};
            desired_pixel_format.nVersion = 1;
            desired_pixel_format.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
            desired_pixel_format.iPixelType = PFD_TYPE_RGBA;
            desired_pixel_format.cColorBits = 32; // TODO: Should this include or exclude the alpha bits? Who knows!? MSDN doesn't!
            desired_pixel_format.cAlphaBits = 8;
            desired_pixel_format.iLayerType = PFD_MAIN_PLANE;
            int suggested_pixel_format_index = ChoosePixelFormat(window_dc, &desired_pixel_format);

            PIXELFORMATDESCRIPTOR suggested_pixel_format;
            DescribePixelFormat(window_dc, suggested_pixel_format_index, sizeof(suggested_pixel_format), &suggested_pixel_format);
            SetPixelFormat(window_dc, suggested_pixel_format_index, &suggested_pixel_format);

            HGLRC glrc = wglCreateContext(window_dc);
            if (wglMakeCurrent(window_dc, glrc)) {
                
            } else {
                INVALID_CODE_PATH;
            }

            ShowWindow(window, show_code);

            running = true;
            while (running) {
                MSG message;
                while (PeekMessageA(&message, NULL, NULL, NULL, PM_REMOVE)) {
                    switch (message.message) {
                        case WM_CLOSE:
                        case WM_QUIT: {
                            running = false;
                        } break;

                        default: {
                            TranslateMessage(&message);
                            DispatchMessageA(&message);
                        } break;
                    }
                }
            }
        } else {
            // TODO: Do something about the fact window creation failed?
            INVALID_CODE_PATH;
        }
    }

    return 0;
}
