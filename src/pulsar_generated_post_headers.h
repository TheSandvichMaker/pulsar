#ifndef PULSAR_GENERATED_POST_HEADERS_H
#define PULSAR_GENERATED_POST_HEADERS_H

char* GetMetaTypeName(MetaType value) {
    switch (value) {
        case MetaType_AssetType: return "AssetType";
        case MetaType_PackedImage: return "PackedImage";
        case MetaType_PixelFormat: return "PixelFormat";
        case MetaType_u32: return "u32";
        case MetaType_v2: return "v2";
        case MetaType_PackedSound: return "PackedSound";
        case MetaType_PackedFont: return "PackedFont";
        case MetaType_f32: return "f32";
        case MetaType_PackedMidi: return "PackedMidi";
        case MetaType_u16: return "u16";
        case MetaType_PackedSoundtrack: return "PackedSoundtrack";
        case MetaType_SoundID: return "SoundID";
        case MetaType_MidiEvent: return "MidiEvent";
        case MetaType_u8: return "u8";
        case MetaType_UndoType: return "UndoType";
        case MetaType_EditorWidgetType: return "EditorWidgetType";
        case MetaType_EntityPrefab: return "EntityPrefab";
        case MetaType_EntityFlag: return "EntityFlag";
        case MetaType_EntityType: return "EntityType";
        case MetaType_WallBehaviour: return "WallBehaviour";
        case MetaType_TriggerBehaviour: return "TriggerBehaviour";
        case MetaType_GameMode: return "GameMode";
        case MetaType_GameConfig: return "GameConfig";
        case MetaType_b32: return "b32";
        case MetaType_String: return "String";
        case MetaType_DummyIntrospectStruct: return "DummyIntrospectStruct";
        case MetaType_s8: return "s8";
        case MetaType_s16: return "s16";
        case MetaType_s32: return "s32";
        case MetaType_s64: return "s64";
        case MetaType_u64: return "u64";
        case MetaType_f64: return "f64";
        case MetaType_char: return "char";
        default: return 0;
    }
}

