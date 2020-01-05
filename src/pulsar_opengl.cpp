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

inline void opengl_rectangle(v2 min_p, v2 x_axis, v2 y_axis, v4 color, GLuint render_mode, v2 min_uv = vec2(0, 0), v2 max_uv = vec2(0, 0)) {
    glBegin(render_mode);

    glColor4f(color.r, color.g, color.b, color.a);

    v2 min_x_min_y = min_p;
    v2 min_x_max_y = min_p + y_axis;
    v2 max_x_min_y = min_p + x_axis;
    v2 max_x_max_y = min_p + x_axis + y_axis;

    // NOTE: Lower triangle
    glTexCoord2f(min_uv.x, min_uv.y);
    glVertex2fv(min_x_min_y.e);
    glTexCoord2f(max_uv.x, min_uv.y);
    glVertex2fv(max_x_min_y.e);
    glTexCoord2f(max_uv.x, max_uv.y);
    glVertex2fv(max_x_max_y.e);

    // NOTE: Upper triangle
    glTexCoord2f(min_uv.x, min_uv.y);
    glVertex2fv(min_x_min_y.e);
    glTexCoord2f(max_uv.x, max_uv.y);
    glVertex2fv(max_x_max_y.e);
    glTexCoord2f(min_uv.x, max_uv.y);
    glVertex2fv(min_x_max_y.e);

    glEnd();
}

inline void opengl_rectangle(Rect2 rect, v4 color, GLuint render_mode, v2 min_uv = vec2(0, 0), v2 max_uv = vec2(1, 1)) {
    v2 dim = get_dim(rect);
    opengl_rectangle(get_min_corner(rect), vec2(dim.x, 0.0f), vec2(0.0f, dim.y), color, render_mode, min_uv, max_uv);
}

inline void opengl_texture(GLuint handle, v2 min_p, v2 x_axis, v2 y_axis, v4 color, v2 min_uv = vec2(0, 0), v2 max_uv = vec2(1, 1)) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, handle);
    opengl_rectangle(min_p, x_axis, y_axis, color, GL_TRIANGLES, min_uv, max_uv);
    glDisable(GL_TEXTURE_2D);
}

inline void opengl_texture(GLuint handle, Rect2 rect, v4 color = vec4(1, 1, 1, 1), v2 min_uv = vec2(0, 0), v2 max_uv = vec2(1, 1)) {
    v2 dim = get_dim(rect);
    opengl_texture(handle, get_min_corner(rect), vec2(dim.x, 0.0f), vec2(0.0f, dim.y), color, min_uv, max_uv);
}

internal GLuint opengl_load_texture(OpenGLInfo* opengl_info, u32 w, u32 h, void* pixels) {
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

internal void opengl_unload_texture(GLuint handle) {
    glDeleteTextures(1, &handle);
}

inline void opengl_check_extension(OpenGLInfo* info, String extension) {
#define GL_CHECK_EXTENSION(name) (strings_are_equal(extension, #name)) { info->name = true; }
    if      GL_CHECK_EXTENSION(GL_EXT_texture_sRGB)
    else if GL_CHECK_EXTENSION(GL_EXT_framebuffer_sRGB)
    else if GL_CHECK_EXTENSION(GL_ARB_framebuffer_sRGB)
    else if GL_CHECK_EXTENSION(GL_ARB_multisample)
#undef GL_CHECK_EXTENSION
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
            opengl_check_extension(info, wrap_cstr(extension));
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
            opengl_check_extension(info, wrap_string(count, at));

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
    if (info->GL_ARB_multisample) {
        glEnable(GL_MULTISAMPLE_ARB);
    }
}

internal void opengl_render_commands(GameRenderCommands* commands) {
    u32 width = commands->width;
    u32 height = commands->height;

    glViewport(0, 0, width, height);
    opengl_set_screenspace(width, height);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    u8* at = commands->command_buffer;
    while (at < (commands->command_buffer + commands->command_buffer_used)) {
        RenderCommandHeader* header = cast(RenderCommandHeader*) at;
        at += sizeof(*header);

        switch (header->type) {
            case RenderCommand_Clear: {
                RenderCommandClear* command = cast(RenderCommandClear*) at;
                at += sizeof(*command);

                glClearColor(command->color.r, command->color.g, command->color.b, command->color.a);
                glClear(GL_COLOR_BUFFER_BIT);
            } break;

            case RenderCommand_Shape: {
                RenderCommandShape* command = cast(RenderCommandShape*) at;
                at += sizeof(*command);

                Transform2D* transform = &command->transform;
                Shape2D* shape = &command->shape;

                u32 circle_quality = 256;

                GLuint render_mode = GL_POLYGON;
                switch (command->render_mode) {
                    case ShapeRenderMode_Fill: {
                        render_mode = GL_POLYGON;
                    } break;
                    case ShapeRenderMode_Outline: {
                        render_mode = GL_LINE_LOOP;
                    } break;
                }

                switch (shape->type) {
                    case Shape_Polygon: {
                        glBegin(render_mode);
                        glColor4fv(command->color.e);

                        for (u32 vertex_index = 0; vertex_index < shape->vert_count; vertex_index++) {
                            v2 v = rotate(transform->scale*shape->vertices[vertex_index], transform->rotation_arm) + transform->offset;
                            glVertex2fv(v.e);
                        }

                        glEnd();
                    } break;

                    case Shape_Circle: {
                        glBegin(render_mode);
                        glColor4fv(command->color.e);

                        for (f32 segment_angle = 0; segment_angle < TAU_32; segment_angle += (TAU_32 / cast(f32) circle_quality)) {
                            v2 v = rotate(transform->scale*vec2(sin(segment_angle), cos(segment_angle))*shape->radius, transform->rotation_arm) + transform->offset;
                            glVertex2fv(v.e);
                        }

                        glEnd();
                    } break;

                    case Shape_Rectangle: {
                        // @IMPORTANT @TODO: This produces inaccurate results! Why?
                        v2 x_axis = transform->rotation_arm*transform->scale;
                        v2 y_axis = perp(x_axis);
                        v2 dim = get_dim(shape->rect);
                        x_axis *= dim.x;
                        y_axis *= dim.y;
                        v2 min_p = transform->offset + get_min_corner(shape->rect) - 0.5f*x_axis - 0.5f*y_axis;

                        v2 p00 = min_p;
                        v2 p10 = min_p + x_axis;
                        v2 p01 = min_p + y_axis;
                        v2 p11 = min_p + x_axis + y_axis;

                        glBegin(render_mode);
                        glColor4fv(command->color.e);

                        glVertex2fv(p00.e);
                        glVertex2fv(p10.e);
                        glVertex2fv(p11.e);
                        glVertex2fv(p01.e);

                        glEnd();

                        // opengl_rectangle(min_p, x_axis*dim.x, y_axis*dim.y, command->color, render_mode);
                    } break;
                }
            } break;

            case RenderCommand_Image: {
                RenderCommandImage* command = cast(RenderCommandImage*) at;
                at += sizeof(*command);

                Image* image = command->image;
                Transform2D t = command->transform;
                v2 x_axis = t.rotation_arm*t.scale;
                v2 y_axis = perp(x_axis);
                v2 align = image->align*vec2(image->w, image->h);
                v2 min_p = t.offset - x_axis*align.x - y_axis*align.y;
                opengl_texture(cast(GLuint) image->handle, min_p, x_axis*cast(f32) image->w, y_axis*cast(f32) image->h, command->color);
            } break;

            INVALID_DEFAULT_CASE;
        }
    }

    glDisable(GL_BLEND);
}
