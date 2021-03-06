#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

enum PlaybackFlag {
    Playback_Looping = 0x1,
};

enum SoundCategory {

};

enum SoundSourceType {
    SoundSource_Sound,
    SoundSource_Synth,
};

#define SOUND_SYNTH(name) f32 name(u32 channel_count, u32 channel_index, u32 sample_rate, u32 sample_index, f32 pitch)
typedef SOUND_SYNTH(Synth);

struct SoundVolume {
    f32 current_volume[2];
    f32 dv_over_t[2];
    f32 target_volume[2];
};

struct PlayingSound {
    b32 initialized;

    SoundVolume volume;

    f32 playback_rate;
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

struct AudioGroup {
    struct AudioMixer* mixer;

    f32 mix_volume[2];

    b32 pause_requested;
    b32 paused;
    SoundVolume volume;
    f32 volume_on_unpause[2];

    PlayingSound* first_playing_sound;
    AudioGroup* next_audio_group;
};

struct AudioMixer {
    MemoryArena* arena;

    f32 master_volume[2];

    AudioGroup* first_audio_group;
    PlayingSound* first_free_playing_sound;
};

#endif /* AUDIO_MIXER_H */
