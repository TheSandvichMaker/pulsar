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

inline RenderCommandClear* push_clear(GameRenderCommands* commands, v4 color) {
    RenderCommandClear* result = push_render_command(commands, Clear);
    result->color = color;
    return result;
}

inline RenderCommandImage* push_image(GameRenderCommands* commands, Image* image, v2 p, v4 color = vec4(1, 1, 1, 1)) {
    RenderCommandImage* result = push_render_command(commands, Image);
    result->image = image;
    result->p = p - image->align*vec2(image->w, image->h);
    result->color = color;
    return result;
}

inline RenderCommandShape* push_shape(GameRenderCommands* commands, Transform2D transform, Shape2D shape, v4 color = vec4(1, 1, 1, 1)) {
    RenderCommandShape* result = push_render_command(commands, Shape);
    result->transform = transform;
    result->shape = shape;
    result->color = color;
    return result;
}
