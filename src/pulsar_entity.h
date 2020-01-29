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
    EntityType_TriggerZone      = 6,

    EntityType_Count,
};

introspect() enum WallBehaviour {
    WallBehaviour_None,

    WallBehaviour_Move   = 1,
    WallBehaviour_Toggle = 2,

    WallBehaviour_Count,
};

introspect() enum TriggerBehaviour {
    TriggerBehaviour_None,

    TriggerBehaviour_EndLevel,

    TriggerBehaviour_Count,
};

struct Entity {
    EntityID guid;
    EntityType type;

    u32 flags;

    b32 killed_this_frame; // @Note: this is to defer entity death to the end of the frame, so they still get rendered on the frame on which they died
    b32 dead;

    v2 p;
    v2 dp;
    v2 ddp;

    v2 collision;

    ImageID sprite;
    v4 color;

    // @TODO: Maybe eventually make this more safe by giving these structs names
    // P.S. that doesn't mean they have to have a type, you can use auto in C++
    // to refer to the type of whatever these otherwise anonymous struct types
    // might be.
    union {
        struct /* Player */ {
            f32 early_jump_timer;
            f32 late_jump_timer;

            f32 gravity;
            f32 moon_time;

            b32 was_supported;
            Entity* support;
            v2 support_dp;
            v2  retained_support_dp;
            f32 retained_support_dp_timer;
            v2 support_normal;

            v2 ballistic_dp;
            v2 contact_move;

            f32 walk_cycle;
        };

        struct /* Wall */ {
            WallBehaviour wall_behaviour;

            u32 midi_note;
            SoundtrackID listening_to;
            f32 movement_speed_ms;

            v2 end_p;

            v2 start_p;
            b32 moving_to_end;
            f32 movement_t;
        };

        struct /* SoundtrackPlayer */ {
            SoundtrackID soundtrack_id;
            u32 playback_flags;

            v2 audible_zone;
            f32 horz_fade_region;
            f32 vert_fade_region;
            b32 can_be_heard_by_player;

            PlayingSound* playing;
        };

        struct /* CameraZone */ {
            b32 primary_camera_zone;
            v2 active_region;
            f32 view_region_height;
            v2 camera_rotation_arm;
        };

        struct /* Checkpoint */ {
            v2 checkpoint_zone;
            v2 respawn_p;
        };

        struct /* Trigger Zone */ {
            TriggerBehaviour trigger_touch_behaviour;
            TriggerBehaviour trigger_envelop_behaviour;
            TriggerBehaviour trigger_leave_behaviour;
            v2 trigger_zone;

            b32 touching_player;
            b32 enveloping_player;
        };
    };
};

inline b32 is_in_entity_local_region(Entity* entity, v2 region, v2 p) {
    b32 result = is_in_region(region, p - entity->p);
    return result;
}

#endif /* PULSAR_ENTITY_H */
