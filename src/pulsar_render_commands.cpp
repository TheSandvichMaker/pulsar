inline void render_worldspace(RenderGroup* render_group, f32 vertical_fov) {
    render_group->vertical_fov = vertical_fov;
}

inline void initialize_render_group(RenderGroup* render_group, GameRenderCommands* commands, f32 vertical_screen_percentage_to_units) {
    zero_struct(*render_group);
    render_worldspace(render_group, vertical_screen_percentage_to_units);
    render_group->camera_rotation_arm = vec2(1, 0);
    render_group->commands = commands;
}

inline void render_screenspace(RenderGroup* render_group) {
    render_group->vertical_fov = cast(f32) render_group->commands->height;
    render_group->camera_p = 0.5f*vec2(render_group->commands->width, render_group->commands->height);
}

inline Transform2D world_to_screen(RenderGroup* render_group, Transform2D world_t) {
    v2 screen_size = vec2(render_group->commands->width, render_group->commands->height);
    f32 units_to_pixels = screen_size.y / render_group->vertical_fov;

    Transform2D result = world_t;
    result.offset = units_to_pixels*rotate_clockwise(world_t.offset - render_group->camera_p, render_group->camera_rotation_arm) + 0.5f*screen_size;
    result.scale  = units_to_pixels*(world_t.scale);
    result.rotation_arm = rotate_clockwise(result.rotation_arm, render_group->camera_rotation_arm);

    return result;
}

inline Transform2D screen_to_world(RenderGroup* render_group, Transform2D screen_t) {
    v2 screen_size = vec2(render_group->commands->width, render_group->commands->height);
    f32 pixels_to_units = render_group->vertical_fov / screen_size.y;

    Transform2D result = screen_t;
    result.offset = pixels_to_units*rotate(screen_t.offset - 0.5f*screen_size, render_group->camera_rotation_arm)  + render_group->camera_p;
    result.scale  = pixels_to_units*(screen_t.scale);
    result.rotation_arm = rotate(result.rotation_arm, render_group->camera_rotation_arm);

    return result;
}

inline v4 transform_color(v4 color) {
    v4 result = color;
    result.rgb = srgb_to_linear(result.rgb);
    return result;
}

#define push_render_command(commands, type) cast(RenderCommand##type*) push_render_command_(commands, RenderCommand_##type, sizeof(RenderCommand##type))
inline void* push_render_command_(GameRenderCommands* commands, RenderCommandType type, u32 render_command_size) {
    void* result = 0;
    if (commands->command_buffer_used + (render_command_size + sizeof(RenderCommandHeader)) < commands->command_buffer_size) {
        RenderCommandHeader* header = cast(RenderCommandHeader*) (commands->command_buffer + commands->command_buffer_used);
        header->type = cast(u8) type;
        commands->command_buffer_used += sizeof(*header);

        result = (commands->command_buffer + commands->command_buffer_used);
        commands->command_buffer_used += render_command_size;
    } else {
        INVALID_CODE_PATH;
    }
    return result;
}

inline RenderCommandClear* push_clear(RenderGroup* render_group, v4 color) {
    RenderCommandClear* result = push_render_command(render_group->commands, Clear);
    if (result) {
        result->color = transform_color(color);
    }
    return result;
}

inline RenderCommandImage* push_image(RenderGroup* render_group, Transform2D world_transform, Image* image, v4 color = vec4(1, 1, 1, 1)) {
    RenderCommandImage* result = push_render_command(render_group->commands, Image);
    if (result) {
        Transform2D transform = world_to_screen(render_group, world_transform);
        transform.scale *= image->scale / vec2(image->w, image->h);
        result->transform = transform;
        result->image = image;
        result->color = transform_color(color);
    }
    return result;
}

inline RenderCommandShape* push_shape(RenderGroup* render_group, Transform2D world_transform, Shape2D shape, v4 color = vec4(1, 1, 1, 1), ShapeRenderMode render_mode = ShapeRenderMode_Fill) {
    RenderCommandShape* result = push_render_command(render_group->commands, Shape);
    if (result) {
        Transform2D transform = world_to_screen(render_group, world_transform);
        result->transform = transform;
        result->shape = shape;
        result->color = transform_color(color);
        result->render_mode = render_mode;
    }
    return result;
}
