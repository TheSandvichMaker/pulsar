#ifndef PULSAR_MAIN_H
#define PULSAR_MAIN_H

// High level overview of @TODOs:
// TOMORROW:
// - Eek out final little pops in audio, look at why midi goes out of sync slowly (likely
//    related problems, midi sync resets when the pop happens)
// TODAY:
// -
//
// IMPORTANT:
// - Fix collision once and for all!!!! The way it stands now, GJK and EPA are not enough
//    for collision handling, because once I intersect two shapes, the collision info I
//    get back does not consider the direction the shape came from at all, it only looks
//    at how to most quickly get the shapes to stop intersecting. This results in
//    collisions resolving in the wrong direction.
//    How to fix? Does the info I get by expanding GJK empower me? Do I need to understand
//    EPA better? Or do I need a different algorithm altogether.
//
//    If this is not done by 14/01/2020, SWITCH TO AXIS ALIGNED BOXES!!!!
//
// - Find the cause of and fix the infinite loop condition in GJK
//
// - How the hell do we make player movement feel good? If we use the support entity scheme,
//    how do we know we're still being supported by the same entity, how do we handle it when
//    we get smacked off the side by something else?
//
//    Movement ideas:
//     The player's speed while jumping needs to be limited, because the player's run speed is
//      currently limited by surface friction, but unbounded in the air. I'm not sure if friction
//      should be the governing factor anyway.
//     The player probably should never lose contact with a platform they're standing on unless
//      they either jump off or are bumped off by an obstacle.
//
// - Basic profiling
// - Get the mixer back to full spec (smooth volume fade and (smooth?) variable playback speed)
//
// - Add enough editor features to be actually able to make levels
// - Actual gameplay mechanics (checkpoints+, hazards+, audio zones)
//
// - Level saving / loading
//
// - Metagame features (savegames, menu, options)
//
// Probably also important:
// - Sub-frame entity logic (e.g. a platform might start moving halfway
//     into the frame since that's when the midi note hits)
// - Midi timing (I believe the sync with samples isn't quite right since the
//     platform asks for more than a frame's worth of audio)
//
// Low importance:
// - Templatize LinearBuffer? Maybe?? Maybe not.
// - Shaders?
// - Make some kind entity system besides just the monolithic entities I've got now

/* RESOURCES:
 * Audio and Music:
 *     test_sound: Handmade Hero
 *     test_music: Glenn Gould: Golberg Variations (1983) - 01 Aria
 *     Everything else: Composed by me
 * Images:
 *     camera icon: Google images (@Copyright!)
 *     speaker icon: Google images (@Copyright!)
 *
 * REFERENCES:
 * Handmade Hero (@TODO: List the stuff directly used from handmade hero):
 *     https://handmadehero.org/
 *     A lot of the (win32) platform layer
 *     DirectSound setup - I rewrote the way the DS buffer is actually written into to reduce latency, make midi sync better, and stop audio glitches on frame drops
 *     A lot of the audio mixer - I rewrote the relevant parts to handle the change to DS, as well as my own needs (sample-perfect looping, midi sync)
 *     Input handling (@TODO: Start using raw input - set up an input thread?)
 *     Sorting (pulsar_sort.cpp is a direct copy of handmade_sort.cpp)
 * OpenGL:
 *     Handmade Hero for OpenGL setup and basic rendering (@TODO: be more specific)
 *     OpenGL wiki for everything else (@TODO: be more specific)
 * General Knowledge and Insight from watching Jonathan Blow streams:
 *     https://www.youtube.com/user/jblow888
 * Equations of motion:
 *     Handmade Hero (https://www.youtube.com/watch?v=LoTRzRFEk5I)
 *     Various wikipedia pages (e.g. https://en.wikipedia.org/wiki/Classical_mechanics)
 * Immediate mode GUIs:
 *     Handmade Hero Debug System
 *     https://www.youtube.com/watch?v=Z1qyvQsjK5Y
 * GJK, EPA:
 *     https://caseymuratori.com/blog_0003
 *     http://www.dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
 *     http://www.dyn4j.org/2010/05/epa-expanding-polytope-algorithm/
 * Computing the winding of a polygon:
 *     http://paulbourke.net/geometry/polygonmesh/
 * MIDI:
 *     https://www.midi.org/specifications-old/item/the-midi-1-0-specification
 * Framerate Independence / Timing
 *     https://www.youtube.com/watch?v=fdAOPHgW7qM
 *     https://www.youtube.com/watch?v=jTzIDmjkLQo
 * Ring buffers (not used for reference in the actual codebase yet, but my idea for simplifying the undo system was sparked by reading this post):
 *     https://fgiesen.wordpress.com/2010/12/14/ring-buffers-and-queues/
 */

