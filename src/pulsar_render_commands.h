#ifndef RENDER_COMMANDS_H
#define RENDER_COMMANDS_H

struct RenderGroup {
    f32 units_to_vertical_screen_percentage;
    f32 vertical_screen_percentage_to_units;

    v2 camera_p;
    v2 camera_rotation_arm;

    struct GameRenderCommands* commands;
};

struct Image {
    union { PackedImage packed_image; struct { BodyOf_PackedImage }; };
    void* pixels;
    void* handle;
};

inline Rect2 get_aligned_image_rect(Image* image) {
    v2 dim = image->scale*vec2(image->w, image->h);
    Rect2 result = rect_min_dim(-image->align*dim, dim);
    return result;
}

struct Transform2D {
    v2 offset;
    v2 rotation_arm;
    v2 sweep;
    v2 scale;
};

inline Transform2D default_transform2d() {
    Transform2D result = {};
    result.rotation_arm = vec2(1, 0);
    result.scale = vec2(1.0f, 1.0f);
    return result;
}

inline Transform2D transform2d(v2 offset, v2 scale = vec2(1.0f, 1.0f), v2 sweep = vec2(0, 0)) {
    Transform2D result = default_transform2d();
    result.offset = offset;
    result.scale = scale;
    result.sweep = sweep;
    return result;
}

enum ShapeType {
    Shape_Point,
    Shape_Polygon,
    Shape_Circle,
    Shape_Rectangle,
};

struct Shape2D {
    ShapeType type;
    union {
        // @Note: Polygon
        struct {
            u32 vert_count;
            v2* vertices;
        };

        // @Note: Circle
        struct {
            f32 radius;
        };

        // @Note: Rectangle
        struct {
            Rect2 rect;
        };
    };
};

inline Shape2D polygon(u32 vert_count, v2* vertices) {
    Shape2D result = {};
    result.type = Shape_Polygon;
    result.vert_count = vert_count;
    result.vertices = vertices;
    return result;
}

inline Shape2D circle(f32 radius) {
    Shape2D result = {};
    result.type = Shape_Circle;
    result.radius = radius;
    return result;
}

inline Shape2D rectangle(Rect2 rect) {
    Shape2D result = {};
    result.type = Shape_Rectangle;
    result.rect = rect;
    return result;
}

inline Shape2D point() {
    Shape2D result = {};
    result.type = Shape_Point;
    return result;
}

enum RenderCommandType {
    RenderCommand_Clear,
    RenderCommand_Shape,
    RenderCommand_Image,
};

struct RenderCommandHeader {
    u8 type;
};

struct RenderCommandClear {
    v4 color;
};

enum ShapeRenderMode {
    ShapeRenderMode_Fill,
    ShapeRenderMode_Outline,
};

struct RenderCommandShape {
    // @TODO: Make transform a more uniform concept
    Transform2D transform;
    Shape2D shape;
    v4 color;
    ShapeRenderMode render_mode;
};

struct RenderCommandImage {
    Transform2D transform;
    Image* image;
    v4 color;
};

#endif /* RENDER_COMMANDS_H */
