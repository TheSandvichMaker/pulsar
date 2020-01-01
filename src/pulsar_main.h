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

#include <stdarg.h>

#include "common.h"

#include "memory_arena.h"
#include "pulsar_platform_bridge.h"

#include "string.h"
#include "math.h"
#include "opengl.h"

#define using_struct(type, as) union { type as; struct { BodyOf_##type }; };

#include "pulsar_assets.h"
#include "pulsar_audio_mixer.h"

enum GameMode {
    GameMode_Ingame,
    GameMode_Editor,
};

enum EntityFlag {
    EntityFlag_Moveable = 0x1,
    EntityFlag_Collides = 0x2,
    EntityFlag_OnGround = 0x4,
};

enum EntityType {
    EntityType_Null,

    EntityType_Player,
    EntityType_Wall,
    EntityType_SoundtrackPlayer,

    EntityType_Count,
};

#define enum_to_string(enum_name) case enum_name: { return #enum_name; }
inline char* entity_type_name(EntityType type) {
    switch (type) {
        enum_to_string(EntityType_Null);
        enum_to_string(EntityType_Player);
        enum_to_string(EntityType_Wall);
        enum_to_string(EntityType_SoundtrackPlayer);
        enum_to_string(EntityType_Count);
        INVALID_DEFAULT_CASE;
    }
    return 0;
}

struct EntityID { u32 value; };

struct Entity {
    EntityID id;

    EntityType type;
    v2 p;
    v2 dp;
    v2 ddp;

    f32 sim_dt;

    Entity* sticking_entity;
    v2 sticking_dp;

    b32 was_on_ground;
    f32 surface_friction;

    f32 movement_t;
    f32 off_ground_timer;
    f32 friction_of_last_touched_surface;

    u32 midi_note;
    v2 midi_test_target;

    b32 soundtrack_has_been_played;
    SoundtrackID soundtrack_id;
    u32 playback_flags;

    u32 flags;

    v4 color;

    Shape2D collision;
};

#include "pulsar_editor.h"

struct PlayingMidi {
    union {
        PlayingMidi* next;
        PlayingMidi* next_free;
    };

    // @Note: Without a sync sound, midi timing is going to be pretty rubbish.
    // With a sync sound however, it will be very good.
    PlayingSound* sync_sound;

    u32 tick_timer;
    u32 event_index;
    MidiTrack* track;

    u32 flags;
};

#define MAX_ENTITY_COUNT 8192
struct Level {
    String name;

    u32 entity_count;
    Entity entities[MAX_ENTITY_COUNT];
};

struct ActiveMidiEvent {
    using_struct(MidiEvent, midi_event);
    f32 dt_left;
};

struct GameState {
    MemoryArena permanent_arena;
    MemoryArena transient_arena;

    EditorState* editor_state;

    GameMode game_mode;

    AudioMixer audio_mixer;
    Assets assets;

    Sound* test_music;
    Sound* test_sound;
    Image* test_image;
    Soundtrack* test_soundtrack;

    PlayingMidi* first_playing_midi;
    PlayingMidi* first_free_playing_midi;

    u32 midi_event_buffer_count;
    ActiveMidiEvent midi_event_buffer[256];

    Level* active_level;

    u32 entity_count;
    Entity entities[MAX_ENTITY_COUNT];

    Shape2D player_collision;

    u32 sound_timer;
};

global PlatformAPI platform;

struct AddEntityResult {
    EntityID id;
    Entity* ptr;
};

inline b32 gjk_intersect_point(Transform2D t, Shape2D s, v2 p);
inline AddEntityResult add_entity(Level* level, EntityType type);
inline AddEntityResult add_player(GameState* game_state, Level* level, v2 starting_p);
inline Entity* get_entity(Level* level, EntityID id);
inline void delete_entity(Level* level, EntityID id);

#endif /* GAME_MAIN_H */
