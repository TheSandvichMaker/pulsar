#ifndef GAME_MAIN_H
#define GAME_MAIN_H

/* REFERENCES:
 * HANDMADE HERO (@TODO: List the stuff directly used from handmade hero):
 *     https://handmadehero.org/
 * GJK, EPA:
 *     https://caseymuratori.com/blog_0003
 *     http://www.dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
 *     http://www.dyn4j.org/2010/05/epa-expanding-polytope-algorithm/
 * Computing the winding of a polygon:
 *     http://paulbourke.net/geometry/polygonmesh/
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
    v2 sweep;
    f32 scale;
};

inline Transform2D default_transform2d() {
    Transform2D result = {};
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
    union {
        f32 radius;
        struct {
            u32 vert_count;
            v2* vertices;
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

enum EntityType {
    EntityType_Player,
    EntityType_Wall,
};

enum EntityFlag {
    EntityFlag_Moveable = 0x1,
    EntityFlag_Collides = 0x2,
};

struct Entity {
    EntityType type;
    v2 p;
    v2 dp;

    u32 flags;

    v4 color;

    Shape2D collision;
};

#define MAX_ENTITY_COUNT 8192
struct GameState {
    MemoryArena permanent_arena;
    MemoryArena transient_arena;

    AudioMixer audio_mixer;
    Assets assets;

    Sound* test_music;
    Sound* test_sound;
    Image* test_image;

    u32 entity_count;
    Entity entities[MAX_ENTITY_COUNT];

    Shape2D player_collision;

    u32 sound_timer;
};

global PlatformAPI platform;

#endif /* GAME_MAIN_H */
