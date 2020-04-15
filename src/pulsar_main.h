#ifndef PULSAR_MAIN_H
#define PULSAR_MAIN_H

/* General Info:
 * The idea of this game was to interpret the theme "waves" rather cheesily as pertaining to soundwaves, but
 * less cheesily to embody the cyclical repetitive nature of rhythms. The hope was that you'd get a platformer
 * where you kind of ride the wave of the soundtrack, getting into a flow navigating the obstacles based on musical
 * queues. In the end, I don't think what I've got now is very fun or embodies that idea that strongly, in terms
 * of design challenges and in the gameplay features and editor capabilities I think there's still a lot more to be
 * gained. But for the work I've put in over the past two months, I'm satisfied with the result.
 *
 * In terms of building the project, you'll need to use vcvarsall.bat or the visual studio developer prompt or whatever
 * to make sure you've got the visual C++ environment variables set up. Other than that, there aren't any external
 * dependencies, all libraries used are in source code form in the src folder, and DirectSound and XInput are linked
 * at runtime.
 *
 * Before you use build.bat to build the game, you'll have to make sure the code generator is built, you can use
 * build_code_generator.bat for this. build.bat will accept the argument "release" to build a release build.
 * Inside the misc folder, you will find ctime.exe, a small program written by Casey Muratori which helps you
 * track your compile times. If this isn't added to PATH or placed next to build.bat, you'll see build.bat complain
 * that it can't find it. This shouldn't affect anything though.
 *
 * pulsar_config.pcf contains various settings (it hotloads, so these settings can mostly be changed at runtime),
 * and things such as the keyboard controls. Controls for gamepad aren't rebindable, and are left stick or dpad to move,
 * A to jump, back to respawn and start for menu.
 *
 * Editor controls:
 * Space + LMB     : Pan
 * Shift + W/D     : Zoom in / out
 * Shift + Q/E     : Increase / decrease grid size
 * S               : Toggle grip snapping
 * Ctrl + D        : Duplicate selected entities
 * Delete          : Delete selected entities
 * Shift (Hold)    : Show spawn menu
 * Shift (Release) : Spawn selected entity
 * Ctrl + Z        : Undo
 * Ctrl + R        : Redo
 * Ctrl + S        : Save level
 * M               : Toggle showing all move widgets
 *
 * Some editor actions you will want to undertake are only available as console commands:
 *     toggle_flag [flag]                     : Toggle the given flag on the selected entities
 *     set_soundtrack [soundtrack asset name] : Set soundtrack_id on selected SoundtrackPlayers and listening_to on selected Walls
 *     save_level [level_name]                : If you want to save the current level with a different name
 *     load_level [level_name]                : Loads another level, but is sort of broken. Use startup_level instead.
 *
 * Right now there sort of isn't good support for making a new level or loading it properly, I didn't get around to fixing that since
 * it was low priority for getting my demo level done. You can save the current level as a new level with the save_level console command,
 * and then set this new level as the startup_level in pulsar_config.pcf instead.
 */

