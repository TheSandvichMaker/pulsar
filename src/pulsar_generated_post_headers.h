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

#endif
