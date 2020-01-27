inline void initialize_audio_mixer(AudioMixer* mixer, MemoryArena* permanent_arena) {
    mixer->arena = permanent_arena;
    mixer->first_audio_group = 0;
    mixer->first_free_playing_sound = 0;
    mixer->master_volume[0] = 1.0f;
    mixer->master_volume[1] = 1.0f;
}

inline void initialize_audio_group(AudioGroup* group, AudioMixer* mixer) {
    group->mixer = mixer;
    group->volume.current_volume[0] = 1.0f;
    group->volume.current_volume[1] = 1.0f;
    group->volume.dv_over_t[0]      = 0.0f;
    group->volume.dv_over_t[1]      = 0.0f;
    group->volume.target_volume[0]  = 1.0f;
    group->volume.target_volume[1]  = 1.0f;
    group->volume_on_unpause[0] = group->volume.current_volume[0];
    group->volume_on_unpause[1] = group->volume.current_volume[1];
    group->first_playing_sound = 0;

    group->next_audio_group = mixer->first_audio_group;
    mixer->first_audio_group = group;
}

inline PlayingSound* play_sound_internal(AudioGroup* group, v2 starting_volume, u32 flags = 0) {
    AudioMixer* mixer = group->mixer;

    if (!mixer->first_free_playing_sound) {
        mixer->first_free_playing_sound = push_struct(mixer->arena, PlayingSound);
        mixer->first_free_playing_sound->next = 0;
    }

    PlayingSound* playing_sound = mixer->first_free_playing_sound;
    mixer->first_free_playing_sound = playing_sound->next;

    *playing_sound = {};
    playing_sound->playback_rate = 1.0f;
    playing_sound->volume.current_volume[0] = starting_volume.x;
    playing_sound->volume.current_volume[1] = starting_volume.y;
    playing_sound->flags = flags;

    playing_sound->next = group->first_playing_sound;
    group->first_playing_sound = playing_sound;

    return playing_sound;
}

inline PlayingSound* play_sound(AudioGroup* group, Sound* sound, v2 starting_volume = vec2(0.5f, 0.5f), u32 flags = 0) {
    PlayingSound* playing_sound = play_sound_internal(group, starting_volume, flags);
    playing_sound->source_type = SoundSource_Sound;
    playing_sound->sound = sound;
    return playing_sound;
}

