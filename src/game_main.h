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

struct PlayingMidi {
    union {
        PlayingMidi* next;
        PlayingMidi* next_free;
    };

    f32 timer;
    u32 event_index;
    MidiTrack* track;
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
    Soundtrack* test_soundtrack;

    PlayingMidi* first_playing_midi;
    PlayingMidi* first_free_playing_midi;

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
