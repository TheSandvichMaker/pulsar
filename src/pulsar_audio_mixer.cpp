inline void initialize_audio_mixer(AudioMixer* mixer, MemoryArena* permanent_arena) {
    mixer->arena = permanent_arena;
    mixer->first_playing_sound = 0;
    mixer->first_free_playing_sound = 0;
    mixer->master_volume[0] = 1.0f;
    mixer->master_volume[1] = 1.0f;
}

inline PlayingSound* play_sound_internal(AudioMixer* mixer, u32 flags = 0) {
    if (!mixer->first_free_playing_sound) {
        mixer->first_free_playing_sound = push_struct(mixer->arena, PlayingSound);
        mixer->first_free_playing_sound->next = 0;
    }

    PlayingSound* playing_sound = mixer->first_free_playing_sound;
    mixer->first_free_playing_sound = playing_sound->next;

    zero_struct(*playing_sound);
    playing_sound->current_volume[0] = 0.5f;
    playing_sound->current_volume[1] = 0.5f;
    playing_sound->flags = flags;

    playing_sound->next = mixer->first_playing_sound;
    mixer->first_playing_sound = playing_sound;

    return playing_sound;
}

inline PlayingSound* play_sound(AudioMixer* mixer, Sound* sound, u32 flags = 0) {
    PlayingSound* playing_sound = play_sound_internal(mixer, flags);
    playing_sound->source_type = SoundSource_Sound;
    playing_sound->sound = sound;
    return playing_sound;
}

inline PlayingSound* play_synth(AudioMixer* mixer, Synth* synth, u32 flags = 0) {
    PlayingSound* playing_sound = play_sound_internal(mixer, flags);
    playing_sound->source_type = SoundSource_Synth;
    playing_sound->synth = synth;
    return playing_sound;
}

internal SOUND_SYNTH(synth_test_tone) {
    f32 t = cast(f32) sample_index / cast(f32) sample_rate;
    f32 result = sin(TAU_32*t*pitch);
    return result;
}

internal SOUND_SYNTH(synth_test_impulse) {
    f32 t = cast(f32) sample_index / cast(f32) sample_rate;
    f32 result = sin(TAU_32*t*pitch)*exp(-t*16.0f);
    return result;
}

inline void change_volume(PlayingSound* sound, f32 t, v2 target_volume) {
    if (t > 0.0f) {
        sound->dv_over_t[0] = (target_volume.e[0] - sound->current_volume[0]) / t;
        sound->dv_over_t[1] = (target_volume.e[1] - sound->current_volume[1]) / t;
        sound->target_volume[0] = target_volume.e[0];
        sound->target_volume[1] = target_volume.e[1];
    } else {
        sound->current_volume[0] = target_volume.e[0];
        sound->current_volume[1] = target_volume.e[1];
    }
}

inline void stop_all_sounds(AudioMixer* mixer) {
    PlayingSound* last_playing_sound = mixer->first_playing_sound;
    if (last_playing_sound) {
        while (last_playing_sound->next) {
            last_playing_sound = last_playing_sound->next;
        }
        last_playing_sound->next = mixer->first_free_playing_sound;
        mixer->first_free_playing_sound = mixer->first_playing_sound;
        mixer->first_playing_sound = 0;
    }
}