#include <stdarg.h>

#include "common.h"

#include "pulsar_memory.h"
#include "pulsar_memory_arena.h"
#include "pulsar_platform_bridge.h"

#include "string.h"
#include "math.h"

#include "pulsar_shapes.h"
#include "pulsar_render_commands.h"
#include "pulsar_opengl.h"

#include "pulsar_assets.h"
#include "pulsar_audio_mixer.h"
#include "pulsar_gjk.h"
#include "pulsar_entity.h"
#include "pulsar_editor.h"

enum GameMode {
    GameMode_Ingame,
    GameMode_Editor,
};

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

struct ActiveMidiEvent {
    using_struct(MidiEvent, midi_event);
    f32 dt_left;
};

struct GameState {
    MemoryArena permanent_arena;
    MemoryArena transient_arena;

    RenderContext render_context;

    EditorState* editor_state;

    GameMode game_mode;

    AudioMixer audio_mixer;
    Assets assets;

    Sound* test_music;
    Sound* test_sound;

    v4 foreground_color;
    v4 background_color;

    f32 frame_dt_left;

    f32 player_respawn_timer;

    f32 rotation;

    Soundtrack* test_soundtrack;

    PlayingMidi* first_playing_midi;
    PlayingMidi* first_free_playing_midi;

    u32 midi_event_buffer_count;
    ActiveMidiEvent midi_event_buffer[256];

    Entity* player;
    Entity* camera_target;

    Entity* last_activated_checkpoint;

    Level* active_level;

    u32 entity_count;
    Entity entities[MAX_ENTITY_COUNT];

    Shape2D player_collision;
    Shape2D arrow;

    u32 sound_timer;
};

#if PULSAR_DEBUG
global GameState* dbg_game_state;
#endif

global PlatformAPI platform;

inline b32 gjk_intersect_point(Transform2D t, Shape2D s, v2 p);
inline void play_soundtrack(GameState* game_state, Soundtrack* soundtrack, u32 flags = 0);
inline void dbg_draw_arrow(v2 start, v2 end, v4 color);

#define DEFINE_COLORS(MIDDLE_FIX, VALUE, COUNTER_VALUE) \
static const v4 COLOR_##MIDDLE_FIX##RED    = { VALUE         , COUNTER_VALUE , COUNTER_VALUE , 1 }; \
static const v4 COLOR_##MIDDLE_FIX##GREEN  = { COUNTER_VALUE , VALUE         , COUNTER_VALUE , 1 }; \
static const v4 COLOR_##MIDDLE_FIX##BLUE   = { COUNTER_VALUE , COUNTER_VALUE , VALUE         , 1 }; \
static const v4 COLOR_##MIDDLE_FIX##YELLOW = { VALUE         , VALUE         , COUNTER_VALUE , 1 }; \
static const v4 COLOR_##MIDDLE_FIX##PINK   = { VALUE         , COUNTER_VALUE , VALUE         , 1 }; \
static const v4 COLOR_##MIDDLE_FIX##CYAN   = { COUNTER_VALUE , VALUE         , VALUE         , 1 }; \

DEFINE_COLORS(, 1.0f, 0.0f)
DEFINE_COLORS(DARK_, 0.75f, 0.0f)
DEFINE_COLORS(DARKER_, 0.5f, 0.0f)
DEFINE_COLORS(DARKEST_, 0.25f, 0.0f)
DEFINE_COLORS(LIGHT_, 1.0f, 0.25f)
DEFINE_COLORS(LIGHTER_, 1.0f, 0.5f)
DEFINE_COLORS(LIGHTEST_, 1.0f, 0.75f)
DEFINE_COLORS(DESATURATED_, 0.65f, 0.35f)
DEFINE_COLORS(DESATURATED_DARK_, 0.5f, 0.25f)
DEFINE_COLORS(DESATURATED_LIGHT_, 0.85f, 0.75f)

static const v4 COLOR_WHITE      = { 1.0f , 1.0f , 1.0f , 1.0f };
static const v4 COLOR_LIGHT_GREY = { 0.75f, 0.75f, 0.75f, 1.0f };
static const v4 COLOR_GREY       = { 0.5f , 0.5f , 0.5f , 1.0f };
static const v4 COLOR_DARK_GREY  = { 0.25f, 0.25f, 0.25f, 1.0f };
static const v4 COLOR_BLACK      = { 0.0f , 0.0f , 0.0f , 1.0f };

#endif /* PULSAR_MAIN_H */
