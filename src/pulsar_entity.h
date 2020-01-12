#ifndef PULSAR_ENTITY_H
#define PULSAR_ENTITY_H

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
    EntityType_Player,
    EntityType_Wall,

    // @Note: Non-physical Entities
    EntityType_SoundtrackPlayer,
    EntityType_CameraZone,
    EntityType_Checkpoint,

    EntityType_Count,
};

struct EntityID { u32 value; };

struct CollisionVolume {
    v2 dim;
    v2 offset;
};

introspect() struct Entity {
    EntityID guid;

    EntityType type;
    v2 p;
    v2 dp;
    v2 ddp;

    b32 dead;

    u32 flags;

    ImageID sprite;
    v4 color;

    AxisAlignedBox2 collision;
    // @TODO: Switch to collision volumes
    // CollisionVolume collision;

    // @Note: Player
    f32 off_ground_timer;
    f32 friction_of_last_touched_surface;
    Entity* support;
    v2 support_normal;
    v2 local_p;

    // @Note: Wall
    Entity* sticking_entity;
    v2 sticking_dp;

    b32 was_on_ground;
    f32 surface_friction;

    f32 movement_t;

    u32 midi_note;
    v2 midi_test_target;

    // @Note: Soundtrack Player
    b32 soundtrack_has_been_played;
    SoundtrackID soundtrack_id;
    u32 playback_flags;

    // @Note: Camera Zone;
    v2 camera_zone;
    v2 camera_rotation_arm;

    // @Note: Checkpoint
    v2 checkpoint_zone;
    v2 most_recent_player_position;
};

#endif /* PULSAR_ENTITY_H */
