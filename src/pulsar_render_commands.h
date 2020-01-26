#ifndef RENDER_COMMANDS_H
#define RENDER_COMMANDS_H

struct RenderContext {
    f32 vertical_fov;

    v2 camera_p;
    v2 camera_rotation_arm;

    f32 sort_key_bias;

    struct GameRenderCommands* commands;
};

struct Image {
    union { PackedImage packed_image; struct { BodyOf_PackedImage }; };
    void* pixels;
    void* handle;
};

struct Particle {
    v2 p;
    f32 alpha;
    f32 depth;
};

struct ParticleSystem {
    u32 count;
    Particle* particles;
};

inline AxisAlignedBox2 get_aligned_image_aab(Image* image) {
    v2 dim = image->scale*vec2(image->w, image->h);
    AxisAlignedBox2 result = aab_min_dim(-image->align*dim, dim);
    return result;
}

enum RenderCommandType {
    RenderCommand_Clear,
    RenderCommand_Shape,
    RenderCommand_Image,
    RenderCommand_ParticleSystem,
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

struct RenderCommandParticleSystem {
    Transform2D transform;
    ParticleSystem* system;
};

#endif /* RENDER_COMMANDS_H */
