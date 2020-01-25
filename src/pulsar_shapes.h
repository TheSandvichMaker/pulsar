#ifndef PULSAR_SHAPES_H
#define PULSAR_SHAPES_H

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
    Shape_Line,
    Shape_Polygon,
    Shape_Circle,
    Shape_Rectangle,
};

struct Shape2D {
    ShapeType type;
    AxisAlignedBox2 bounding_box; // @Note: Also used as the shape information for Shape_Rectangle
    union {
        struct /* Shape_Line */ {
            v2 arm;
        };

        struct /* Shape_Polygon */ {
            u32 vert_count;
            v2* vertices;
        };

        struct /* Shape_Circle */ {
            f32 radius;
        };
    };
};

inline AxisAlignedBox2 find_bounding_box_for_shape(Shape2D shape) {
    AxisAlignedBox2 result = {};
    switch (shape.type) {
        case Shape_Point: {
            /* Do nothing at all... */
        } break;

        case Shape_Line: {
            result = aab_min_dim(vec2(0, 0), shape.arm);
        } break;

        case Shape_Polygon: {
            for (u32 vert_index = 0; vert_index < shape.vert_count; vert_index++) {
                result = grow_to_contain(result, shape.vertices[vert_index]);
            }
        } break;

        case Shape_Circle: {
            result = aab_center_half_dim(vec2(0, 0), vec2(shape.radius, shape.radius));
        } break;

        case Shape_Rectangle: {
            /* A rectangle is by definition already a bounding box, so that's that. */
        } break;

        INVALID_DEFAULT_CASE;
    }
    return result;
}

inline Shape2D point() {
    Shape2D result = {};
    result.type = Shape_Point;
    return result;
}

inline Shape2D line(v2 arm) {
    Shape2D result = {};
    result.type = Shape_Line;
    result.arm = arm;
    result.bounding_box = find_bounding_box_for_shape(result);
    return result;
}

inline Shape2D polygon(u32 vert_count, v2* vertices) {
    Shape2D result = {};
    result.type = Shape_Polygon;
    result.vert_count = vert_count;
    result.vertices = vertices;
    result.bounding_box = find_bounding_box_for_shape(result);
    return result;
}

inline Shape2D circle(f32 radius) {
    Shape2D result = {};
    result.type = Shape_Circle;
    result.radius = radius;
    result.bounding_box = find_bounding_box_for_shape(result);
    return result;
}

inline Shape2D rectangle(AxisAlignedBox2 aab) {
    Shape2D result = {};
    result.type = Shape_Rectangle;
    result.bounding_box = aab;
    return result;
}

inline Shape2D rectangle(v2 dim) {
    Shape2D result = {};
    result.type = Shape_Rectangle;
    result.bounding_box = aab_center_dim(vec2(0, 0), dim);
    return result;
}

#endif /* PULSAR_SHAPES_H */