internal void output_playing_sounds(AudioMixer* mixer, GameSoundOutputBuffer* sound_buffer, MemoryArena* temp_arena) {
    // @TODO:
    // Formalize the handling of channels in the mixer.
    // Restore Handmade Hero functionality (smooth volume fade, variable playback rate).
    //
    // I think the having to output speculative audio versus making canonical
    // changes has a risk of spiraling into error prone complexity, as the
    // volume fades already are starting to demonstrate. There should be a way
    // to make that better. Specifically, using the same code paths for the
    // speculative and canonical parts seems like a key point.
    TemporaryMemory mixer_memory = begin_temporary_memory(temp_arena);

    u32 sample_count = sound_buffer->samples_to_write;

    f32* float_channel0 = push_array(temp_arena, sample_count, f32);
    f32* float_channel1 = push_array(temp_arena, sample_count, f32);

    for (PlayingSound** playing_sound_ptr = &mixer->first_playing_sound; *playing_sound_ptr;) {
        PlayingSound* playing_sound = *playing_sound_ptr;

        b32 sound_finished = false;
        b32 looping = playing_sound->flags & Playback_Looping;

        if (playing_sound->initialized) {
            // @Incomplete: With this approach, if left unaccounted for, midi sync will have a 1 frame delay
            // @Incomplete: This approach as-is will not work with a smooth variable playback rate (maths may be able to get to the rescue)
            playing_sound->samples_played += sound_buffer->samples_committed;

            for (u32 channel = 0; channel < 2; channel++) {
                f32 volume_c = playing_sound->dv_over_t[channel] / cast(f32) sound_buffer->sample_rate;

                playing_sound->current_volume[channel] += volume_c*sound_buffer->samples_committed;

                if (volume_c > 0.0f) {
                    playing_sound->current_volume[channel] = min(playing_sound->current_volume[channel], playing_sound->target_volume[channel]);
                } else {
                    playing_sound->current_volume[channel] = max(playing_sound->current_volume[channel], playing_sound->target_volume[channel]);
                }
            }
        } else {
            playing_sound->samples_played = 0;
            playing_sound->initialized = true;
        }

        if (playing_sound->synced_midi) {
            // @Note: This is here for the sake of handling looping correctly, but I don't like it. I want to move this
            // out of the mixer.
            playing_sound->synced_midi->tick_timer = playing_sound->samples_played;
        }

        if (playing_sound->source_type == SoundSource_Sound) {
            Sound* sound = playing_sound->sound;

            if (sound) {
                if (looping) {
                    if (playing_sound->samples_played >= sound->sample_count) {
                        playing_sound->samples_played %= sound->sample_count;
                    }
                } else if (playing_sound->samples_played >= sound->sample_count) {
                    playing_sound->samples_played = sound->sample_count;
                    sound_finished = true;
                }
            } else {
                sound_finished = true;
            }
        } else {
            assert(playing_sound->source_type == SoundSource_Synth);
            if (!playing_sound->synth) {
                sound_finished = true;
            }
        }

        if (sound_finished) {
            *playing_sound_ptr = playing_sound->next;
            playing_sound->next = mixer->first_free_playing_sound;
            mixer->first_free_playing_sound = playing_sound;
        } else {
            u32 samples_played = playing_sound->samples_played;
            u32 total_samples_to_mix = sound_buffer->samples_to_write;

            f32* dest0 = float_channel0;
            f32* dest1 = float_channel1;
            f32* dest[2] = { dest0, dest1 };

            if (playing_sound->source_type == SoundSource_Sound) {
                Sound* sound = playing_sound->sound;

                while (sound && total_samples_to_mix > 0) {
                    if (looping) {
                        if (samples_played >= sound->sample_count) {
                            samples_played %= sound->sample_count;
                        }
                    }

                    assert(playing_sound->samples_played >= 0 && playing_sound->samples_played < sound->sample_count);

                    u32 samples_to_mix = total_samples_to_mix;
                    u32 samples_remaining_in_sound = looping ? sound->sample_count : sound->sample_count - playing_sound->samples_played;

                    if (!looping) {
                        if (samples_to_mix > samples_remaining_in_sound) {
                            samples_to_mix = samples_remaining_in_sound;
                        }
                    }

                    f32 volume_c[2];
                    volume_c[0] = playing_sound->dv_over_t[0] / cast(f32) sound_buffer->sample_rate;
                    volume_c[1] = playing_sound->dv_over_t[1] / cast(f32) sound_buffer->sample_rate;

                    for (u32 sample_index = 0; sample_index < samples_to_mix; sample_index++) {
                        u32 mapped_sample_index = (samples_played + sample_index) % sound->sample_count;

                        for (u32 channel = 0; channel < sound->channel_count; channel++) {
                            f32 volume = volume_c[channel]*sample_index + playing_sound->current_volume[channel];
                            if (volume_c[channel] > 0.0f) {
                                volume = min(volume, playing_sound->target_volume[channel]);
                            } else {
                                volume = max(volume, playing_sound->target_volume[channel]);
                            }

                            f32 sample_value = cast(f32) (sound->samples + sound->sample_count*channel)[mapped_sample_index];
                            *dest[channel]++ += volume*(sample_value / cast(f32) INT16_MAX);
                        }
                    }

                    assert(total_samples_to_mix >= samples_to_mix);
                    total_samples_to_mix -= samples_to_mix;

                    samples_played += samples_to_mix;

                    if (!looping && samples_to_mix >= samples_remaining_in_sound) {
                        break;
                    }
                }
            } else {
                assert(playing_sound->source_type == SoundSource_Synth);
                for (u32 sample_index = 0; sample_index < total_samples_to_mix; sample_index++) {
                    // @TODO: Formalize channels
                    for (u32 channel = 0; channel < 2; channel++) {
                        // @TODO: Decide on a way synths can communicate they're done playing
                        *dest[channel]++ += cast(f32) playing_sound->current_volume[channel]*playing_sound->synth(2, channel, sound_buffer->sample_rate, samples_played + sample_index, 440.0f);
                    }
                }
            }

            playing_sound_ptr = &playing_sound->next;
        }
    }

    // @Note: Convert to 16 bit and output to sound buffer
    // @TODO: Dither
    f32* source0 = float_channel0;
    f32* source1 = float_channel1;
    s16* sample_out = sound_buffer->samples;
    for (u32 sample_index = 0; sample_index < sample_count; sample_index++) {
        *sample_out++ = cast(s16) (mixer->master_volume[0]*(INT16_MAX*(*source0++)));
        *sample_out++ = cast(s16) (mixer->master_volume[1]*(INT16_MAX*(*source1++)));
    }

    end_temporary_memory(mixer_memory);
}
