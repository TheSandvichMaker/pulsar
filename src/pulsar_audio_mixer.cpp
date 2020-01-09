internal void initialize_audio_mixer(AudioMixer* mixer, MemoryArena* permanent_arena) {
    mixer->arena = permanent_arena;
    mixer->first_playing_sound = 0;
    mixer->first_free_playing_sound = 0;
    mixer->master_volume[0] = 1.0f;
    mixer->master_volume[1] = 1.0f;
}

internal PlayingSound* play_sound(AudioMixer* mixer, Sound* sound, u32 flags = 0) {
    if (!mixer->first_free_playing_sound) {
        mixer->first_free_playing_sound = push_struct(mixer->arena, PlayingSound);
        mixer->first_free_playing_sound->next = 0;
    }

    PlayingSound* playing_sound = mixer->first_free_playing_sound;
    mixer->first_free_playing_sound = playing_sound->next;

    playing_sound->samples_played = 0;
    playing_sound->current_volume[0] = 0.5f;
    playing_sound->current_volume[1] = 0.5f;
    playing_sound->sound = sound;
    playing_sound->flags = flags;

    playing_sound->next = mixer->first_playing_sound;
    mixer->first_playing_sound = playing_sound;

    return playing_sound;
}

internal void change_volume(PlayingSound* sound, v2 target_volume) {
    sound->current_volume[0] = target_volume.e[0];
    sound->current_volume[1] = target_volume.e[1];
}

// NOTE: pan is in degrees using the constant power law, -45° for full left, 0° for center, 45° for full right.
// TODO: note that fading between different pans is linear and thus slightly incorrect.
inline void change_balance(PlayingSound* sound, f32 amplitude, f32 pan) {
    f32 pow = square_root(2.0f) / 2.0f;
    f32 theta = deg_to_rad(pan);
    v2 volume = pow * vec2(cos(theta), sin(theta));
    change_volume(sound, amplitude * volume);
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
    /* TODO:
     * Formalize the handling of channels in the mixer.
     *
     * Restore Handmade Hero functionality.
     *
     * ..
     *
     * Handmade Hero functionality TODOs:
     * Have volume exposed as a scalar, and automate pan together with volume
     * to avoid the "gap in the middle" effect during panning (by using a
     * better pan law than just straight linear interpolation).
     *
     * Allow for the smooth automation of pitch bends.
     */

    if (sound_buffer->samples_to_write > 0) {
        TemporaryMemory mixer_memory = begin_temporary_memory(temp_arena);

        u32 sample_count = sound_buffer->samples_to_write;

        f32* float_channel0 = push_array(temp_arena, sample_count, f32);
        f32* float_channel1 = push_array(temp_arena, sample_count, f32);

        f32 seconds_per_sample = 1.0f / sound_buffer->sample_rate;

        for (PlayingSound** playing_sound_ptr = &mixer->first_playing_sound; *playing_sound_ptr;) {
            PlayingSound* playing_sound = *playing_sound_ptr;

            b32 sound_finished = false;
            b32 looping = playing_sound->flags & Playback_Looping;

            Sound* sound = playing_sound->sound;

            if (sound) {
                // @Incomplete: This approach will not work with a smooth variable playback rate.
                playing_sound->samples_played += sound_buffer->samples_committed;

                if (looping) {
                    if (playing_sound->samples_played >= sound->sample_count) {
                        playing_sound->samples_played %= sound->sample_count;
                    }
                } else if (playing_sound->samples_played >= sound->sample_count) {
                    sound_finished = true;
                }
            } else {
                sound_finished = true;
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

                while (total_samples_to_mix > 0) {
                    v2 volume;
                    volume.e[0] = mixer->master_volume[0]*playing_sound->current_volume[0];
                    volume.e[1] = mixer->master_volume[1]*playing_sound->current_volume[1];

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

                    u32 begin_sample_position = samples_played;
                    u32 end_sample_position = begin_sample_position + samples_to_mix;
                    for (u32 sample_index = begin_sample_position; sample_index < end_sample_position; sample_index++) {
                        u32 wrapped_sample_index = sample_index;
                        if (wrapped_sample_index >= sound->sample_count) {
                            wrapped_sample_index -= sound->sample_count;
                        }

                        for (u32 channel = 0; channel < sound->channel_count; channel++) {
                            f32 sample_value = cast(f32) (sound->samples + sound->sample_count*channel)[wrapped_sample_index];
                            *dest[channel]++ += volume.e[channel]*sample_value;
                        }
                    }

                    assert(total_samples_to_mix >= samples_to_mix);
                    total_samples_to_mix -= samples_to_mix;

                    samples_played += samples_to_mix;

                    if (!looping && samples_to_mix >= samples_remaining_in_sound) {
                        break;
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
            *sample_out++ = cast(s16) (*source0++);
            *sample_out++ = cast(s16) (*source1++);
        }

        end_temporary_memory(mixer_memory);
    }
}
