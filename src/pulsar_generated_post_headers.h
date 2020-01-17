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

char* GetEnumNameOf_PlatformKeyCode(int value) {
    switch (value) {
        case PKC_LButton: return "PKC_LButton";
        case PKC_RButton: return "PKC_RButton";
        case PKC_Cancel: return "PKC_Cancel";
        case PKC_MButton: return "PKC_MButton";
        case PKC_XButton1: return "PKC_XButton1";
        case PKC_XButton2: return "PKC_XButton2";
        case PKC_Back: return "PKC_Back";
        case PKC_Tab: return "PKC_Tab";
        case PKC_Clear: return "PKC_Clear";
        case PKC_Return: return "PKC_Return";
        case PKC_Shift: return "PKC_Shift";
        case PKC_Control: return "PKC_Control";
        case PKC_Alt: return "PKC_Alt";
        case PKC_Pause: return "PKC_Pause";
        case PKC_CapsLock: return "PKC_CapsLock";
        case PKC_Kana: return "PKC_Kana";
        case PKC_Junja: return "PKC_Junja";
        case PKC_Final: return "PKC_Final";
        case PKC_Kanji: return "PKC_Kanji";
        case PKC_Escape: return "PKC_Escape";
        case PKC_Convert: return "PKC_Convert";
        case PKC_NonConvert: return "PKC_NonConvert";
        case PKC_Accept: return "PKC_Accept";
        case PKC_ModeChange: return "PKC_ModeChange";
        case PKC_Space: return "PKC_Space";
        case PKC_PageUp: return "PKC_PageUp";
        case PKC_PageDown: return "PKC_PageDown";
        case PKC_End: return "PKC_End";
        case PKC_Home: return "PKC_Home";
        case PKC_Left: return "PKC_Left";
        case PKC_Up: return "PKC_Up";
        case PKC_Right: return "PKC_Right";
        case PKC_Down: return "PKC_Down";
        case PKC_Select: return "PKC_Select";
        case PKC_Print: return "PKC_Print";
        case PKC_Execute: return "PKC_Execute";
        case PKC_PrintScreen: return "PKC_PrintScreen";
        case PKC_Insert: return "PKC_Insert";
        case PKC_Delete: return "PKC_Delete";
        case PKC_Help: return "PKC_Help";
        case PKC_LSys: return "PKC_LSys";
        case PKC_RSys: return "PKC_RSys";
        case PKC_Apps: return "PKC_Apps";
        case PKC_Sleep: return "PKC_Sleep";
        case PKC_Numpad0: return "PKC_Numpad0";
        case PKC_Numpad1: return "PKC_Numpad1";
        case PKC_Numpad2: return "PKC_Numpad2";
        case PKC_Numpad3: return "PKC_Numpad3";
        case PKC_Numpad4: return "PKC_Numpad4";
        case PKC_Numpad5: return "PKC_Numpad5";
        case PKC_Numpad6: return "PKC_Numpad6";
        case PKC_Numpad7: return "PKC_Numpad7";
        case PKC_Numpad8: return "PKC_Numpad8";
        case PKC_Numpad9: return "PKC_Numpad9";
        case PKC_Multiply: return "PKC_Multiply";
        case PKC_Add: return "PKC_Add";
        case PKC_Separator: return "PKC_Separator";
        case PKC_Subtract: return "PKC_Subtract";
        case PKC_Decimal: return "PKC_Decimal";
        case PKC_Divide: return "PKC_Divide";
        case PKC_F1: return "PKC_F1";
        case PKC_F2: return "PKC_F2";
        case PKC_F3: return "PKC_F3";
        case PKC_F4: return "PKC_F4";
        case PKC_F5: return "PKC_F5";
        case PKC_F6: return "PKC_F6";
        case PKC_F7: return "PKC_F7";
        case PKC_F8: return "PKC_F8";
        case PKC_F9: return "PKC_F9";
        case PKC_F10: return "PKC_F10";
        case PKC_F11: return "PKC_F11";
        case PKC_F12: return "PKC_F12";
        case PKC_F13: return "PKC_F13";
        case PKC_F14: return "PKC_F14";
        case PKC_F15: return "PKC_F15";
        case PKC_F16: return "PKC_F16";
        case PKC_F17: return "PKC_F17";
        case PKC_F18: return "PKC_F18";
        case PKC_F19: return "PKC_F19";
        case PKC_F20: return "PKC_F20";
        case PKC_F21: return "PKC_F21";
        case PKC_F22: return "PKC_F22";
        case PKC_F23: return "PKC_F23";
        case PKC_F24: return "PKC_F24";
        case PKC_Numlock: return "PKC_Numlock";
        case PKC_Scroll: return "PKC_Scroll";
        case PKC_LShift: return "PKC_LShift";
        case PKC_RShift: return "PKC_RShift";
        case PKC_LControl: return "PKC_LControl";
        case PKC_RControl: return "PKC_RControl";
        case PKC_LAlt: return "PKC_LAlt";
        case PKC_RAlt: return "PKC_RAlt";
        case PKC_VolumeMute: return "PKC_VolumeMute";
        case PKC_VolumeDown: return "PKC_VolumeDown";
        case PKC_VolumeUp: return "PKC_VolumeUp";
        case PKC_MediaNextTrack: return "PKC_MediaNextTrack";
        case PKC_MediaPrevTrack: return "PKC_MediaPrevTrack";
        case PKC_Oem1: return "PKC_Oem1";
        case PKC_Plus: return "PKC_Plus";
        case PKC_Comma: return "PKC_Comma";
        case PKC_Minus: return "PKC_Minus";
        case PKC_Period: return "PKC_Period";
        case PKC_Oem2: return "PKC_Oem2";
        case PKC_Oem3: return "PKC_Oem3";
        case PKC_Play: return "PKC_Play";
        case PKC_Zoom: return "PKC_Zoom";
        case PKC_OemClear: return "PKC_OemClear";
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

#endif
