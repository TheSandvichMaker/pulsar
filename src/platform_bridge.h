#ifndef PLATFORM_BRIDGE_H
#define PLATFORM_BRIDGE_H

struct GameSoundOutputBuffer {
    u32 channel_count;
    u32 sample_rate;
    u32 sample_count;
    s16* samples;
};

#endif /* PLATFORM_BRIDGE_H */
