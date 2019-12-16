#ifndef GAME_MAIN_H
#define GAME_MAIN_H

/* REFERENCES:
 * HANDMADE HERO (@TODO: List the stuff directly used from handmade hero):
 *     https://handmadehero.org/
 * GJK:
 *     https://caseymuratori.com/blog_0003
 *     http://www.dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
 */

#include "common.h"
#include "memory_arena.h"
#include "platform_bridge.h"

#include "string.h"
#include "math.h"
#include "opengl.h"

#include "game_assets.h"
#include "audio_mixer.h"

struct Transform2D {
    v2 offset;
    v2 rotation_arm;
    f32 scale;
};

inline Transform2D default_transform2d() {
    Transform2D result;
    result.offset = vec2(0, 0);
    result.rotation_arm = vec2(1, 0);
    result.scale = 1.0f;
    return result;
}

enum ShapeType {
    Shape_Polygon,
    Shape_Circle,
};

struct Shape2D {
    ShapeType type;
    v2 sweep;
    union {
        f32 radius;
        struct {
            u32 vert_count;
            v2* vertices;
        };
    };
};

inline Shape2D polygon(u32 vert_count, v2* vertices, v2 sweep = vec2(0, 0)) {
    Shape2D result = {};
    result.type = Shape_Polygon;
    result.vert_count = vert_count;
    result.vertices = vertices;
    result.sweep = sweep;
    return result;
}

inline Shape2D circle(f32 radius, v2 sweep = vec2(0, 0)) {
    Shape2D result = {};
    result.type = Shape_Circle;
    result.radius = radius;
    result.sweep = sweep;
    return result;
}

enum EntityType {
    EntityType_Player,
};

struct Entity {
    EntityType type;
    v2 p;
    f32 rotation;
};

struct GameState {
    MemoryArena permanent_arena;
    MemoryArena transient_arena;

    AudioMixer audio_mixer;
    Assets assets;

    Sound* test_music;
    Sound* test_sound;
    Image* test_image;

    Entity player;
    f32 rotation;

    u32 sound_timer;
};

global PlatformAPI platform;

#endif /* GAME_MAIN_H */
