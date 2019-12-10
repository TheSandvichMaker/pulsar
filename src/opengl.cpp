inline void opengl_set_screenspace(u32 width, u32 height) {
    glMatrixMode(GL_PROJECTION);
    f32 a = safe_ratio_1(2.0f, (f32)width);
    f32 b = safe_ratio_1(2.0f, (f32)height);
    f32 projection_matrix[] = {
         a,  0, 0, 0,
         0,  b, 0, 0,
         0,  0, 1, 0,
        -1, -1, 0, 1,
    };
    glLoadMatrixf(projection_matrix);
}

inline void opengl_rectangle(v2 min_p, v2 max_p, v4 color, v2 min_uv = vec2(0, 0), v2 max_uv = vec2(1, 1)) {
    glBegin(GL_TRIANGLES);

    glColor4f(color.r, color.g, color.b, color.a);

    // NOTE: Lower triangle
    glTexCoord2f(min_uv.x, min_uv.y);
    glVertex2f(min_p.x, min_p.y);
    glTexCoord2f(max_uv.x, min_uv.y);
    glVertex2f(max_p.x, min_p.y);
    glTexCoord2f(max_uv.x, max_uv.y);
    glVertex2f(max_p.x, max_p.y);

    // NOTE: Upper triangle
    glTexCoord2f(min_uv.x, min_uv.y);
    glVertex2f(min_p.x, min_p.y);
    glTexCoord2f(max_uv.x, max_uv.y);
    glVertex2f(max_p.x, max_p.y);
    glTexCoord2f(min_uv.x, max_uv.y);
    glVertex2f(min_p.x, max_p.y);

    glEnd();
}

inline void opengl_rectangle(Rect2 rect, v4 color, v2 min_uv = vec2(0, 0), v2 max_uv = vec2(1, 1)) {
    opengl_rectangle(get_min_corner(rect), get_max_corner(rect), color, min_uv, max_uv);
}

inline void opengl_texture(GLuint handle, v2 min_p, v2 max_p, v4 color, v2 min_uv = vec2(0, 0), v2 max_uv = vec2(1, 1)) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, handle);
    opengl_rectangle(min_p, max_p, color, min_uv, max_uv);
    glDisable(GL_TEXTURE_2D);
}

inline void opengl_texture(GLuint handle, Rect2 rect, v4 color, v2 min_uv = vec2(0, 0), v2 max_uv = vec2(1, 1)) {
    opengl_texture(handle, get_min_corner(rect), get_max_corner(rect), color, min_uv, max_uv);
}

inline GLuint opengl_load_texture(OpenGLInfo* opengl_info, u32 w, u32 h, void* pixels) {
    // @TODO: Ponder different texture formats
    GLuint texture_handle;
    glGenTextures(1, &texture_handle);
    glBindTexture(GL_TEXTURE_2D, texture_handle);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        opengl_info->default_internal_texture_format,
        w, h,
        0,
        GL_BGRA_EXT,
        GL_UNSIGNED_BYTE,
        pixels
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();

    return texture_handle;
}

internal void opengl_get_info(b32 modern_context, OpenGLInfo* info) {
    info->modern_context = modern_context;
    info->vendor = (char*)glGetString(GL_VENDOR);
    info->renderer = (char*)glGetString(GL_RENDERER);
    info->version = (char*)glGetString(GL_VERSION);
    if (info->modern_context) {
        info->shading_language_version = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    } else {
        info->shading_language_version = "(none)";
    }

    info->extensions = (char*)glGetString(GL_EXTENSIONS);

    if (modern_context && glGetStringi) {
        GLint num_extensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
        for (GLint extension_index = 0; extension_index < num_extensions; extension_index++) {
            char* extension = (char*)glGetStringi(GL_EXTENSIONS, extension_index);
#define GL_CHECK_EXTENSION(name) (strings_are_equal(extension, #name)) { info->name = true; }
            if      GL_CHECK_EXTENSION(GL_EXT_texture_sRGB)
            else if GL_CHECK_EXTENSION(GL_EXT_framebuffer_sRGB)
            else if GL_CHECK_EXTENSION(GL_ARB_framebuffer_sRGB)
#undef GL_CHECK_EXTENSION
        }
    } else {
        char* at = info->extensions;
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
            if      GL_CHECK_EXTENSION(GL_EXT_texture_sRGB)
            else if GL_CHECK_EXTENSION(GL_EXT_framebuffer_sRGB)
            else if GL_CHECK_EXTENSION(GL_ARB_framebuffer_sRGB)
#undef GL_CHECK_EXTENSION

            at = end;
        }
    }
}

internal void opengl_init(b32 modern_context, OpenGLInfo* info) {
    opengl_get_info(modern_context, info);

    info->default_internal_texture_format = GL_RGBA8;
    if ((info->GL_EXT_framebuffer_sRGB || info->GL_ARB_framebuffer_sRGB) && (info->GL_EXT_texture_sRGB)) {
        info->default_internal_texture_format = GL_SRGB8_ALPHA8;
        glEnable(GL_FRAMEBUFFER_SRGB);
    }
}
