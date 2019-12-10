#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

struct PlayingSound {
    f32 current_volume[2];
    u32 samples_played;

    Sound* sound;

    PlayingSound* next;
};

struct AudioMixer {
    MemoryArena* arena;

    f32 master_volume[2];

    PlayingSound* first_playing_sound;
    PlayingSound* first_free_playing_sound;
};

#endif /* AUDIO_MIXER_H */
