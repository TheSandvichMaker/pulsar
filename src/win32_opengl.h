#ifndef WIN32_OPENGL_H
#define WIN32_OPENGL_H

static const GLuint WGL_CONTEXT_MAJOR_VERSION_ARB             = 0x2091;
static const GLuint WGL_CONTEXT_MINOR_VERSION_ARB             = 0x2092;
static const GLuint WGL_CONTEXT_LAYER_PLANE_ARB               = 0x2093;
static const GLuint WGL_CONTEXT_FLAGS_ARB                     = 0x2094;
static const GLuint WGL_CONTEXT_PROFILE_MASK_ARB              = 0x9126;
static const GLuint WGL_CONTEXT_DEBUG_BIT_ARB                 = 0x0001;
static const GLuint WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB    = 0x0002;
static const GLuint WGL_CONTEXT_CORE_PROFILE_BIT_ARB          = 0x00000001;
static const GLuint WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB = 0x00000002;

static const GLuint WGL_DRAW_TO_WINDOW_ARB                    = 0x2001;
static const GLuint WGL_ACCELERATION_ARB                      = 0x2003;
static const GLuint WGL_SUPPORT_OPENGL_ARB                    = 0x2010;
static const GLuint WGL_DOUBLE_BUFFER_ARB                     = 0x2011;
static const GLuint WGL_PIXEL_TYPE_ARB                        = 0x2013;
static const GLuint WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB          = 0x20A9;

static const GLuint WGL_COLOR_BITS_ARB                        = 0x2014;
static const GLuint WGL_DEPTH_BITS_ARB                        = 0x2022;
static const GLuint WGL_STENCIL_BITS_ARB                      = 0x2023;

static const GLuint WGL_FULL_ACCELERATION_ARB                 = 0x2027;
static const GLuint WGL_TYPE_RGBA_ARB                         = 0x202B;

GL_FUNCTION(BOOL, wglChoosePixelFormatARB, HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
GL_FUNCTION(HGLRC, wglCreateContextAttribsARB, HDC hDC, HGLRC hShareContext, const int *attribList);
GL_FUNCTION(BOOL, wglSwapIntervalEXT, int interval);
GL_FUNCTION(const char*, wglGetExtensionsStringEXT);

struct WglInfo {
    b32 WGL_EXT_framebuffer_sRGB;
};

#endif /* WIN32_OPENGL_H */
