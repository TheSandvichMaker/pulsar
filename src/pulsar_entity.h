#ifndef PULSAR_ENTITY_H
#define PULSAR_ENTITY_H

struct EntityID { u32 value; };

introspect(flags: true) enum EntityFlag {
    EntityFlag_Physical  = 0x1,
    EntityFlag_Collides  = 0x2,
    EntityFlag_OnGround  = 0x4,
    EntityFlag_Invisible = 0x8,
    EntityFlag_Hazard    = 0x10,
};

introspect() enum EntityType {
    EntityType_Null,

    // @Note: Physical Entities
    EntityType_Player = 1,
    EntityType_Wall   = 2,

    // @Note: Non-physical Entities
    EntityType_SoundtrackPlayer = 3,
    EntityType_CameraZone       = 4,
    EntityType_Checkpoint       = 5,

    EntityType_Count,
};

introspect() enum WallBehaviour {
    WallBehaviour_None,

    WallBehaviour_Move = 1,
    WallBehaviour_Toggle = 2,

    WallBehaviour_Count,
};

struct Entity {
    EntityID guid;
    EntityType type;

    u32 flags;

    b32 killed_this_frame;
    b32 dead;

    v2 p;
    v2 dp;
    v2 ddp;
    v2 ballistic_dp;

    v2 collision;

    ImageID sprite;
    v4 color;

    union {
        struct {
            // @Note: Player
            f32 early_jump_timer;
            f32 late_jump_timer;

            f32 gravity;

            b32 was_supported;
            Entity* support;
            v2 support_dp;
            v2  retained_support_dp;
            f32 retained_support_dp_timer;
            v2 support_normal;

            Entity* contact;
            v2 contact_move;
        };

        struct {
            // @Note: Wall
            WallBehaviour behaviour;

            u32 midi_note;
            SoundtrackID listening_to;
            v2 end_p;
            f32 movement_speed_ms;

            v2 start_p;
            b32 moving_to_end;
            f32 movement_t;
        };

        struct {
            // @Note: Soundtrack Player
            SoundtrackID soundtrack_id;
            u32 playback_flags;
            v2 audible_zone;
            f32 horz_fade_region;
            f32 vert_fade_region;

            PlayingSound* playing;
        };

        struct {
            // @Note: Camera Zone;
            v2 active_region;
            f32 view_region_height;
            v2 camera_rotation_arm;
        };

        struct {
            // @Note: Checkpoint
            v2 checkpoint_zone;
            v2 most_recent_player_position;
        };
    };
};

#endif /* PULSAR_ENTITY_H */
