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
 * Pan Laws:
 *     http://www.cs.cmu.edu/~music/icm-online/readings/panlaws/panlaws.pdf
 * Various things I've learned over time and useful functions like smooth_min:
 *     https://www.iquilezles.org/www/index.htm
 */

#include <stdarg.h>

#include "pulsar_common.h"
#include "pulsar_platform_bridge.h"

global GameConfig* game_config;

#include "pulsar_shapes.h"
#include "pulsar_render_commands.h"
#include "pulsar_opengl.h"

#include "pulsar_assets.h"
#include "pulsar_audio_mixer.h"
#include "pulsar_gjk.h"
#include "pulsar_entity.h"
#include "pulsar_editor.h"
#include "pulsar_console.h"

introspect() enum GameMode {
    GameMode_Menu,
    GameMode_Ingame,
    GameMode_Editor,

    GameMode_Count,
};

struct PlayingMidi {
    union {
        PlayingMidi* next;
        PlayingMidi* next_free;
    };

    SoundtrackID source_soundtrack;

    // @Note: Without a sync sound, midi timing is going to be pretty rubbish.
    // With a sync sound however, it will be very good.
    PlayingSound* sync_sound;

    f32 playback_rate;
    u32 tick_timer;
    u32 event_index;
    MidiTrack* track;

    u32 flags;
};

struct ActiveMidiEvent {
    using_struct(MidiEvent, midi_event);
    SoundtrackID source_soundtrack;
    f32 dt_left;
};

struct CameraView {
    v2 camera_p;
    v2 camera_rotation_arm;
    f32 vfov;
};

struct MenuState {
    GameMode source_gamemode;

    Font* font;
    Font* big_font;

    Sound* select_sound;
    Sound* confirm_sound;

    PlayingSound* music;

    u32 selected_item;
    f32 bob_t;

    f32 fade_in_timer;

    b32 asking_for_quit_confirmation;
    f32 quit_timer;
};

struct GameState {
    MemoryArena permanent_arena;
    MemoryArena transient_arena;

    RenderContext render_context;

    Assets assets;

    AudioMixer audio_mixer;
    AudioGroup game_audio;
    AudioGroup ui_audio;

    GameMode game_mode;

    MenuState* menu_state;
    ConsoleState* console_state;
    EditorState* editor_state;

    v4 foreground_color;
    v4 background_color;
    f32 background_pulse_t;
    f32 background_pulse_dt;

    ParticleSystem background_particles;

    f32 player_respawn_timer;

    Entity* player;
    Entity* last_activated_checkpoint;

    Entity* camera_target;
    Entity* previous_camera_zone;
    Entity* active_camera_zone;

    b32 mid_camera_transition;
    f32 camera_transition_t;

    CameraView death_cam_start;
    CameraView death_cam_end;

    String desired_level;

    Level* background_level;
    Level* active_level;

    f32 level_intro_timer;

    b32 midi_paused;
    PlayingMidi* first_playing_midi;
    PlayingMidi* first_free_playing_midi;

    u32 midi_event_buffer_count;
    ActiveMidiEvent midi_event_buffer[256];

    u32 entity_count;
    u32 entity_type_counts [EntityType_Count];
    u32 entity_type_offsets[EntityType_Count];
    Entity entities[MAX_ENTITY_COUNT];
};

inline Entity* get_entities_for_type(GameState* game_state, EntityType type, u32* count = 0) {
    assert(type > EntityType_Null && type < EntityType_Count);
    Entity* result = 0;

    if (game_state->entity_count > 0 && game_state->entity_type_counts[type] > 0) {
        result = game_state->entities + game_state->entity_type_offsets[type];
        assert(result < game_state->entities + game_state->entity_count);
    }

    if (count) {
        *count = game_state->entity_type_counts[type];
    }
    return result;
}

#define for_entity_type(game_state, type, it_identifier)                                                   \
    for (Entity* it_identifier = get_entities_for_type(game_state, type);                                  \
         it_identifier < get_entities_for_type(game_state, type) + (game_state)->entity_type_counts[type]; \
         it_identifier++)

inline b32 gjk_intersect_point(Transform2D t, Shape2D s, v2 p);
inline PlayingSound* play_soundtrack(GameState* game_state, SoundtrackID soundtrack_id, u32 flags = 0);

inline void switch_gamemode(GameState* game_state, GameMode game_mode);
internal void write_level_to_disk(GameState* game_state, Level* level, String level_name);
internal b32 load_level_from_disk(GameState* game_state, Level* level, String level_name);
internal void load_level(GameState* game_state, String level_name);

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
#undef DEFINE_COLORS

#endif /* PULSAR_MAIN_H */
