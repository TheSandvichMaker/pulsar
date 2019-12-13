#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#include "common.h"
#include "memory_arena.h"
#include "platform_bridge.h"

#include "string.h"
#include "math.h"
#include "opengl.h"

#include "game_assets.h"
#include "audio_mixer.h"

struct GameState {
    MemoryArena permanent_arena;
    MemoryArena transient_arena;

    AudioMixer audio_mixer;
    Assets assets;

    Sound* test_music;
    Sound* test_sound;
    Image* test_image;

    v2 p1;
    v2 p2;

    u32 sound_timer;
};

global PlatformAPI platform;

#endif /* GAME_MAIN_H */
