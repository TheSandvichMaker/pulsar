#ifndef PULSAR_GENERATED_POST_HEADERS_H
#define PULSAR_GENERATED_POST_HEADERS_H

char* GetEnumNameOf_EditorWidgetType(int value) {
    switch (value) {
        case Widget_None: return "Widget_None";
        case Widget_DragEditable: return "Widget_DragEditable";
        case Widget_ManipulateEntity: return "Widget_ManipulateEntity";
        case Widget_DragAxisAlignedBox: return "Widget_DragAxisAlignedBox";
        case Widget_DragV2: return "Widget_DragV2";
        default: return 0;
    }
}

char* GetEnumNameOf_UndoType(int value) {
    switch (value) {
        case Undo_Null: return "Undo_Null";
        case Undo_SetData: return "Undo_SetData";
        case Undo_SetEntityData: return "Undo_SetEntityData";
        case Undo_CreateEntity: return "Undo_CreateEntity";
        case Undo_DeleteEntity: return "Undo_DeleteEntity";
        default: return 0;
    }
}

int GetNextEnumFlagNameOf_EntityFlag(unsigned int* value, char** name) {
    if (*value & EntityFlag_Physical) { *name = "EntityFlag_Physical"; *value &= ~EntityFlag_Physical; return 1; }
    else if (*value & EntityFlag_Collides) { *name = "EntityFlag_Collides"; *value &= ~EntityFlag_Collides; return 1; }
    else if (*value & EntityFlag_OnGround) { *name = "EntityFlag_OnGround"; *value &= ~EntityFlag_OnGround; return 1; }
    else if (*value & EntityFlag_Invisible) { *name = "EntityFlag_Invisible"; *value &= ~EntityFlag_Invisible; return 1; }
    else if (*value & EntityFlag_Hazard) { *name = "EntityFlag_Hazard"; *value &= ~EntityFlag_Hazard; return 1; }
    else { *name = 0; return 0; }
}

char* GetEnumNameOf_EntityType(int value) {
    switch (value) {
        case EntityType_Null: return "EntityType_Null";
        case EntityType_Player: return "EntityType_Player";
        case EntityType_Wall: return "EntityType_Wall";
        case EntityType_SoundtrackPlayer: return "EntityType_SoundtrackPlayer";
        case EntityType_CameraZone: return "EntityType_CameraZone";
        case EntityType_Checkpoint: return "EntityType_Checkpoint";
        case EntityType_Count: return "EntityType_Count";
        default: return 0;
    }
}

static MemberDefinition MembersOf_PackedImage[] = {
    { 0, MetaType_PixelFormat, 12, "pixel_format", (unsigned int)&((PackedImage*)0)->pixel_format, sizeof(PixelFormat) },
    { 0, MetaType_u32, 1, "w", (unsigned int)&((PackedImage*)0)->w, sizeof(u32) },
    { 0, MetaType_u32, 1, "h", (unsigned int)&((PackedImage*)0)->h, sizeof(u32) },
    { 0, MetaType_v2, 5, "align", (unsigned int)&((PackedImage*)0)->align, sizeof(v2) },
    { 0, MetaType_v2, 5, "scale", (unsigned int)&((PackedImage*)0)->scale, sizeof(v2) },
};

static MemberDefinition MembersOf_PackedSound[] = {
    { 0, MetaType_u32, 13, "channel_count", (unsigned int)&((PackedSound*)0)->channel_count, sizeof(u32) },
    { 0, MetaType_u32, 12, "sample_count", (unsigned int)&((PackedSound*)0)->sample_count, sizeof(u32) },
};

static MemberDefinition MembersOf_PackedFont[] = {
    { 0, MetaType_u32, 15, "first_codepoint", (unsigned int)&((PackedFont*)0)->first_codepoint, sizeof(u32) },
    { 0, MetaType_u32, 23, "one_past_last_codepoint", (unsigned int)&((PackedFont*)0)->one_past_last_codepoint, sizeof(u32) },
    { 0, MetaType_u32, 4, "size", (unsigned int)&((PackedFont*)0)->size, sizeof(u32) },
    { 0, MetaType_u32, 17, "oversample_amount", (unsigned int)&((PackedFont*)0)->oversample_amount, sizeof(u32) },
    { 0, MetaType_f32, 16, "whitespace_width", (unsigned int)&((PackedFont*)0)->whitespace_width, sizeof(f32) },
    { 0, MetaType_f32, 6, "ascent", (unsigned int)&((PackedFont*)0)->ascent, sizeof(f32) },
    { 0, MetaType_f32, 7, "descent", (unsigned int)&((PackedFont*)0)->descent, sizeof(f32) },
    { 0, MetaType_f32, 8, "line_gap", (unsigned int)&((PackedFont*)0)->line_gap, sizeof(f32) },
};

static MemberDefinition MembersOf_PackedMidi[] = {
    { 0, MetaType_u32, 16, "ticks_per_second", (unsigned int)&((PackedMidi*)0)->ticks_per_second, sizeof(u32) },
    { 0, MetaType_u32, 16, "beats_per_minute", (unsigned int)&((PackedMidi*)0)->beats_per_minute, sizeof(u32) },
    { 0, MetaType_u16, 24, "time_signature_numerator", (unsigned int)&((PackedMidi*)0)->time_signature_numerator, sizeof(u16) },
    { 0, MetaType_u16, 26, "time_signature_denominator", (unsigned int)&((PackedMidi*)0)->time_signature_denominator, sizeof(u16) },
    { 0, MetaType_u32, 11, "event_count", (unsigned int)&((PackedMidi*)0)->event_count, sizeof(u32) },
};