char* GetEnumNameOf_AssetType(int value) {
    switch (value) {
        case AssetType_Unknown: return "AssetType_Unknown";
        case AssetType_Image: return "AssetType_Image";
        case AssetType_Sound: return "AssetType_Sound";
        case AssetType_Font: return "AssetType_Font";
        case AssetType_Midi: return "AssetType_Midi";
        case AssetType_Soundtrack: return "AssetType_Soundtrack";
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

char* GetEnumNameOf_EditorWidgetType(int value) {
    switch (value) {
        case Widget_None: return "Widget_None";
        case Widget_Stateless: return "Widget_Stateless";
        case Widget_ManipulateEditable: return "Widget_ManipulateEditable";
        case Widget_DragRegion: return "Widget_DragRegion";
        case Widget_DragP: return "Widget_DragP";
        default: return 0;
    }
}

char* GetEnumNameOf_EntityPrefab(int value) {
    switch (value) {
        case EntityPrefab_Null: return "EntityPrefab_Null";
        case EntityPrefab_Hazard: return "EntityPrefab_Hazard";
        case EntityPrefab_InvisibleHazard: return "EntityPrefab_InvisibleHazard";
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
        case EntityType_TriggerZone: return "EntityType_TriggerZone";
        case EntityType_Count: return "EntityType_Count";
        default: return 0;
    }
}

char* GetEnumNameOf_WallBehaviour(int value) {
    switch (value) {
        case WallBehaviour_None: return "WallBehaviour_None";
        case WallBehaviour_Move: return "WallBehaviour_Move";
        case WallBehaviour_Toggle: return "WallBehaviour_Toggle";
        case WallBehaviour_Count: return "WallBehaviour_Count";
        default: return 0;
    }
}

char* GetEnumNameOf_TriggerBehaviour(int value) {
    switch (value) {
        case TriggerBehaviour_None: return "TriggerBehaviour_None";
        case TriggerBehaviour_EndLevel: return "TriggerBehaviour_EndLevel";
        case TriggerBehaviour_Count: return "TriggerBehaviour_Count";
        default: return 0;
    }
}

char* GetEnumNameOf_GameMode(int value) {
    switch (value) {
        case GameMode_Menu: return "GameMode_Menu";
        case GameMode_Ingame: return "GameMode_Ingame";
        case GameMode_Editor: return "GameMode_Editor";
        case GameMode_Count: return "GameMode_Count";
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
    { 0, MetaType_f32, 16, "beats_per_minute", (unsigned int)&((PackedMidi*)0)->beats_per_minute, sizeof(f32) },
    { 0, MetaType_u16, 24, "time_signature_numerator", (unsigned int)&((PackedMidi*)0)->time_signature_numerator, sizeof(u16) },
    { 0, MetaType_u16, 26, "time_signature_denominator", (unsigned int)&((PackedMidi*)0)->time_signature_denominator, sizeof(u16) },
    { 0, MetaType_u32, 11, "event_count", (unsigned int)&((PackedMidi*)0)->event_count, sizeof(u32) },
    { 0, MetaType_u32, 5, "flags", (unsigned int)&((PackedMidi*)0)->flags, sizeof(u32) },
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

static MemberDefinition MembersOf_GameConfig[] = {
    { 0, MetaType_b32, 16, "start_fullscreen", (unsigned int)&((GameConfig*)0)->start_fullscreen, sizeof(b32) },
    { 0, MetaType_String, 13, "startup_level", (unsigned int)&((GameConfig*)0)->startup_level, sizeof(String) },
    { 0, MetaType_u32, 22, "command_buffer_size_mb", (unsigned int)&((GameConfig*)0)->command_buffer_size_mb, sizeof(u32) },
    { 0, MetaType_u32, 25, "permanent_storage_size_mb", (unsigned int)&((GameConfig*)0)->permanent_storage_size_mb, sizeof(u32) },
    { 0, MetaType_u32, 25, "transient_storage_size_mb", (unsigned int)&((GameConfig*)0)->transient_storage_size_mb, sizeof(u32) },
    { 0, MetaType_u32, 10, "msaa_count", (unsigned int)&((GameConfig*)0)->msaa_count, sizeof(u32) },
    { 0, MetaType_f32, 13, "master_volume", (unsigned int)&((GameConfig*)0)->master_volume, sizeof(f32) },
    { 0, MetaType_f32, 15, "gameplay_volume", (unsigned int)&((GameConfig*)0)->gameplay_volume, sizeof(f32) },
    { 0, MetaType_f32, 9, "ui_volume", (unsigned int)&((GameConfig*)0)->ui_volume, sizeof(f32) },
    { 0, MetaType_f32, 22, "directsound_latency_ms", (unsigned int)&((GameConfig*)0)->directsound_latency_ms, sizeof(f32) },
    { 0, MetaType_u32, 25, "directsound_safety_frames", (unsigned int)&((GameConfig*)0)->directsound_safety_frames, sizeof(u32) },
    { 0, MetaType_u8, 2, "up", (unsigned int)&((GameConfig*)0)->up, sizeof(u8) },
    { 0, MetaType_u8, 4, "left", (unsigned int)&((GameConfig*)0)->left, sizeof(u8) },
    { 0, MetaType_u8, 4, "down", (unsigned int)&((GameConfig*)0)->down, sizeof(u8) },
    { 0, MetaType_u8, 5, "right", (unsigned int)&((GameConfig*)0)->right, sizeof(u8) },
    { 0, MetaType_u8, 4, "jump", (unsigned int)&((GameConfig*)0)->jump, sizeof(u8) },
    { 0, MetaType_u8, 8, "interact", (unsigned int)&((GameConfig*)0)->interact, sizeof(u8) },
    { 0, MetaType_u8, 12, "alternate_up", (unsigned int)&((GameConfig*)0)->alternate_up, sizeof(u8) },
    { 0, MetaType_u8, 14, "alternate_left", (unsigned int)&((GameConfig*)0)->alternate_left, sizeof(u8) },
    { 0, MetaType_u8, 14, "alternate_down", (unsigned int)&((GameConfig*)0)->alternate_down, sizeof(u8) },
    { 0, MetaType_u8, 15, "alternate_right", (unsigned int)&((GameConfig*)0)->alternate_right, sizeof(u8) },
    { 0, MetaType_u8, 14, "alternate_jump", (unsigned int)&((GameConfig*)0)->alternate_jump, sizeof(u8) },
    { 0, MetaType_u8, 18, "alternate_interact", (unsigned int)&((GameConfig*)0)->alternate_interact, sizeof(u8) },
    { 0, MetaType_u32, 24, "max_collision_iterations", (unsigned int)&((GameConfig*)0)->max_collision_iterations, sizeof(u32) },
    { 0, MetaType_f32, 7, "gravity", (unsigned int)&((GameConfig*)0)->gravity, sizeof(f32) },
    { 0, MetaType_f32, 27, "downward_gravity_multiplier", (unsigned int)&((GameConfig*)0)->downward_gravity_multiplier, sizeof(f32) },
    { 0, MetaType_f32, 18, "moon_time_duration", (unsigned int)&((GameConfig*)0)->moon_time_duration, sizeof(f32) },
    { 0, MetaType_f32, 27, "death_by_crushing_threshold", (unsigned int)&((GameConfig*)0)->death_by_crushing_threshold, sizeof(f32) },
    { 0, MetaType_f32, 25, "support_dp_retention_time", (unsigned int)&((GameConfig*)0)->support_dp_retention_time, sizeof(f32) },
    { 0, MetaType_f32, 14, "movement_speed", (unsigned int)&((GameConfig*)0)->movement_speed, sizeof(f32) },
    { 0, MetaType_f32, 9, "max_x_vel", (unsigned int)&((GameConfig*)0)->max_x_vel, sizeof(f32) },
    { 0, MetaType_f32, 9, "min_y_vel", (unsigned int)&((GameConfig*)0)->min_y_vel, sizeof(f32) },
    { 0, MetaType_f32, 9, "max_y_vel", (unsigned int)&((GameConfig*)0)->max_y_vel, sizeof(f32) },
    { 0, MetaType_f32, 19, "max_ballistic_x_vel", (unsigned int)&((GameConfig*)0)->max_ballistic_x_vel, sizeof(f32) },
    { 0, MetaType_f32, 19, "max_ballistic_y_vel", (unsigned int)&((GameConfig*)0)->max_ballistic_y_vel, sizeof(f32) },
    { 0, MetaType_f32, 10, "jump_force", (unsigned int)&((GameConfig*)0)->jump_force, sizeof(f32) },
    { 0, MetaType_f32, 17, "early_jump_window", (unsigned int)&((GameConfig*)0)->early_jump_window, sizeof(f32) },
    { 0, MetaType_f32, 16, "late_jump_window", (unsigned int)&((GameConfig*)0)->late_jump_window, sizeof(f32) },
    { 0, MetaType_f32, 20, "player_respawn_speed", (unsigned int)&((GameConfig*)0)->player_respawn_speed, sizeof(f32) },
    { 0, MetaType_f32, 24, "player_walk_cycle_length", (unsigned int)&((GameConfig*)0)->player_walk_cycle_length, sizeof(f32) },
    { 0, MetaType_f32, 17, "level_intro_speed", (unsigned int)&((GameConfig*)0)->level_intro_speed, sizeof(f32) },
    { 0, MetaType_f32, 17, "level_outro_speed", (unsigned int)&((GameConfig*)0)->level_outro_speed, sizeof(f32) },
    { 0, MetaType_f32, 26, "background_pulse_intensity", (unsigned int)&((GameConfig*)0)->background_pulse_intensity, sizeof(f32) },
    { 0, MetaType_f32, 38, "background_pulse_world_shake_intensity", (unsigned int)&((GameConfig*)0)->background_pulse_world_shake_intensity, sizeof(f32) },
    { 0, MetaType_f32, 29, "background_pulse_spring_force", (unsigned int)&((GameConfig*)0)->background_pulse_spring_force, sizeof(f32) },
    { 0, MetaType_f32, 30, "background_pulse_spring_dampen", (unsigned int)&((GameConfig*)0)->background_pulse_spring_dampen, sizeof(f32) },
    { 0, MetaType_f32, 23, "camera_transition_speed", (unsigned int)&((GameConfig*)0)->camera_transition_speed, sizeof(f32) },
    { 0, MetaType_f32, 22, "camera_stop_smoothness", (unsigned int)&((GameConfig*)0)->camera_stop_smoothness, sizeof(f32) },
    { 0, MetaType_f32, 14, "menu_bob_speed", (unsigned int)&((GameConfig*)0)->menu_bob_speed, sizeof(f32) },
    { 0, MetaType_f32, 18, "menu_bob_magnitude", (unsigned int)&((GameConfig*)0)->menu_bob_magnitude, sizeof(f32) },
    { 0, MetaType_f32, 18, "menu_fade_in_speed", (unsigned int)&((GameConfig*)0)->menu_fade_in_speed, sizeof(f32) },
    { 0, MetaType_f32, 15, "menu_quit_speed", (unsigned int)&((GameConfig*)0)->menu_quit_speed, sizeof(f32) },
    { 0, MetaType_f32, 15, "simulation_rate", (unsigned int)&((GameConfig*)0)->simulation_rate, sizeof(f32) },
    { 0, MetaType_f32, 18, "console_open_speed", (unsigned int)&((GameConfig*)0)->console_open_speed, sizeof(f32) },
    { 0, MetaType_f32, 19, "console_close_speed", (unsigned int)&((GameConfig*)0)->console_close_speed, sizeof(f32) },
};

static MemberDefinition MembersOf_DummyIntrospectStruct[] = {
    { 0, MetaType_b32, 9, "dummy_b32", (unsigned int)&((DummyIntrospectStruct*)0)->dummy_b32, sizeof(b32) },
    { 0, MetaType_s8, 8, "dummy_s8", (unsigned int)&((DummyIntrospectStruct*)0)->dummy_s8, sizeof(s8) },
    { 0, MetaType_s16, 9, "dummy_s16", (unsigned int)&((DummyIntrospectStruct*)0)->dummy_s16, sizeof(s16) },
    { 0, MetaType_s32, 9, "dummy_s32", (unsigned int)&((DummyIntrospectStruct*)0)->dummy_s32, sizeof(s32) },
    { 0, MetaType_s64, 9, "dummy_s64", (unsigned int)&((DummyIntrospectStruct*)0)->dummy_s64, sizeof(s64) },
    { 0, MetaType_u8, 8, "dummy_u8", (unsigned int)&((DummyIntrospectStruct*)0)->dummy_u8, sizeof(u8) },
    { 0, MetaType_u16, 9, "dummy_u16", (unsigned int)&((DummyIntrospectStruct*)0)->dummy_u16, sizeof(u16) },
    { 0, MetaType_u32, 9, "dummy_u32", (unsigned int)&((DummyIntrospectStruct*)0)->dummy_u32, sizeof(u32) },
    { 0, MetaType_u64, 9, "dummy_u64", (unsigned int)&((DummyIntrospectStruct*)0)->dummy_u64, sizeof(u64) },
    { 0, MetaType_f32, 9, "dummy_f32", (unsigned int)&((DummyIntrospectStruct*)0)->dummy_f32, sizeof(f32) },
    { 0, MetaType_f64, 9, "dummy_f64", (unsigned int)&((DummyIntrospectStruct*)0)->dummy_f64, sizeof(f64) },
    { 0, MetaType_char, 10, "dummy_char", (unsigned int)&((DummyIntrospectStruct*)0)->dummy_char, sizeof(char) },
    { 0, MetaType_String, 12, "dummy_string", (unsigned int)&((DummyIntrospectStruct*)0)->dummy_string, sizeof(String) },
};

#endif
