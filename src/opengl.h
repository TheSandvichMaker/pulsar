#ifndef OPENGL_H
#define OPENGL_H

static const GLuint GL_FRAMEBUFFER_SRGB         = 0x8DB9;
static const GLuint GL_SRGB8_ALPHA8             = 0x8C43;
static const GLuint GL_NUM_EXTENSIONS           = 0x821D;
static const GLuint GL_SHADING_LANGUAGE_VERSION = 0x8B8C;

#define GL_FUNCTION(return_type, name, ...) \
    typedef return_type WINAPI GL_FUNCTION_##name(__VA_ARGS__); \
    global GL_FUNCTION_##name* name;

GL_FUNCTION(GLubyte*, glGetStringi, GLenum name, GLuint index);

struct OpenGLInfo {
    b32 modern_context;

    char* vendor;
    char* renderer;
    char* version;
    char* shading_language_version;
    char* extensions;

    GLuint default_internal_texture_format;

    // @Note: Extensions
    b32 GL_EXT_texture_sRGB;
    b32 GL_EXT_framebuffer_sRGB;
    b32 GL_ARB_framebuffer_sRGB;
};

#endif /* OPENGL_H */