inline PlayingSound* play_synth(AudioGroup* group, Synth* synth, v2 starting_volume = vec2(0.5f, 0.5f), u32 flags = 0) {
    PlayingSound* playing_sound = play_sound_internal(group, starting_volume, flags);
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

inline void change_volume(SoundVolume* volume, f32 t, v2 target_volume) {
    if (t > 0.0f) {
        volume->dv_over_t[0] = (target_volume.e[0] - volume->current_volume[0]) / t;
        volume->dv_over_t[1] = (target_volume.e[1] - volume->current_volume[1]) / t;
        volume->target_volume[0] = target_volume.e[0];
        volume->target_volume[1] = target_volume.e[1];
    } else {
        volume->current_volume[0] = target_volume.e[0];
        volume->current_volume[1] = target_volume.e[1];
    }
}

inline void change_volume(PlayingSound* sound, f32 t, v2 target_volume) {
    change_volume(&sound->volume, t, target_volume);
}

inline void change_volume(AudioGroup* group, f32 t, v2 target_volume) {
    change_volume(&group->volume, t, target_volume);
    group->volume_on_unpause[0] = target_volume.e[0];
    group->volume_on_unpause[1] = target_volume.e[1];
}

inline void pause_group(AudioGroup* group, f32 t) {
    group->pause_requested = true;
    group->volume_on_unpause[0] = group->volume.current_volume[0];
    group->volume_on_unpause[1] = group->volume.current_volume[1];
    change_volume(&group->volume, t, vec2(0, 0));
}

inline void unpause_group(AudioGroup* group, f32 t) {
    group->paused = false;
    change_volume(group, t, vec2(group->volume_on_unpause[0], group->volume_on_unpause[1]));
}

inline void change_playback_rate(PlayingSound* sound, f32 playback_rate) {
    sound->playback_rate = clamp(playback_rate, 0.1f, 32.0f);
}

inline void stop_all_sounds(AudioGroup* group) {
    AudioMixer* mixer = group->mixer;

    PlayingSound* last_playing_sound = group->first_playing_sound;
    if (last_playing_sound) {
        while (last_playing_sound->next) {
            last_playing_sound = last_playing_sound->next;
        }
        last_playing_sound->next = mixer->first_free_playing_sound;
        mixer->first_free_playing_sound = group->first_playing_sound;
        group->first_playing_sound = 0;
    }
}

inline void stop_all_sounds(AudioMixer* mixer) {
    for (AudioGroup* group = mixer->first_audio_group; group; group = group->next_audio_group) {
        stop_all_sounds(group);
    }
}

inline f32 get_volume_for_sample_offset(SoundVolume volume, u32 channel, u32 sample_rate, u32 sample_offset) {
    f32 slope  = volume.dv_over_t[channel] / cast(f32) sample_rate;
    f32 result = volume.current_volume[channel] + slope*sample_offset;
    if (slope > 0.0f) {
        result = min(result, volume.target_volume[channel]);
    } else {
        result = max(result, volume.target_volume[channel]);
    }
    return result;
}

internal void output_playing_sounds(AudioMixer* mixer, GameSoundOutputBuffer* sound_buffer, MemoryArena* temp_arena) {
    // @TODO: Formalize the handling of channels in the mixer.
    // @TODO: I think the having to output speculative audio versus making canonical changes has a risk of spiraling
    // into error prone complexity. It would be nice to use the same code paths wherever possible.
    // @TODO: Linear interpolation for varyin playback rates

    TemporaryMemory mixer_memory = begin_temporary_memory(temp_arena);

    u32 sample_count = sound_buffer->samples_to_write;

    f32* float_channel0 = push_array(temp_arena, sample_count, f32);
    f32* float_channel1 = push_array(temp_arena, sample_count, f32);

    for (AudioGroup* group = mixer->first_audio_group; group; group = group->next_audio_group) {
        if (group->paused) {
            continue;
        }

        b32 all_channels_quiet = true;
        for (u32 channel = 0; channel < 2; channel++) {
            group->volume.current_volume[channel] = get_volume_for_sample_offset(group->volume, channel, sound_buffer->sample_rate, sound_buffer->samples_committed);
            if (group->volume.current_volume[channel] > 0.0f) {
                all_channels_quiet = false;
            }
        }

        if (group->pause_requested && all_channels_quiet) {
            group->pause_requested = false;
            group->paused = true;
            continue;
        }

        for (PlayingSound** playing_sound_ptr = &group->first_playing_sound; *playing_sound_ptr;) {
            PlayingSound* playing_sound = *playing_sound_ptr;

            f32 playback_rate = game_config->simulation_rate*playing_sound->playback_rate;

            b32 sound_finished = false;
            b32 looping = playing_sound->flags & Playback_Looping;

            if (playing_sound->initialized) {
                // @Incomplete: With this approach, if left unaccounted for, midi sync will have a 1 frame delay
                // @Incomplete: This approach as-is will not work with a smooth variable playback rate (maths may be able to get to the rescue)
                playing_sound->samples_played += cast(u32) (cast(f32) sound_buffer->samples_committed*playback_rate);

                for (u32 channel = 0; channel < 2; channel++) {
                    playing_sound->volume.current_volume[channel] = get_volume_for_sample_offset(playing_sound->volume, channel, sound_buffer->sample_rate, sound_buffer->samples_committed);
                }
            } else {
                playing_sound->samples_played = 0;
                playing_sound->initialized = true;
            }

            if (playing_sound->synced_midi) {
                // @Note: This is here for the sake of handling looping correctly, but I don't like it. I want to move this
                // out of the mixer. Or midi into the mixer entirely.
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

                        for (u32 sample_index = 0; sample_index < samples_to_mix; sample_index++) {
                            u32 adjusted_sample_index = cast(u32) (cast(f32) sample_index*playback_rate);
                            u32 mapped_sample_index = (samples_played + adjusted_sample_index) % sound->sample_count;

                            for (u32 channel = 0; channel < sound->channel_count; channel++) {
                                f32 volume = get_volume_for_sample_offset(playing_sound->volume, channel, sound_buffer->sample_rate, adjusted_sample_index);
                                f32 group_volume = group->mix_volume[channel]*get_volume_for_sample_offset(group->volume, channel, sound_buffer->sample_rate, adjusted_sample_index);

                                f32 sample_value = cast(f32) (sound->samples + sound->sample_count*channel)[mapped_sample_index];
                                *dest[channel]++ += group_volume*volume*(sample_value / cast(f32) INT16_MAX);
                            }
                        }

                        assert(total_samples_to_mix >= samples_to_mix);
                        total_samples_to_mix -= samples_to_mix;

                        samples_played += cast(u32) (cast(f32) samples_to_mix*playback_rate);

                        if (!looping && samples_to_mix >= samples_remaining_in_sound) {
                            break;
                        }
                    }
                } else {
                    assert(playing_sound->source_type == SoundSource_Synth);
                    for (u32 sample_index = 0; sample_index < total_samples_to_mix; sample_index++) {
                        // @TODO: Formalize channels
                        for (u32 channel = 0; channel < 2; channel++) {
                            f32 volume = get_volume_for_sample_offset(playing_sound->volume, channel, sound_buffer->sample_rate, sample_index);
                            f32 group_volume = group->mix_volume[channel]*get_volume_for_sample_offset(group->volume, channel, sound_buffer->sample_rate, sample_index);
                            // @TODO: Decide on a way synths can communicate they're done playing
                            *dest[channel]++ += volume*group_volume*playing_sound->synth(2, channel, sound_buffer->sample_rate, samples_played + sample_index, 440.0f);
                        }
                    }
                }

                playing_sound_ptr = &playing_sound->next;
            }
        }
    }

    // @Note: Convert to 16 bit and output to sound buffer
    // @TODO: Dither
    f32* source0 = float_channel0;
    f32* source1 = float_channel1;
    s16* sample_out = sound_buffer->samples;
    for (u32 sample_index = 0; sample_index < sample_count; sample_index++) {
        *sample_out++ = cast(s16) (INT16_MAX*clamp(mixer->master_volume[0]*(*source0++), -1.0f, 1.0f));
        *sample_out++ = cast(s16) (INT16_MAX*clamp(mixer->master_volume[1]*(*source1++), -1.0f, 1.0f));
    }

    end_temporary_memory(mixer_memory);
}
