#ifndef PULSAR_GENERATED_POST_HEADERS_H
#define PULSAR_GENERATED_POST_HEADERS_H

char* GetEnumNameOf_EditorWidgetType(int value) {
    switch (value) {
        case Widget_None: return "Widget_None";
        case Widget_DragEditable: return "Widget_DragEditable";
        case Widget_ManipulateEntity: return "Widget_ManipulateEntity";
        case Widget_DragRect: return "Widget_DragRect";
        default: return "Unknown value for EditorWidgetType";
    }
}

char* GetEnumNameOf_UndoType(int value) {
    switch (value) {
        case Undo_Null: return "Undo_Null";
        case Undo_SetData: return "Undo_SetData";
        case Undo_SetEntityData: return "Undo_SetEntityData";
        case Undo_CreateEntity: return "Undo_CreateEntity";
        case Undo_DeleteEntity: return "Undo_DeleteEntity";
        default: return "Unknown value for UndoType";
    }
}

int GetNextEnumFlagNameOf_EntityFlag(unsigned int* value, char** name) {
    if (*value & EntityFlag_Physical) { *name = "EntityFlag_Physical"; *value &= ~EntityFlag_Physical; return 1; }
    else if (*value & EntityFlag_Collides) { *name = "EntityFlag_Collides"; *value &= ~EntityFlag_Collides; return 1; }
    else if (*value & EntityFlag_OnGround) { *name = "EntityFlag_OnGround"; *value &= ~EntityFlag_OnGround; return 1; }
    else if (*value & EntityFlag_Invisible) { *name = "EntityFlag_Invisible"; *value &= ~EntityFlag_Invisible; return 1; }
    else { *name = 0; return 0; }
}

char* GetEnumNameOf_EntityType(int value) {
    switch (value) {
        case EntityType_Null: return "EntityType_Null";
        case EntityType_Player: return "EntityType_Player";
        case EntityType_Wall: return "EntityType_Wall";
        case EntityType_SoundtrackPlayer: return "EntityType_SoundtrackPlayer";
        case EntityType_CameraZone: return "EntityType_CameraZone";
        case EntityType_Count: return "EntityType_Count";
        default: return "Unknown value for EntityType";
    }
}

static MemberDefinition MembersOf_PackedImage[] = {
    { 0, MetaType_PixelFormat, "pixel_format", (u32)&((PackedImage*)0)->pixel_format },
    { 0, MetaType_u32, "w", (u32)&((PackedImage*)0)->w },
    { 0, MetaType_u32, "h", (u32)&((PackedImage*)0)->h },
    { 0, MetaType_v2, "align", (u32)&((PackedImage*)0)->align },
    { 0, MetaType_v2, "scale", (u32)&((PackedImage*)0)->scale },
};

static MemberDefinition MembersOf_PackedSound[] = {
    { 0, MetaType_u32, "channel_count", (u32)&((PackedSound*)0)->channel_count },
    { 0, MetaType_u32, "sample_count", (u32)&((PackedSound*)0)->sample_count },
};

static MemberDefinition MembersOf_PackedFont[] = {
    { 0, MetaType_u32, "first_codepoint", (u32)&((PackedFont*)0)->first_codepoint },
    { 0, MetaType_u32, "one_past_last_codepoint", (u32)&((PackedFont*)0)->one_past_last_codepoint },
    { 0, MetaType_u32, "size", (u32)&((PackedFont*)0)->size },
    { 0, MetaType_u32, "oversample_amount", (u32)&((PackedFont*)0)->oversample_amount },
    { 0, MetaType_f32, "whitespace_width", (u32)&((PackedFont*)0)->whitespace_width },
    { 0, MetaType_f32, "ascent", (u32)&((PackedFont*)0)->ascent },
    { 0, MetaType_f32, "descent", (u32)&((PackedFont*)0)->descent },
    { 0, MetaType_f32, "line_gap", (u32)&((PackedFont*)0)->line_gap },
};

static MemberDefinition MembersOf_PackedMidi[] = {
    { 0, MetaType_u32, "ticks_per_second", (u32)&((PackedMidi*)0)->ticks_per_second },
    { 0, MetaType_u32, "beats_per_minute", (u32)&((PackedMidi*)0)->beats_per_minute },
    { 0, MetaType_u16, "time_signature_numerator", (u32)&((PackedMidi*)0)->time_signature_numerator },
    { 0, MetaType_u16, "time_signature_denominator", (u32)&((PackedMidi*)0)->time_signature_denominator },
    { 0, MetaType_u32, "event_count", (u32)&((PackedMidi*)0)->event_count },
};

static MemberDefinition MembersOf_PackedSoundtrack[] = {
    { 0, MetaType_SoundID, "sound", (u32)&((PackedSoundtrack*)0)->sound },
    { 0, MetaType_u32, "midi_track_count", (u32)&((PackedSoundtrack*)0)->midi_track_count },
};

static MemberDefinition MembersOf_MidiEvent[] = {
    { 0, MetaType_u32, "absolute_time_in_ticks", (u32)&((MidiEvent*)0)->absolute_time_in_ticks },
    { 0, MetaType_u8, "channel", (u32)&((MidiEvent*)0)->channel },
    { 0, MetaType_u8, "type", (u32)&((MidiEvent*)0)->type },
    { 0, MetaType_u8, "note_value", (u32)&((MidiEvent*)0)->note_value },
    { 0, MetaType_u8, "velocity", (u32)&((MidiEvent*)0)->velocity },
};

#endif