static MemberDefinition MembersOf_PackedSoundtrack[] = {
    { 0, MetaType_SoundID, 5, "sound", (unsigned int)&((PackedSoundtrack*)0)->sound, sizeof(SoundID) },
    { 0, MetaType_u32, 16, "midi_track_count", (unsigned int)&((PackedSoundtrack*)0)->midi_track_count, sizeof(u32) },
};

static MemberDefinition MembersOf_MidiEvent[] = {
    { 0, MetaType_u32, 22, "absolute_time_in_ticks", (unsigned int)&((MidiEvent*)0)->absolute_time_in_ticks, sizeof(u32) },
    { 0, MetaType_u8, 7, "channel", (unsigned int)&((MidiEvent*)0)->channel, sizeof(u8) },
    { 0, MetaType_u8, 4, "type", (unsigned int)&((MidiEvent*)0)->type, sizeof(u8) },
    { 0, MetaType_u8, 10, "note_value", (unsigned int)&((MidiEvent*)0)->note_value, sizeof(u8) },
    { 0, MetaType_u8, 8, "velocity", (unsigned int)&((MidiEvent*)0)->velocity, sizeof(u8) },
};

static MemberDefinition MembersOf_EditorAssets[] = {
    { 0, MetaType_ImageID, 11, "camera_icon", (unsigned int)&((EditorAssets*)0)->camera_icon, sizeof(ImageID) },
    { 0, MetaType_ImageID, 12, "speaker_icon", (unsigned int)&((EditorAssets*)0)->speaker_icon, sizeof(ImageID) },
    { 0, MetaType_ImageID, 15, "checkpoint_icon", (unsigned int)&((EditorAssets*)0)->checkpoint_icon, sizeof(ImageID) },
};

static MemberDefinition MembersOf_Entity[] = {
    { 0, MetaType_EntityID, 4, "guid", (unsigned int)&((Entity*)0)->guid, sizeof(EntityID) },
    { 0, MetaType_EntityType, 4, "type", (unsigned int)&((Entity*)0)->type, sizeof(EntityType) },
    { 0, MetaType_u32, 5, "flags", (unsigned int)&((Entity*)0)->flags, sizeof(u32) },
    { 0, MetaType_b32, 4, "dead", (unsigned int)&((Entity*)0)->dead, sizeof(b32) },
    { 0, MetaType_v2, 1, "p", (unsigned int)&((Entity*)0)->p, sizeof(v2) },
    { 0, MetaType_v2, 2, "dp", (unsigned int)&((Entity*)0)->dp, sizeof(v2) },
    { 0, MetaType_v2, 3, "ddp", (unsigned int)&((Entity*)0)->ddp, sizeof(v2) },
    { 0, MetaType_AxisAlignedBox2, 9, "collision", (unsigned int)&((Entity*)0)->collision, sizeof(AxisAlignedBox2) },
    { 0, MetaType_ImageID, 6, "sprite", (unsigned int)&((Entity*)0)->sprite, sizeof(ImageID) },
    { 0, MetaType_v4, 5, "color", (unsigned int)&((Entity*)0)->color, sizeof(v4) },
    { 0, MetaType_f32, 16, "off_ground_timer", (unsigned int)&((Entity*)0)->off_ground_timer, sizeof(f32) },
    { 0, MetaType_f32, 32, "friction_of_last_touched_surface", (unsigned int)&((Entity*)0)->friction_of_last_touched_surface, sizeof(f32) },
    { MetaMemberFlag_IsPointer, MetaType_Entity, 7, "support", (unsigned int)&((Entity*)0)->support, sizeof(Entity) },
    { 0, MetaType_v2, 14, "support_normal", (unsigned int)&((Entity*)0)->support_normal, sizeof(v2) },
    { 0, MetaType_v2, 7, "local_p", (unsigned int)&((Entity*)0)->local_p, sizeof(v2) },
    { 0, MetaType_f32, 16, "surface_friction", (unsigned int)&((Entity*)0)->surface_friction, sizeof(f32) },
    { 0, MetaType_u32, 9, "midi_note", (unsigned int)&((Entity*)0)->midi_note, sizeof(u32) },
    { 0, MetaType_f32, 10, "movement_t", (unsigned int)&((Entity*)0)->movement_t, sizeof(f32) },
    { MetaMemberFlag_IsPointer, MetaType_Entity, 15, "sticking_entity", (unsigned int)&((Entity*)0)->sticking_entity, sizeof(Entity) },
    { 0, MetaType_v2, 11, "sticking_dp", (unsigned int)&((Entity*)0)->sticking_dp, sizeof(v2) },
    { 0, MetaType_v2, 16, "midi_test_target", (unsigned int)&((Entity*)0)->midi_test_target, sizeof(v2) },
    { 0, MetaType_SoundtrackID, 13, "soundtrack_id", (unsigned int)&((Entity*)0)->soundtrack_id, sizeof(SoundtrackID) },
    { 0, MetaType_u32, 14, "playback_flags", (unsigned int)&((Entity*)0)->playback_flags, sizeof(u32) },
    { 0, MetaType_b32, 26, "soundtrack_has_been_played", (unsigned int)&((Entity*)0)->soundtrack_has_been_played, sizeof(b32) },
    { 0, MetaType_v2, 11, "camera_zone", (unsigned int)&((Entity*)0)->camera_zone, sizeof(v2) },
    { 0, MetaType_v2, 19, "camera_rotation_arm", (unsigned int)&((Entity*)0)->camera_rotation_arm, sizeof(v2) },
    { 0, MetaType_v2, 15, "checkpoint_zone", (unsigned int)&((Entity*)0)->checkpoint_zone, sizeof(v2) },
    { 0, MetaType_v2, 27, "most_recent_player_position", (unsigned int)&((Entity*)0)->most_recent_player_position, sizeof(v2) },
};

#endif