/* The Special Handmade Hero credits section:
 * On Youtube: https://www.youtube.com/user/handmadeheroarchive
 * Handmade Hero is an ongoing tutorial series by Casey Muratori which covers how to program a game from scratch in C++ without using
 * any libraries whatsoever. It has been a big influence on my coding style and is the source of a large amount of my game programming knowledge,
 * so I opted to use that knowledge (and some of the code directly) from Handmade Hero in place of using something like a library.
 * The upshot is that I've coded along 300 episodes of Handmade Hero by hand, trying to understand each line and debugging all my own
 * bugs along the way, as well as making my own minor changes, improvements, and fixes as I came across them. This means that unlike
 * if I used libraries, I am totally familiar with the code in question and why + how it works.
 *
 * What follows is a list, as complete as I could make it from the top of my head, of things partially or entirely adopted from Handmade Hero.
 * Often, I haven't actually copied the code from Handmade Hero directly, but I've reimplemented the same ideas / techniques from scratch
 * using just my knowledge and understanding of them. But then the end result often looked so close to the original Handmade Hero code that
 * I wouldn't dare claim ownership of it.
 *
 * Memory Arenas, Temporary Memory (minor changes, although LinearBuffer is my own)
 * You'll also find that AllocateParams in pulsar_memory.h comes from Handmade Hero's ArenaPushParams which I made more general.
 *
 * Win32 window setup (with minor changes?)
 * OpenGL setup (with minor changes)
 * WGL setup (with minor changes)
 * XInput loading (with minor changes)
 * DirectSound setup (virtually unchanged)
 * Platform Layer style API design (with minor changes, I actually don't like the way this turned out so far and I'd redo it if I had time)
 * Win32 file reading and writing (straight copy)
 * Various Win32 setup things like initializing the perf counter, getting the refresh rate, understanding how the Win32 API works in general.
 * GameButtonState, GameController, and surrounding functionality (straight copy, except for of course the actual buttons used)
 * Various Win32 input handling things, like mouse buttons and the way input messages are handled in win32_handle_remaining_messages (not event mode though, that's my own)
 * XInput usage (minor changes)
 *
 * Sorting (no changes)
 *
 * Basic code generator framework (but I built upon it a lot and have written more complex parsers before,
 *   so using the handmade hero code generator as a base isn't really much of a leg up. Mostly seeing Casey
 *   do it just gave me confidence to pursue the idea. The Handmade Hero code generator was only covered in
 *   two episodes and isn't actually in use in Handmade Hero, it only did very basic struct introspection)
 *
 * Initial framework for the mixer and output_playing_sounds (I've overhauled output_playing_sounds a good bit,
 *   and have made some additions to the mixer, as well as redoing how the game interfaces with DirectSound)
 *
 * Most of math.h (although a lot of it I would've written myself just as happily anyway)
 *
 * load_bitmap and load_wav (and related RIFF stuff) in pulsar_asset_packer
 * General idea and broad design of the asset pack file (but the actual asset packer and pack format is my own)
 *
 * UILayout idea and broad design (implemented the same idea from memory)
 * EditorWidget... kind of. It's not entirely like in Handmade Hero and the design of is still in quite a lot of flux,
 * but the general ImGUI ideas are definitely from Casey's examples and influence.
 *
 * Renderer design (GameRenderCommands, RenderContexts are modelled after Handmade Hero's RenderGroups)
 * OpenGL functions (various opengl helper functions and opengl_render_commands will look very familiar, although my own render commands are a bit different)
 */

/* Other references:
 * General Knowledge and Insight from watching Jonathan Blow streams (my console and main menu vaguely pulled from streams I've watched of him implementing such things):
 *     https://www.youtube.com/user/jblow888
 * GJK, EPA (not that I actually used these in the end result, but the code is there and works):
 *     https://caseymuratori.com/blog_0003
 *     http://www.dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
 *     http://www.dyn4j.org/2010/05/epa-expanding-polytope-algorithm/
 * Computing the winding of a polygon:
 *     http://paulbourke.net/geometry/polygonmesh/
 * Framerate Independence / Timing
 *     https://www.youtube.com/watch?v=fdAOPHgW7qM
 *     https://www.youtube.com/watch?v=jTzIDmjkLQo
 * Immediate mode GUIs:
 *     Handmade Hero Debug System
 *     https://www.youtube.com/watch?v=Z1qyvQsjK5Y
 * MIDI:
 *     https://www.midi.org/specifications-old/item/the-midi-1-0-specification
 * Various things I've learned over time and useful functions like smooth_min:
 *     https://www.iquilezles.org/www/index.htm
 */

/* Resources:
 * Sounds and Music:
 *     all created by me (clearance on some of the samples used is fuzzy)
 * Images:
 *     camera icon: Random google images
 *     speaker icon: Random google images
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

    Entity* source_soundtrack_player;

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
    GameMode source_game_mode;

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

    struct {
        Sound* player_land;
        Sound* player_high_jump;
        Sound* player_jump[3];
        Sound* player_footsteps[3];
    } sounds;

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
    f32 player_self_destruct_timer;

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
    f32 level_outro_timer;

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

#define for_entity_type(game_state, type, it)                                 \
    for (Entity* it = get_entities_for_type(game_state, type),                \
               * it##_loop_end = it + (game_state)->entity_type_counts[type]; \
         it < it##_loop_end;                                                  \
         ++it)

inline PlayingSound* play_soundtrack(GameState* game_state, Entity* soundtrack_player, u32 flags = 0);

inline void switch_game_mode(GameState* game_state, GameMode game_mode);
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
