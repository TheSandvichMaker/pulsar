internal void wgl_set_pixel_format(HDC window_dc, WglInfo* wgl_info) {
    int suggested_pixel_format_index = 0;
    GLuint number_of_extended_formats_found = 0;

    if (wglChoosePixelFormatARB) {
        int always_available_attributes[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
        };

        const int number_of_extended_attributes = 3;
        int attribute_list[ARRAY_COUNT(always_available_attributes) + 2*number_of_extended_attributes + 1] = {};

        int attribute_cursor = 0;
        for (; attribute_cursor < ARRAY_COUNT(always_available_attributes); attribute_cursor++) {
            attribute_list[attribute_cursor] = always_available_attributes[attribute_cursor];
        }

        if (wgl_info) {
            if (wgl_info->WGL_EXT_framebuffer_sRGB) {
                attribute_list[attribute_cursor++] = WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB;
                attribute_list[attribute_cursor++] = GL_TRUE;
            }
            if (wgl_info->WGL_ARB_multisample) {
                attribute_list[attribute_cursor++] = WGL_SAMPLE_BUFFERS_ARB;
                attribute_list[attribute_cursor++] = 1;
                attribute_list[attribute_cursor++] = WGL_SAMPLES_ARB;
                attribute_list[attribute_cursor++] = 4;
            }
        }
        assert(attribute_list[attribute_cursor] == 0);

        wglChoosePixelFormatARB(window_dc, attribute_list, 0, 1, &suggested_pixel_format_index, &number_of_extended_formats_found);
    }

    if (number_of_extended_formats_found == 0) {
        PIXELFORMATDESCRIPTOR desired_pixel_format = {};
        desired_pixel_format.nVersion = 1;
        desired_pixel_format.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
        desired_pixel_format.iPixelType = PFD_TYPE_RGBA;
        desired_pixel_format.cColorBits = 32; // @TODO: Should this include or exclude the alpha bits? Who knows!? MSDN doesn't!
        desired_pixel_format.cAlphaBits = 8;
        desired_pixel_format.cDepthBits = 24;
        desired_pixel_format.cStencilBits = 8;
        desired_pixel_format.iLayerType = PFD_MAIN_PLANE;
        suggested_pixel_format_index = ChoosePixelFormat(window_dc, &desired_pixel_format);
    }

    PIXELFORMATDESCRIPTOR suggested_pixel_format;
    DescribePixelFormat(window_dc, suggested_pixel_format_index, sizeof(suggested_pixel_format), &suggested_pixel_format);
    SetPixelFormat(window_dc, suggested_pixel_format_index, &suggested_pixel_format);
}

internal void wgl_load_extensions(WglInfo* info) {
    assert(info);

    WNDCLASSA window_class = {};
    window_class.lpfnWndProc = DefWindowProcA;
    window_class.hInstance = GetModuleHandle(0);
    window_class.lpszClassName = "WGLLoaderWindow";

    if (RegisterClass(&window_class)) {
        HWND window = CreateWindowA(
            window_class.lpszClassName,
            "WGL Loader",
            NULL,
            CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT,
            NULL, NULL,
            window_class.hInstance,
            NULL
        );

        HDC window_dc = GetDC(window);
        wgl_set_pixel_format(window_dc, 0);

        HGLRC glrc = wglCreateContext(window_dc);
        if (wglMakeCurrent(window_dc, glrc)) {
#define GL_LOAD_FUNCTION(name) name = (GL_FUNCTION_##name*)wglGetProcAddress(#name)
            // @Note: WGL functions
            GL_LOAD_FUNCTION(wglChoosePixelFormatARB);
            GL_LOAD_FUNCTION(wglGetExtensionsStringEXT);
            GL_LOAD_FUNCTION(wglCreateContextAttribsARB);
            GL_LOAD_FUNCTION(wglSwapIntervalEXT);

            // @Note: Platform independent functions
            GL_LOAD_FUNCTION(glGetStringi);
#undef GL_LOAD_FUNCTION

            if (wglGetExtensionsStringEXT) {
                char* extensions = (char*)wglGetExtensionsStringEXT();
                char* at = extensions;
                while (at[0]) {
                    while (is_whitespace(at[0])) {
                        at++;
                    }

                    char* end = at;

                    while (end[0] && !is_whitespace(end[0])) {
                        end++;
                    }

                    size_t count = end - at;

                    String substr = wrap_string(count, at);
#define GL_CHECK_EXTENSION(name) (strings_are_equal(substr, #name)) { info->name = true; }
                    if      GL_CHECK_EXTENSION(WGL_EXT_framebuffer_sRGB)
                    else if GL_CHECK_EXTENSION(WGL_ARB_multisample)
#undef GL_CHECK_EXTENSION

                    at = end;
                }
            }

            wglMakeCurrent(0, 0);
        }

        wglDeleteContext(glrc);
        ReleaseDC(window, window_dc);
        DestroyWindow(window);
    }
}

int wgl_opengl_attribs[] = {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 0,
    WGL_CONTEXT_FLAGS_ARB, 0 // NOTE: Enable for testing? WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#if HANDMADE_INTERNAL
    | WGL_CONTEXT_DEBUG_BIT_ARB
#endif
    ,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    0,
};

internal HGLRC wgl_opengl_init(HDC window_dc, WglInfo* wgl_info, OpenGLInfo* opengl_info) {
    if (!wgl_info) {
        WglInfo local_wgl_info;
        wgl_info = &local_wgl_info;
    }

    if (!opengl_info) {
        OpenGLInfo local_opengl_info;
        opengl_info = &local_opengl_info;
    }

    wgl_load_extensions(wgl_info);
    wgl_set_pixel_format(window_dc, wgl_info);

    b32 modern_context = true;

    HGLRC glrc = 0;
    if (wglCreateContextAttribsARB) {
        glrc = wglCreateContextAttribsARB(window_dc, 0, wgl_opengl_attribs);
    }

    if (!glrc) {
        modern_context = false;
        glrc = wglCreateContext(window_dc);
    }

    if (wglMakeCurrent(window_dc, glrc)) {
        opengl_init(modern_context, opengl_info);

        if (wglSwapIntervalEXT) {
            wglSwapIntervalEXT(1);
        }
    } else {
        INVALID_CODE_PATH;
    }

    glFinish();

    return glrc;
}
