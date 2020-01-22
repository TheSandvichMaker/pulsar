inline v2 get_screen_dim(RenderContext* render_context) {
    v2 result = vec2(render_context->commands->width, render_context->commands->height);
    return result;
}

inline f32 get_aspect_ratio(RenderContext* render_context) {
    v2 dim = get_screen_dim(render_context);
    f32 result = dim.x / dim.y;
    return result;
}

inline void render_worldspace(RenderContext* render_context, f32 vertical_fov) {
    render_context->vertical_fov = vertical_fov;
}

inline void initialize_render_context(RenderContext* render_context, GameRenderCommands* commands, f32 vertical_screen_percentage_to_units) {
    *render_context = {};
    render_worldspace(render_context, vertical_screen_percentage_to_units);
    render_context->camera_rotation_arm = vec2(1, 0);
    render_context->commands = commands;
}

inline void render_screenspace(RenderContext* render_context) {
    render_context->vertical_fov = cast(f32) render_context->commands->height;
    render_context->camera_p = 0.5f*vec2(render_context->commands->width, render_context->commands->height);
}

inline Transform2D world_to_screen(RenderContext* render_context, Transform2D world_t) {
    v2 screen_size = vec2(render_context->commands->width, render_context->commands->height);
    f32 units_to_pixels = screen_size.y / render_context->vertical_fov;

    Transform2D result = world_t;
    result.offset = units_to_pixels*rotate_clockwise(world_t.offset - render_context->camera_p, render_context->camera_rotation_arm) + 0.5f*screen_size;
    result.scale  = units_to_pixels*(world_t.scale);
    result.rotation_arm = rotate_clockwise(result.rotation_arm, render_context->camera_rotation_arm);

    return result;
}

inline Transform2D screen_to_world(RenderContext* render_context, Transform2D screen_t) {
    v2 screen_size = vec2(render_context->commands->width, render_context->commands->height);
    f32 pixels_to_units = render_context->vertical_fov / screen_size.y;

    Transform2D result = screen_t;
    result.offset = pixels_to_units*rotate(screen_t.offset - 0.5f*screen_size, render_context->camera_rotation_arm)  + render_context->camera_p;
    result.scale  = pixels_to_units*(screen_t.scale);
    result.rotation_arm = rotate(result.rotation_arm, render_context->camera_rotation_arm);

    return result;
}

inline f32 approximate_camera_distance_from_vfov(f32 vfov) {
    f32 approx_height_of_screen = 35.0f;
    f32 result = approx_height_of_screen / tan(vfov);
    return result;
}

inline v4 transform_color(v4 color) {
    v4 result = color;
    result.rgb = srgb_to_linear(result.rgb)*result.a;
    return result;
}

#define push_render_command(commands, type, sort_key) cast(RenderCommand##type*) push_render_command_(commands, RenderCommand_##type, sizeof(RenderCommand##type), sort_key)
inline void* push_render_command_(GameRenderCommands* commands, RenderCommandType type, u32 render_command_size, f32 sort_key) {
    void* result = 0;
    u32 next_command_index = commands->first_command - render_command_size - sizeof(RenderCommandHeader);
    if (sizeof(SortEntry)*(commands->sort_entry_count + 1) < next_command_index) {
        commands->first_command = next_command_index;

        RenderCommandHeader* header = cast(RenderCommandHeader*) (commands->command_buffer + commands->first_command);
        header->type = cast(u8) type;

        result = header + 1;

        SortEntry* sort_entry = cast(SortEntry*) commands->command_buffer + commands->sort_entry_count++;

        sort_entry->sort_key = sort_key;
        sort_entry->index = commands->first_command;
    } else {
        INVALID_CODE_PATH;
    }
    return result;
}

inline RenderCommandClear* push_clear(RenderContext* render_context, v4 color, f32 sort_key = -F32_MAX) {
    RenderCommandClear* result = push_render_command(render_context->commands, Clear, sort_key);
    if (result) {
        result->color = transform_color(color);
    }
    return result;
}

inline RenderCommandImage* push_image(RenderContext* render_context, Transform2D world_transform, Image* image, v4 color = vec4(1, 1, 1, 1), f32 sort_key = 0.0f) {
    RenderCommandImage* result = push_render_command(render_context->commands, Image, render_context->sort_key_bias + sort_key);
    if (result) {
        Transform2D transform = world_to_screen(render_context, world_transform);
        transform.scale *= image->scale / vec2(image->w, image->h);
        result->transform = transform;
        result->image = image;
        result->color = transform_color(color);
    }
    return result;
}

inline RenderCommandShape* push_shape(RenderContext* render_context, Transform2D world_transform, Shape2D shape, v4 color = vec4(1, 1, 1, 1), ShapeRenderMode render_mode = ShapeRenderMode_Fill, f32 sort_key = 0.0f) {
    RenderCommandShape* result = push_render_command(render_context->commands, Shape, render_context->sort_key_bias + sort_key);
    if (result) {
        Transform2D transform = world_to_screen(render_context, world_transform);
        result->transform = transform;
        result->shape = shape;
        result->color = transform_color(color);
        result->render_mode = render_mode;
    }
    return result;
}
