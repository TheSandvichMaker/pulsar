inline void opengl_set_screenspace(u32 width, u32 height) {
    glMatrixMode(GL_PROJECTION);
    f32 a = safe_ratio_1(2.0f, cast(f32) width);
    f32 b = safe_ratio_1(2.0f, cast(f32) height);
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

    glColor4fv(color.e);

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

inline void opengl_rectangle(AxisAlignedBox2 aab, v4 color, GLuint render_mode, v2 min_uv = vec2(0, 0), v2 max_uv = vec2(1, 1)) {
    v2 dim = get_dim(aab);
    opengl_rectangle(get_min_corner(aab), vec2(dim.x, 0.0f), vec2(0.0f, dim.y), color, render_mode, min_uv, max_uv);
}

inline void opengl_texture(GLuint handle, v2 min_p, v2 x_axis, v2 y_axis, v4 color, v2 min_uv = vec2(0, 0), v2 max_uv = vec2(1, 1)) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, handle);
    opengl_rectangle(min_p, x_axis, y_axis, color, GL_TRIANGLES, min_uv, max_uv);
    glDisable(GL_TEXTURE_2D);
}

inline void opengl_texture(GLuint handle, AxisAlignedBox2 aab, v4 color = vec4(1, 1, 1, 1), v2 min_uv = vec2(0, 0), v2 max_uv = vec2(1, 1)) {
    v2 dim = get_dim(aab);
    opengl_texture(handle, get_min_corner(aab), vec2(dim.x, 0.0f), vec2(0.0f, dim.y), color, min_uv, max_uv);
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
#define GL_CHECK_EXTENSION(name) (strings_are_equal(extension, string_literal(#name))) { info->name = true; }
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

    for (u32 sort_entry_index = 0; sort_entry_index < commands->sort_entry_count; sort_entry_index++) {
        SortEntry* entry = cast(SortEntry*) commands->command_buffer + sort_entry_index;
        u8* at = commands->command_buffer + entry->index;

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

                f32 line_width = 2.0f;
                glLineWidth(line_width);

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
                    case Shape_Line: {
                        glBegin(GL_LINES);
                        glColor4fv(command->color.e);
                        v2 start_p = transform->offset;
                        v2 end_p   = transform->offset + transform->scale*rotate(shape->arm, transform->rotation_arm);
                        glVertex2fv(start_p.e);
                        glVertex2fv(end_p.e);
                        glEnd();
                    } break;

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
                        v2 x_axis = transform->rotation_arm*transform->scale;
                        v2 y_axis = perp(x_axis);

                        AxisAlignedBox2 aab = shape->bounding_box;

                        v2 a, b, c, d;
                        if (command->render_mode == ShapeRenderMode_Outline && line_width > 1.0f) {
                            v2 x_adjust = -0.5f*normalize_or_zero(x_axis)*line_width;
                            v2 y_adjust = -0.5f*normalize_or_zero(y_axis)*line_width;

                            // @Robustness: x_adjust and y_adjust here boldly assume the aab contains its origin!!
                            a = x_axis*corner_a(aab).x - x_adjust + y_axis*corner_a(aab).y - y_adjust;
                            b = x_axis*corner_b(aab).x + x_adjust + y_axis*corner_b(aab).y - y_adjust;
                            c = x_axis*corner_c(aab).x + x_adjust + y_axis*corner_c(aab).y + y_adjust;
                            d = x_axis*corner_d(aab).x - x_adjust + y_axis*corner_d(aab).y + y_adjust;
                        } else {
                            a = x_axis*corner_a(aab).x + y_axis*corner_a(aab).y;
                            b = x_axis*corner_b(aab).x + y_axis*corner_b(aab).y;
                            c = x_axis*corner_c(aab).x + y_axis*corner_c(aab).y;
                            d = x_axis*corner_d(aab).x + y_axis*corner_d(aab).y;
                        }

                        v2 p00 = transform->offset + a;
                        v2 p10 = transform->offset + b;
                        v2 p11 = transform->offset + c;
                        v2 p01 = transform->offset + d;

                        glBegin(render_mode);
                        glColor4fv(command->color.e);

                        glVertex2fv(p00.e);
                        glVertex2fv(p10.e);
                        glVertex2fv(p11.e);
                        glVertex2fv(p01.e);

                        glEnd();
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

            case RenderCommand_ParticleSystem: {
                RenderCommandParticleSystem* command = cast(RenderCommandParticleSystem*) at;
                at += sizeof(*command);

                Transform2D t = command->transform;
                ParticleSystem* system = command->system;

                v2 x_axis = t.rotation_arm*t.scale;
                v2 y_axis = perp(x_axis);

                for (u32 particle_index = 0; particle_index < system->count; particle_index++) {
                    Particle particle = system->particles[particle_index];
                    v2 transformed = t.offset + x_axis*particle.p.x + y_axis*particle.p.y;

                    v2 min_p = transformed + t.scale*vec2(-0.1f, -0.1f);
                    v2 max_p = transformed + t.scale*vec2( 0.1f,  0.1f);

                    v4 color = vec4(1, 1, 1, 1)*particle.alpha;

                    glBegin(GL_TRIANGLES);
                    glColor4fv(color.e);
                    // NOTE: Lower triangle
                    // glTexCoord2f(min_uv.x, min_uv.y);
                    glVertex2f(min_p.x, min_p.y);
                    // glTexCoord2f(max_uv.x, min_uv.y);
                    glVertex2f(max_p.x, min_p.y);
                    // glTexCoord2f(max_uv.x, max_uv.y);
                    glVertex2f(max_p.x, max_p.y);

                    // NOTE: Upper triangle
                    // glTexCoord2f(min_uv.x, min_uv.y);
                    glVertex2f(min_p.x, min_p.y);
                    // glTexCoord2f(max_uv.x, max_uv.y);
                    glVertex2f(max_p.x, max_p.y);
                    // glTexCoord2f(min_uv.x, max_uv.y);
                    glVertex2f(min_p.x, max_p.y);
                    glEnd();
                }
            } break;

            INVALID_DEFAULT_CASE;
        }
    }

    glDisable(GL_BLEND);
}
