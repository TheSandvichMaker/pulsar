#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

enum PlaybackFlag {
    Playback_Looping = 0x1,
};

enum SoundSourceType {
    SoundSource_Sound,
    SoundSource_Synth,
};

#define SOUND_SYNTH(name) f32 name(u32 channel_count, u32 channel_index, u32 sample_rate, u32 sample_index, f32 pitch)
typedef SOUND_SYNTH(Synth);

struct PlayingSound {
    b32 initialized;

    f32 current_volume[2];
    f32 dv_over_t[2]; // v: volume
    f32 target_volume[2];

    u32 samples_played;

    u32 flags;

    SoundSourceType source_type;
    union {
        Sound* sound;
        Synth* synth;
    };

    struct PlayingMidi* synced_midi; // Icky

    PlayingSound* next;
};

struct AudioMixer {
    MemoryArena* arena;

    f32 master_volume[2];

    PlayingSound* first_playing_sound;
    PlayingSound* first_free_playing_sound;
};

#endif /* AUDIO_MIXER_H */
