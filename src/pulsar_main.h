#ifndef PULSAR_MAIN_H
#define PULSAR_MAIN_H

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
 * OpenGL:
 *     Handmade Hero for OpenGL setup and basic rendering (@TODO: be more specific)
 *     OpenGL wiki for everything else (@TODO: be more specific)
 * General Knowledge and Insight from watching Jonathan Blow streams:
 *     https://www.youtube.com/user/jblow888
 * Handmade Hero (@TODO: List the stuff directly used from handmade hero):
 *     https://handmadehero.org/
 *     A lot of the (win32) platform layer
 *     DirectSound setup and usage
 *     Input handling (@TODO: Start using raw input - set up an input thread?)
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
 */

#include <stdarg.h>

#include "common.h"

#include "memory_arena.h"
#include "pulsar_platform_bridge.h"

#include "string.h"
#include "math.h"
#include "pulsar_opengl.h"

#define using_struct(type, as) union { type as; struct { BodyOf_##type }; };

#include "pulsar_assets.h"
#include "pulsar_audio_mixer.h"
#include "pulsar_entity.h"

#define MAX_ENTITY_COUNT 8192
struct Level {
    String name;

    u32 entity_count;
    Entity entities[MAX_ENTITY_COUNT];
};

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

    RenderGroup render_group;

    EditorState* editor_state;

    GameMode game_mode;

    AudioMixer audio_mixer;
    Assets assets;

    Sound* test_music;
    Sound* test_sound;

    // @Note: Editor Assets (should they go somewhere else?)
    Image* camera_icon;
    Image* speaker_icon;

    f32 rotation;

    Soundtrack* test_soundtrack;

    PlayingMidi* first_playing_midi;
    PlayingMidi* first_free_playing_midi;

    u32 midi_event_buffer_count;
    ActiveMidiEvent midi_event_buffer[256];

    EntityID camera_target;

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

struct AddEntityResult {
    EntityID id;
    Entity* ptr;
};

inline b32 gjk_intersect_point(Transform2D t, Shape2D s, v2 p);
inline AddEntityResult add_entity(Level* level, EntityType type);
inline AddEntityResult add_player(GameState* game_state, Level* level, v2 starting_p);
inline Entity* get_entity(Level* level, EntityID id);
inline void delete_entity(Level* level, EntityID id);

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
