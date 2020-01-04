#ifndef PULSAR_ENTITY_H
#define PULSAR_ENTITY_H

enum EntityFlag {
    EntityFlag_Physical = 0x1,
    EntityFlag_Collides = 0x2,
    EntityFlag_OnGround = 0x4,
    EntityFlag_Invisible = 0x8,
};

enum EntityType {
    EntityType_Null,

    EntityType_Player,
    EntityType_Wall,
    EntityType_SoundtrackPlayer,
    EntityType_CameraZone,

    EntityType_Count,
};

#define enum_to_string(enum_name) case enum_name: { return #enum_name; }
inline char* entity_type_name(EntityType type) {
    switch (type) {
        enum_to_string(EntityType_Null);
        enum_to_string(EntityType_Player);
        enum_to_string(EntityType_Wall);
        enum_to_string(EntityType_SoundtrackPlayer);
        enum_to_string(EntityType_CameraZone);
        enum_to_string(EntityType_Count);
    }
    return "Unknown EntityType";
}

struct EntityID { u32 value; };

struct Entity {
    EntityID id;

    EntityType type;
    v2 p;
    v2 dp;
    v2 ddp;

    f32 sim_dt;

    // @Note: Player
    f32 off_ground_timer;
    f32 friction_of_last_touched_surface;

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
    Rect2 camera_zone;
    v2 camera_rotation_arm;

    u32 flags;

    Image* sprite;
    v4 color;

    Shape2D collision;
};

#endif /* PULSAR_ENTITY_H */
