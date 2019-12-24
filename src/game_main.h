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
 * MIDI:
 *     https://www.midi.org/specifications-old/item/the-midi-1-0-specification
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

inline Transform2D transform2d(v2 offset, v2 sweep = vec2(0, 0)) {
    Transform2D result = default_transform2d();
    result.offset = offset;
    result.sweep = sweep;
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
    EntityType_Null,

    EntityType_Player,
    EntityType_Wall,
};

enum EntityFlag {
    EntityFlag_Moveable = 0x1,
    EntityFlag_Collides = 0x2,
    EntityFlag_OnGround = 0x4,
};

struct Entity {
    EntityType type;
    v2 p;
    v2 dp;
    v2 ddp;

    Entity* sticking_entity;
    v2 sticking_dp;

    b32 was_on_ground;

    f32 movement_t;
    f32 off_ground_timer;
    f32 surface_friction;
    f32 friction_of_last_touched_surface;

    v2 midi_test_target;

    u32 flags;

    v4 color;

    Shape2D collision;
};

enum GameMode {
    GameMode_Ingame,
    GameMode_Editor,
};

#define MAX_ENTITY_COUNT 8192
struct GameState {
    MemoryArena permanent_arena;
    MemoryArena transient_arena;

    GameMode game_mode;

    AudioMixer audio_mixer;
    Assets assets;

    Font* debug_font;

    Sound* test_music;
    Sound* test_sound;
    Image* test_image;
    MidiTrack* test_midi_track;

    u32 midi_event_index;
    f32 midi_timer;

    u32 midi_event_buffer_count;
    MidiEvent midi_event_buffer[256];

    u32 level_entity_count;
    Entity level_entities[MAX_ENTITY_COUNT];

    u32 entity_count;
    Entity entities[MAX_ENTITY_COUNT];

    Shape2D player_collision;

    u32 sound_timer;
};

global PlatformAPI platform;

#endif /* GAME_MAIN_H */
