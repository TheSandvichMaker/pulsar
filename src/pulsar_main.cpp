#include "pulsar_main.h"
#include "pulsar_generated_post_headers.h"

#define RND_IMPLEMENTATION
#define RND_U32 u32
#define RND_U64 u64
#include "external/rnd.h"

#include "pulsar_assets.cpp"
#include "pulsar_audio_mixer.cpp"
#include "pulsar_render_commands.cpp"
#include "pulsar_gjk.cpp"
#include "pulsar_entity.cpp"
#include "pulsar_editor.cpp"
#include "pulsar_console.cpp"

// @TODO: Decouple file paths from level loading

inline Level* allocate_level(MemoryArena* arena, char* name) {
    Level* result = push_struct(arena, Level);
	
    // @Note: The 0th entity is reserved as the null entity
    result->first_available_guid = 1;
    result->entity_count = 1;
	
    size_t name_length = cstr_length(name);
    assert(name_length <= ARRAY_COUNT(result->name));
    result->name_length = cast(u32) name_length;
    copy(result->name_length, name, result->name);
	
    return result;
}

struct SerializeResult {
    union {
		u64   data_value;
        void* data_ptr;
    };
    b32 data_is_ptr;
    u32 size;
};

inline SerializeResult serialize_value(u32 size, u64 value) {
    SerializeResult result = {};
    result.data_value = value;
    result.size = size;
    return result;
}

inline SerializeResult serialize_data(u32 size, void* data) {
    SerializeResult result = {};
    result.data_ptr = data;
    result.data_is_ptr = true;
    result.size = size;
    return result;
}

#define SERIALIZE_CALLBACK(name) SerializeResult name(GameState* game_state, u32 data_size, void* data_ptr, b32 deserialize)
typedef SERIALIZE_CALLBACK(SerializeCallback);

struct Serializable {
    SerializeCallback* callback;
	
    String name;
    s32 type; // @Note: Why does the compiler want EntityType to be signed? @TODO: could use C++11 enum type specifiers if I made the code generator not barf on them
    u32 offset;
    u32 size;
};

#define SERIALIZABLE(struct_type, identifying_type, member, ...) \
    create_serializable(string_literal(#member), identifying_type, offsetof(struct_type, member), sizeof(cast(struct_type*) 0)->member, ##__VA_ARGS__)
inline Serializable create_serializable(String name, s32 type, u32 offset, u32 size, SerializeCallback* callback = 0) {
    Serializable result;
    result.callback = callback;
    result.name = name;
    result.type = type;
    result.offset = offset;
    result.size = size;
    return result;
}

internal SERIALIZE_CALLBACK(serialize_AssetID) {
   SerializeResult result = {};
   
   if (deserialize) {
       String asset_name = wrap_string(data_size, cast(char*) data_ptr);
       result = serialize_value(sizeof(AssetID), get_asset_id_by_name(&game_state->assets, asset_name).value);
   } else {
       AssetID id = *(cast(AssetID*) data_ptr);
       if (id.value) {
           Asset* asset = get_asset(&game_state->assets, id);
           if (asset) {
               result = serialize_data(cast(u32) asset->name.len, asset->name.data);
           }
       }
   }
   
   return result;
}

global Serializable entity_serializables[] = {
   SERIALIZABLE(Entity, EntityType_Any, guid),
   SERIALIZABLE(Entity, EntityType_Any, type),
   SERIALIZABLE(Entity, EntityType_Any, flags),
   SERIALIZABLE(Entity, EntityType_Any, dead),
   SERIALIZABLE(Entity, EntityType_Any, p),
   SERIALIZABLE(Entity, EntityType_Any, collision),
   SERIALIZABLE(Entity, EntityType_Any, sprite, serialize_AssetID),
   SERIALIZABLE(Entity, EntityType_Any, color),
   // @Note: Player
   // ...
   // @Note: Wall
   SERIALIZABLE(Entity, EntityType_Wall, wall_behaviour),
   SERIALIZABLE(Entity, EntityType_Wall, midi_note),
   SERIALIZABLE(Entity, EntityType_Wall, end_p),
   SERIALIZABLE(Entity, EntityType_Wall, movement_speed_ms),
   SERIALIZABLE(Entity, EntityType_Wall, listening_to, serialize_AssetID),
   // @Note: Soundtrack Player
   SERIALIZABLE(Entity, EntityType_SoundtrackPlayer, soundtrack_id, serialize_AssetID),
   SERIALIZABLE(Entity, EntityType_SoundtrackPlayer, playback_flags),
   SERIALIZABLE(Entity, EntityType_SoundtrackPlayer, audible_zone),
   SERIALIZABLE(Entity, EntityType_SoundtrackPlayer, horz_fade_region),
   SERIALIZABLE(Entity, EntityType_SoundtrackPlayer, vert_fade_region),
   // @Note: Camera Zone
   SERIALIZABLE(Entity, EntityType_CameraZone, primary_camera_zone),
   SERIALIZABLE(Entity, EntityType_CameraZone, active_region),
   SERIALIZABLE(Entity, EntityType_CameraZone, view_region_height),
   SERIALIZABLE(Entity, EntityType_CameraZone, camera_rotation_arm),
   // @Note: Checkpoint
   SERIALIZABLE(Entity, EntityType_Checkpoint, checkpoint_zone),
   // @Note: Trigger Zone
   SERIALIZABLE(Entity, EntityType_TriggerZone, trigger_touch_behaviour),
   SERIALIZABLE(Entity, EntityType_TriggerZone, trigger_envelop_behaviour),
   SERIALIZABLE(Entity, EntityType_TriggerZone, trigger_leave_behaviour),
   SERIALIZABLE(Entity, EntityType_TriggerZone, trigger_zone),
};

/* Lev file layout:
* LevFileHeader header;
* for each entity serialized:
*     u32 member_count; // number of members to follow
*     for member_count:
*         u32  member_name_length;
*         char member_name[member_name_length];
*         u32  member_data_size;
*         u8   member_data[member_data_size];
*/

#pragma pack(push, 1)
struct LevFileHeader {
   struct {
       char magic[4];
       
#define LEV_VERSION 2
       u32 version;
       
       u32 header_size;
   } prelude;
   
   u32 entity_count;
};
#pragma pack(pop)

internal void write_level_to_disk(GameState* game_state, Level* level, String level_name) {
   MemoryArena* arena = &game_state->transient_arena;
   
   TemporaryMemory temp = begin_temporary_memory(arena);
   
   LinearBuffer<u8>* stream = begin_linear_buffer<u8>(arena);
   
   // @TODO: I should have some kind of nice way to write byte streams to memory arenas
#define write_stream(size, data) { u8* buf = lb_push_n(stream, size); copy(size, data, buf); }
   
   LevFileHeader header;
   
   header.prelude.magic[0] = 'l';
   header.prelude.magic[1] = 'e';
   header.prelude.magic[2] = 'v';
   header.prelude.magic[3] = 'l';
   
   header.prelude.version = LEV_VERSION;
   header.prelude.header_size = sizeof(header);
   
   header.entity_count = level->entity_count;
   
   write_stream(sizeof(header), &header);
   
   for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
       Entity* entity = level->entities + entity_index;
       
       u32* place_of_member_count = cast(u32*) lb_push_n(stream, sizeof(u32));
       u32 member_count = 0;
       
       for (u32 ser_index = 0; ser_index < ARRAY_COUNT(entity_serializables); ser_index++) {
           Serializable* ser = entity_serializables + ser_index;
           if (ser->type == EntityType_Any || ser->type == entity->type) {
               void** data_ptr = cast(void**) (cast(u8*) entity + ser->offset);
               
               SerializeResult ser_data;
               if (ser->callback) {
                   ser_data = ser->callback(game_state, ser->size, data_ptr, false);
               } else {
                   ser_data.data_ptr = data_ptr;
                   ser_data.data_is_ptr = true;
                   ser_data.size = ser->size;
               }
               
               if (ser_data.size > 0) {
                   member_count++;
                   
                   write_stream(sizeof(u32), &ser->name.len);
                   write_stream(ser->name.len, ser->name.data);
                   
                   if (ser_data.data_is_ptr) {
                       write_stream(sizeof(u32), &ser_data.size);
                       write_stream(ser_data.size, ser_data.data_ptr);
                   } else {
                       write_stream(sizeof(u32), &ser_data.size);
                       write_stream(ser_data.size, &ser_data.data_value);
                   }
               }
           }
       }
       
       *place_of_member_count = member_count;
   }
   
#undef write_stream
   
   end_linear_buffer(stream);
   
   char* temp_level_name_cstr = push_string_and_null_terminate(arena, level_name.len, level_name.data);
   platform.write_entire_file(temp_level_name_cstr, cast(u32) stream->count, stream->data);
   
   log_print(LogLevel_Info, "Saved level '%.*s' to disk", string_expand(level_name));
   
   end_temporary_memory(temp);
}

inline Serializable* find_serializable_by_name(u32 serializable_count, Serializable* serializables, String name) {
   Serializable* result = 0;
   
   for (u32 ser_index = 0; ser_index < serializable_count; ser_index++) {
       Serializable* ser = serializables + ser_index;
       if (strings_are_equal(name, ser->name)) {
           result = ser;
           break;
       }
   }
   
   return result;
}

internal b32 load_level_from_disk(GameState* game_state, Level* level, String level_name) {
   assert(level);
   
   MemoryArena* arena = &game_state->transient_arena;
   
   b32 level_load_error = false;
   
   TemporaryMemory temp = begin_temporary_memory(arena);
   
   // @TODO: Start using String consistently to avoid this kind of sillyness
   char* temp_level_name_cstr = push_string_and_null_terminate(arena, level_name.len, level_name.data);
   EntireFile file = platform.read_entire_file(temp_level_name_cstr, allocator(arena_allocator, arena));
   if (file.size > 0) {
       u8* stream = cast(u8*) file.data;
       u8* stream_end = stream + file.size;
       
#define read_stream(size) (assert(stream + (size) <= stream_end), stream += (size), stream - (size))
       LevFileHeader* header = cast(LevFileHeader*) stream;
       assert(header->prelude.header_size <= sizeof(*header));
       
       stream += header->prelude.header_size;
       
       if (header->prelude.magic[0] == 'l' &&
           header->prelude.magic[1] == 'e' &&
           header->prelude.magic[2] == 'v' &&
           header->prelude.magic[3] == 'l'
           ) {
           if (header->prelude.version != LEV_VERSION) {
               log_print(LogLevel_Warn, "Level Load Warning: Version mismatch: %u in file, expected %u", header->prelude.version, LEV_VERSION);
           }
           
           assert(level_name.len <= ARRAY_COUNT(level->name));
           level->name_length = cast(u32) level_name.len;
           copy(level_name.len, level_name.data, level->name);
           
           level->entity_count = header->entity_count;
           level->first_available_guid = 1;
           
           for (u32 entity_index = 1; entity_index < level->entity_count; entity_index++) {
               Entity* entity = level->entities + entity_index;
               
               u32 member_count = *(cast(u32*) read_stream(sizeof(u32)));
               
               for (u32 member_index = 0; member_index < member_count; member_index++) {
                   u32 member_name_length = *(cast(u32*) read_stream(sizeof(u32)));
                   assert(member_name_length);
                   char* member_name_c = cast(char*) read_stream(member_name_length);
                   
                   String member_name = wrap_string(member_name_length, member_name_c);
                   
                   // @Note: Remap renamed entity data
                   if (header->prelude.version < 2) {
                       if (strings_are_equal(member_name, string_literal("behaviour"))) {
                           member_name = string_literal("wall_behaviour");
                       }
                   }
                   
                   u32 data_size = *(cast(u32*) read_stream(sizeof(u32)));
                   void* data_source = read_stream(data_size);
                   
                   Serializable* ser = find_serializable_by_name(ARRAY_COUNT(entity_serializables), entity_serializables, member_name);
                   if (ser) {
                       void** data_dest = cast(void**) (cast(u8*) entity + ser->offset);
                       
                       SerializeResult ser_data;
                       if (ser->callback) {
                           ser_data = ser->callback(game_state, data_size, data_source, true);
                       } else {
                           ser_data.data_ptr = data_source;
                           ser_data.data_is_ptr = true;
                           ser_data.size = data_size;
                       }
                       
                       if (ser_data.size && ser->size == ser_data.size) {
                           if (ser_data.data_is_ptr) {
                               copy(ser_data.size, ser_data.data_ptr, data_dest);
                           } else {
                               assert(ser_data.size <= sizeof(ser_data.data_value));
                               copy(ser_data.size, &ser_data.data_value, data_dest);
                           }
                           
                           if (strings_are_equal(ser->name, string_literal("guid"))) {
                               if (entity->guid.value >= level->first_available_guid) {
                                   level->first_available_guid = entity->guid.value + 1;
                               }
                           }
                       } else {
                           level_load_error = true;
                           log_print(LogLevel_Error, "Level Load Error: Member size mismatch: %u in file, expected %u", data_size, ser->size);
                           break;
                       }
                   } else {
                       log_print(LogLevel_Warn, "Level Load Warning: Could not find matching member '%.*s'", member_name_length, member_name);
                   }
               }
           }
       } else {
           level_load_error = true;
           log_print(LogLevel_Error, "Level Load Error: Header did not start with magic value 'levl'");
       }
#undef read_stream
       
       if (!level_load_error) {
           // @Note: Upgrade outdated entity data
           if (header->prelude.version == 0) {
               for (u32 entity_index = 0; entity_index < level->entity_count; entity_index++) {
                   Entity* entity = level->entities + entity_index;
                   switch (entity->type) {
                       case EntityType_Wall: {
                           entity->end_p -= entity->p; // @Note: end_p used to be absolute, is now relative
                       } break;
                   }
               }
           }
       }
   } else {
       level_load_error = true;
       log_print(LogLevel_Error, "Level Load Error: Could not open file '%.*s'", string_expand(level_name));
   }
   
   if (level_load_error) {
       level->entity_count = 1;
       level->first_available_guid = 1;
   } else {
       log_print(LogLevel_Info, "Successfully loaded level '%.*s'", string_expand(level_name));
   }
   
   end_temporary_memory(temp);
   
   return !level_load_error;
}

internal void load_level(GameState* game_state, String level_name) {
   if (load_level_from_disk(game_state, game_state->background_level, level_name)) {
       Level* temp = game_state->active_level;
       game_state->active_level = game_state->background_level;
       game_state->background_level = temp;
       
       if (game_state->editor_state) {
           load_level_into_editor(game_state->editor_state, game_state->active_level);
       }
   }
}

inline String level_name(Level* level) {
   String result = wrap_string(level->name_length, level->name);
   return result;
}

inline PlayingMidi* play_midi(GameState* game_state, MidiTrack* track, u32 flags = 0, PlayingSound* sync_sound = 0, Entity* source_soundtrack_player = 0) {
   if (!game_state->first_free_playing_midi) {
       game_state->first_free_playing_midi = push_struct(&game_state->permanent_arena, PlayingMidi);
       game_state->first_free_playing_midi->next_free = 0;
   }
   
   PlayingMidi* playing_midi = game_state->first_free_playing_midi;
   game_state->first_free_playing_midi = playing_midi->next_free;
   
   *playing_midi = {};
   playing_midi->track = track;
   playing_midi->flags = flags;
   playing_midi->source_soundtrack_player = source_soundtrack_player;
   playing_midi->sync_sound = sync_sound;
   playing_midi->sync_sound->synced_midi = playing_midi;
   playing_midi->playback_rate = 1.0f;
   
   playing_midi->next = game_state->first_playing_midi;
   game_state->first_playing_midi = playing_midi;
   
   String track_name = {};
   
   Asset* track_asset = cast(Asset*) track;
   if (track_asset->name.len) {
       track_name = track_asset->name;
   } else if (source_soundtrack_player) {
       Soundtrack* soundtrack = get_soundtrack(&game_state->assets, source_soundtrack_player->soundtrack_id);
       if (soundtrack) {
           Asset* soundtrack_asset = cast(Asset*) soundtrack;
           track_name = soundtrack_asset->name;
       }
   }
   
   log_print(LogLevel_Info, "Playing midi track %.*s with %u events", string_expand(track_name), track->event_count);
   
   return playing_midi;
}

inline void change_playback_rate(PlayingMidi* midi, f32 playback_rate) {
   midi->playback_rate = clamp(playback_rate, 0.1f, 32.0f);
}

inline PlayingSound* play_soundtrack(GameState* game_state, Entity* soundtrack_player, u32 flags) {
   PlayingSound* result = 0;
   Soundtrack* soundtrack = get_soundtrack(&game_state->assets, soundtrack_player->soundtrack_id);
   if (soundtrack) {
       Sound* sound = get_sound(&game_state->assets, soundtrack->sound);
       if (sound) {
           result = play_sound(&game_state->game_audio, sound, vec2(0.5f, 0.5f), flags);
           for (u32 midi_index = 0; midi_index < soundtrack->midi_track_count; midi_index++) {
               MidiTrack* track = get_midi(&game_state->assets, soundtrack->midi_tracks[midi_index]);
               PlayingMidi* playing_midi = play_midi(game_state, track, flags, result, soundtrack_player);
           }
       }
   }
   
   return result;
}

inline void stop_all_midi_tracks(GameState* game_state) {
   PlayingMidi* last_playing_midi = game_state->first_playing_midi;
   if (last_playing_midi) {
       while (last_playing_midi->next) {
           last_playing_midi = last_playing_midi->next;
       }
       last_playing_midi->next = game_state->first_free_playing_midi;
       game_state->first_free_playing_midi = game_state->first_playing_midi;
       game_state->first_playing_midi = 0;
   }
   
   game_state->midi_event_buffer_count = 0;
}

internal void play_level(GameState* game_state, Level* level) {
   game_state->active_level = level;
   
   TemporaryMemory temp = begin_temporary_memory(&game_state->transient_arena);
   SortEntry* sort_temp = push_array(&game_state->transient_arena, level->entity_count, SortEntry);
   
   SortEntry* sorted_entities = push_array(&game_state->transient_arena, level->entity_count, SortEntry);
   for (u32 entity_index = 0; entity_index < level->entity_count; entity_index++) {
       Entity* entity = level->entities + entity_index;
       SortEntry* entry = sorted_entities + entity_index;
       entry->sort_key = cast(f32) entity->type; // @TODO: Kind of weird to cast an enum to a float, makes me feel bad
       entry->index = entity_index;
   }
   
   radix_sort(level->entity_count, sorted_entities, sort_temp);
   
   game_state->player = 0;
   
   u32 current_type_count = 0;
   EntityType current_type = EntityType_Null;
   game_state->entity_type_offsets[current_type] = 0;
   for (u32 entity_index = 0; entity_index < level->entity_count; entity_index++) {
       SortEntry* sort_entry = sorted_entities + entity_index;
       
       Entity* source = level->entities + sort_entry->index;
       Entity* dest = game_state->entities + entity_index;
       
       *dest = *source;
       
       EntityHash* hash = get_entity_hash_slot(game_state->editor_state, dest->guid);
       if (hash) {
           hash->gamestate_index = entity_index;
       }
       
       if (dest->type != current_type) {
           game_state->entity_type_counts[current_type] = current_type_count;
           current_type_count = 0;
           current_type = dest->type;
           game_state->entity_type_offsets[current_type] = entity_index;
       }
       
       current_type_count++;
       
       if (dest->type == EntityType_Player) {
           if (!game_state->player) {
               game_state->player = dest;
           } else {
               log_print(LogLevel_Error, "Level has more than one player!");
           }
       }
   }
   game_state->entity_type_counts[current_type] = current_type_count;
   game_state->entity_count = level->entity_count;
   
   if (!game_state->entity_type_counts[EntityType_Checkpoint]) {
       // @TODO: Handle invalid levels in some robust way
       log_print(LogLevel_Error, "Level '%.*s' has no checkpoints! This is not good! Fix it!! Now!!! Unless you're not in a position to fix it, in which case I'm sorry.", string_expand(level_name(level)));
   }
   
   end_temporary_memory(temp);
   
   assert(game_state->player); // @TODO: Handle invalid levels in some robust way
   
   Entity* player = game_state->player;
   
   f32 best_checkpoint_distance_sq = FLT_MAX;
   game_state->last_activated_checkpoint = 0;
   
   f32 best_camera_zone_distance_sq = FLT_MAX;
   Entity* nearest_camera_zone = 0;
   
   game_state->active_camera_zone = 0;
   
   for (u32 entity_index = 1; entity_index < game_state->entity_count; entity_index++) {
       Entity* entity = game_state->entities + entity_index;
       
       if (entity->type == EntityType_Wall) {
           entity->start_p = entity->p;
       } else if (entity->type == EntityType_Checkpoint) {
           // @TODO: This distance should be distance from the checkpoint region, not the entity itself
           f32 distance_sq = length_sq(entity->p - player->p);
           if (distance_sq < best_checkpoint_distance_sq) {
               best_checkpoint_distance_sq = distance_sq;
               game_state->last_activated_checkpoint = entity;
               game_state->last_activated_checkpoint->respawn_p = entity->p;
           }
       } else if (entity->type == EntityType_CameraZone) {
           // @TODO: Is there really a point to primary_camera_zone?
           if (entity->primary_camera_zone) {
               if (!game_state->active_camera_zone) {
                   game_state->active_camera_zone = entity;
               } else {
                   log_print(LogLevel_Warn, "More than one Camera Zone was selected as primary. Ignoring Camera Zone %u", entity->guid.value);
               }
           }
           
           // @TODO: This distance should be distance from the active region, not the entity itself
           f32 distance_sq = length_sq(entity->p - player->p);
           if (distance_sq < best_camera_zone_distance_sq) {
               best_camera_zone_distance_sq = distance_sq;
               nearest_camera_zone = entity;
           }
       }
   }
   
   if (!game_state->active_camera_zone) {
       game_state->active_camera_zone = nearest_camera_zone;
   }
   
   game_state->level_intro_timer = 1.0f;
   game_state->background_pulse_t = 0.0f;
   game_state->background_pulse_dt = 0.0f;
}

inline void switch_game_mode(GameState* game_state, GameMode game_mode) {
   GameMode prev_game_mode = game_state->game_mode;
   
   if (game_mode != prev_game_mode) {
       if (prev_game_mode == GameMode_Editor) {
           EditorState* editor = game_state->editor_state;
           editor->camera_p_on_exit = game_state->render_context.camera_p;
       } else if (prev_game_mode == GameMode_Menu) {
           pause_group(&game_state->ui_audio, 1.0f);
       }
   }
   
   MenuState* menu = game_state->menu_state;
   
   switch (game_mode) {
       case GameMode_Menu: {
           menu->source_game_mode = prev_game_mode;
           
           if (!menu->music) {
               menu->music = play_sound(&game_state->ui_audio, get_sound_by_name(&game_state->assets, string_literal("menu_ambient")), vec2(0.0f, 0.0f), Playback_Looping);
               change_volume(menu->music, 2.0f*game_config->menu_fade_in_speed, vec2(0.5f, 0.5f));
           }
           
           unpause_group(&game_state->ui_audio, 1.0f);
           
           if (menu->source_game_mode == GameMode_Menu) {
               menu->fade_in_timer = 1.0f;
           } else {
               menu->fade_in_timer = 0.0f;
           }
           
           if (prev_game_mode == GameMode_Ingame) {
               pause_group(&game_state->game_audio, 1.0f);
               game_state->midi_paused = true;
           }
       } break;
       
       case GameMode_Ingame: {
           if (!strings_are_equal(game_state->desired_level, level_name(game_state->active_level))) {
               load_level(game_state, game_state->desired_level);
               play_level(game_state, game_state->active_level);
           } else if (menu->source_game_mode != GameMode_Ingame || prev_game_mode == GameMode_Editor) {
               play_level(game_state, game_state->active_level);
           }
           
           unpause_group(&game_state->game_audio, 1.0f);
           game_state->midi_paused = false;
       } break;
       
       case GameMode_Editor: {
           if (!strings_are_equal(game_state->desired_level, level_name(game_state->active_level))) {
               load_level(game_state, game_state->desired_level);
           }
           
           stop_all_sounds(&game_state->game_audio);
           stop_all_midi_tracks(game_state);
           
           EditorState* editor = game_state->editor_state;
           game_state->render_context.camera_p = editor->camera_p_on_exit;
           
           editor->shown = true;
           
           game_state->entity_count = 0;
           game_state->player = 0;
       } break;
   }
   
   game_state->game_mode = game_mode;
}

inline CameraView get_camera_view(GameState* game_state, Entity* camera_zone, v2 camera_target) {
   CameraView result;
   
   ADD_DEBUG_BREAK(camera_view);
   
   f32 aspect_ratio = get_aspect_ratio(&game_state->render_context);
   
   v2 rel_camera_p  = camera_target - camera_zone->p;
   v2 view_region   = vec2(aspect_ratio*camera_zone->view_region_height, camera_zone->view_region_height);
   v2 movement_zone = max(vec2(0, 0), camera_zone->active_region - view_region);
   
   f32 k = game_config->camera_stop_smoothness;
   rel_camera_p.x = rel_camera_p.x > 0.0f ? smooth_min(rel_camera_p.x, 0.5f*movement_zone.x, k) : -smooth_min(-rel_camera_p.x, 0.5f*movement_zone.x, k);
   rel_camera_p.y = clamp(rel_camera_p.y, -0.5f*movement_zone.y, 0.5f*movement_zone.y);
   
   result.camera_p            = camera_zone->p + rel_camera_p;
   result.camera_rotation_arm = camera_zone->camera_rotation_arm;
   result.vfov                = view_region.y;
   
   return result;
}

inline CameraView lerp_camera_views(CameraView view1, CameraView view2, f32 t) {
   CameraView lerped_view;
   lerped_view.camera_p = lerp(view1.camera_p, view2.camera_p, t);
   lerped_view.camera_rotation_arm = lerp(view1.camera_rotation_arm, view2.camera_rotation_arm, t); // @TODO: This is obviously not how rotation goes.
   lerped_view.vfov = lerp(view1.vfov, view2.vfov, t);
   return lerped_view;
}

inline void set_camera_view(RenderContext* rc, CameraView view) {
   rc->camera_p = view.camera_p;
   rc->camera_rotation_arm = view.camera_rotation_arm;
   render_worldspace(rc, view.vfov);
}

inline ParticleSystem create_particle_system(MemoryArena* arena, u32 count) {
   ParticleSystem result;
   result.count = count;
   result.particles = push_array(arena, count, Particle);
   return result;
}

internal GAME_UPDATE_AND_RENDER(game_update_and_render) {
   assert(memory->permanent_storage_size >= sizeof(GameState));
   
   game_config = &memory->config;
   platform = memory->platform_api;
   
   //
   // Initialization
   //
   
   GameState* game_state = cast(GameState*) memory->permanent_storage;
   
   if (!memory->initialized) {
       initialize_arena(&game_state->permanent_arena, memory->permanent_storage_size - sizeof(GameState), cast(u8*) memory->permanent_storage + sizeof(GameState));
       initialize_arena(&game_state->transient_arena, memory->transient_storage_size, memory->transient_storage);
       
       // @TODO: Make the load_assets routine ignorant of the platform's file system
       load_assets(&game_state->assets, &game_state->transient_arena, "assets.pla");
       
       game_state->sounds.player_footsteps[0] = get_sound_by_name(&game_state->assets, string_literal("player_footstep_1"));
       game_state->sounds.player_footsteps[1] = get_sound_by_name(&game_state->assets, string_literal("player_footstep_2"));
       game_state->sounds.player_footsteps[2] = get_sound_by_name(&game_state->assets, string_literal("player_footstep_3"));
       
       game_state->sounds.player_jump[0] = get_sound_by_name(&game_state->assets, string_literal("player_jump_1"));
       game_state->sounds.player_jump[1] = get_sound_by_name(&game_state->assets, string_literal("player_jump_2"));
       game_state->sounds.player_jump[2] = get_sound_by_name(&game_state->assets, string_literal("player_jump_3"));
       
       game_state->sounds.player_high_jump = get_sound_by_name(&game_state->assets, string_literal("player_high_jump"));
       
       game_state->sounds.player_land = get_sound_by_name(&game_state->assets, string_literal("player_land"));
       
       initialize_audio_mixer(&game_state->audio_mixer, &game_state->permanent_arena);
       initialize_audio_group(&game_state->game_audio, &game_state->audio_mixer);
       initialize_audio_group(&game_state->ui_audio, &game_state->audio_mixer);
       
       initialize_render_context(&game_state->render_context, render_commands, 30.0f);
       
       {
           ConsoleState* console = game_state->console_state = push_struct(&game_state->permanent_arena, ConsoleState);
           
           initialize_render_context(&game_state->console_state->rc, render_commands, 1.0f);
           console->font = get_font_by_name(&game_state->assets, string_literal("console_font"));
       }
       
       {
           MenuState* menu = game_state->menu_state = push_struct(&game_state->permanent_arena, MenuState);
           
           menu->source_game_mode = GameMode_Menu;
           menu->font          = get_font_by_name(&game_state->assets, string_literal("menu_font"));
           menu->big_font      = get_font_by_name(&game_state->assets, string_literal("big_menu_font"));
           menu->select_sound  = get_sound_by_name(&game_state->assets, string_literal("menu_select"));
           menu->confirm_sound = get_sound_by_name(&game_state->assets, string_literal("menu_confirm"));
       }
       
       game_state->desired_level    = game_config->startup_level;
       game_state->background_level = allocate_level(&game_state->permanent_arena, "background level");
       game_state->active_level     = allocate_level(&game_state->permanent_arena, "no level");
       
       game_state->foreground_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
       game_state->background_color = vec4(0.3f, 0.2f, 0.4f, 1.0f);
       
       game_state->editor_state = allocate_editor(game_state, render_commands);
       
#if 0
       {
           rnd_pcg_t pcg;
           rnd_pcg_seed(&pcg, 0);
           
           f32 aspect_ratio = get_aspect_ratio(&game_state->render_context);
           game_state->background_particles = create_particle_system(&game_state->transient_arena, 1024);
           ParticleSystem* bg_particles = &game_state->background_particles;
           for (u32 particle_index = 0; particle_index < bg_particles->count; particle_index++) {
               Particle particle;
               particle.p = vec2(rnd_pcg_nextf(&pcg) - 0.5f, rnd_pcg_nextf(&pcg) - 0.5f)*vec2(30.0f*aspect_ratio, 30.0f);
               particle.alpha = rnd_pcg_nextf(&pcg);
               particle.depth = map_to_range(rnd_pcg_nextf(&pcg), 1.0f, 10.0f);
               game_state->background_particles.particles[particle_index] = particle;
           }
       }
#endif
       
       switch_game_mode(game_state, GameMode_Menu);
       
       memory->initialized = true;
   }
   
   game_state->audio_mixer.master_volume[0] = game_config->master_volume;
   game_state->audio_mixer.master_volume[1] = game_config->master_volume;
   
   game_state->game_audio.mix_volume[0] = game_config->gameplay_volume;
   game_state->game_audio.mix_volume[1] = game_config->gameplay_volume;
   
   game_state->ui_audio.mix_volume[0] = game_config->ui_volume;
   game_state->ui_audio.mix_volume[1] = game_config->ui_volume;
   
   EditorState* editor = game_state->editor_state;
   
   input->show_cursor = (game_state->game_mode != GameMode_Menu && editor->shown) || game_state->console_state->open;
   
   f32 frame_dt = input->frame_dt;
   v2 mouse_p = vec2(input->mouse_x, input->mouse_y);
   
   RenderContext* render_context = &game_state->render_context;
   
   v2 screen_dim = get_screen_dim(render_context);
   f32 aspect_ratio = get_aspect_ratio(render_context);
   
   execute_console(game_state, game_state->console_state, input);
   
   if (was_pressed(input->debug_fkeys[2])) {
       editor->shown = !editor->shown;
   }
   
   if (was_pressed(input->debug_fkeys[3])) {
       editor->show_statistics = !editor->show_statistics;
   }
   
   if (game_state->game_mode == GameMode_Menu) {
       MenuState* menu = game_state->menu_state;
       
       execute_editor(game_state, editor, input); // @Note: Just to show stats
       
       render_screenspace(render_context);
       push_clear(render_context, game_state->background_color);
       
       // @TODO: I feel like things like this just indicate I should be using more global state
       // Stuffing all these pointers to GameState stuff into structs randomly is hardly any more "safe" or "organized"
       UILayoutContext layout_context;
       layout_context.rc = render_context;
       layout_context.assets = &game_state->assets;
       layout_context.temp_arena = &game_state->transient_arena;
       
       char* menu_items[32];
       u32 item_index = 0;
       u32 start_game   = (menu_items[item_index] = menu->source_game_mode == GameMode_Ingame ? "Resume" : "Start Game", item_index++);
       u32 enter_editor = (menu_items[item_index] = menu->source_game_mode == GameMode_Editor ? "Resume Editing" : "Enter Editor", item_index++);
#if 0
       u32 options      = (menu_items[item_index] = "Options", item_index++);
#endif
       u32 quit         = (menu_items[item_index] = menu->asking_for_quit_confirmation ? "Really Quit?" : "Quit", item_index++);
       
       f32 spacing = 48.0f;
       
       u32 num_items  = item_index;
       f32 row_height = menu->font->size + get_line_spacing(menu->font) + spacing;
       
       GameController* controller = &input->controller;
       if (menu->quit_timer <= 0.0f) {
           if (was_pressed(controller->move_down)) {
               if (menu->selected_item < num_items - 1) {
                   menu->selected_item++;
                   menu->bob_t = 1.0f;
                   menu->asking_for_quit_confirmation = false;
                   play_sound(&game_state->ui_audio, menu->select_sound);
               }
           }
           if (was_pressed(controller->move_up)) {
               if (menu->selected_item > 0) {
                   menu->selected_item--;
                   menu->bob_t = 1.0f;
                   menu->asking_for_quit_confirmation = false;
                   play_sound(&game_state->ui_audio, menu->select_sound);
               }
           }
       }
       
       f32 bob_amount = game_config->menu_bob_magnitude*4.0f*menu->bob_t*(1.0f - menu->bob_t);
       
       if (menu->bob_t > 0.0f) {
           menu->bob_t -= frame_dt / game_config->menu_bob_speed;
           if (menu->bob_t < 0.0f) {
               menu->bob_t = 0.0f;
           }
       }
       
       // @TODO: Consider the height of the title properly so I can accurately center all this without weird guesswork
       UILayout layout = make_layout(layout_context, menu->big_font, vec2(0.5f*screen_dim.x, 0.5f*screen_dim.y + 0.25f*(cast(f32) num_items*row_height)), Layout_CenterAlign);
       set_spacing(&layout, spacing);
       
       layout_print_line(&layout, COLOR_WHITE, "P U L S A R");
       
       set_font(&layout, menu->font);
       
       for (u32 item = 0; item < num_items; item++) {
           v4 color = COLOR_WHITE;
           if (item == menu->selected_item) {
               layout.offset_p = vec2(0.0f, -bob_amount);
               color.rgb = COLOR_YELLOW.rgb;
           }
           
           layout_print_line(&layout, color, menu_items[item]);
           layout.offset_p = vec2(0, 0);
       }
       
       if (was_pressed(controller->interact)) {
           b32 play_the_sound = true;
           
           if (menu->selected_item == start_game) {
               editor->shown = false;
               switch_game_mode(game_state, GameMode_Ingame);
           } else if (menu->selected_item == enter_editor) {
               switch_game_mode(game_state, GameMode_Editor);
#if 0
           } else if (menu->selected_item == options) {
               /* there's no options for now */
#endif
           } else if (menu->selected_item == quit) {
               if (menu->asking_for_quit_confirmation) {
                   if (menu->quit_timer <= 0.0f) {
                       menu->quit_timer = 1.0f;
                       change_volume(menu->music, game_config->menu_quit_speed, vec2(0.0f, 0.0f));
                   } else {
                       play_the_sound = false;
                   }
               } else {
                   menu->asking_for_quit_confirmation = true;
               }
           }
           
           if (play_the_sound) {
               play_sound(&game_state->ui_audio, menu->confirm_sound);
           }
       }
       
       if (was_pressed(input->controller.start)) {
           if (menu->source_game_mode != GameMode_Menu) {
               switch_game_mode(game_state, menu->source_game_mode);
           }
       }
       
       if (menu->quit_timer > 0.0f) {
           menu->quit_timer -= frame_dt / game_config->menu_quit_speed;
           if (menu->quit_timer <= 0.0f) {
               input->quit_requested = true;
           }
           f32 alpha = smoothstep(square_root(clamp01(1.1f*(1.0f - menu->quit_timer))));
           push_shape(render_context, default_transform2d(), rectangle(aab_min_dim(vec2(0, 0), screen_dim)), vec4(0.0f, 0.0f, 0.0f, alpha));
       }
       if (menu->fade_in_timer > 0.0f) {
           menu->fade_in_timer -= frame_dt / game_config->menu_fade_in_speed;
           f32 alpha = smoothstep(clamp01(1.1f*menu->fade_in_timer));
           push_shape(render_context, default_transform2d(), rectangle(aab_min_dim(vec2(0, 0), screen_dim)), vec4(0.0f, 0.0f, 0.0f, alpha));
       }
   } else {
       //
       // Pre-Sim
       //
       
       render_worldspace(render_context, 30.0f);
       
       if (game_state->game_mode == GameMode_Ingame) {
           game_state->background_pulse_dt += frame_dt*(-game_config->background_pulse_spring_force*game_state->background_pulse_t - game_config->background_pulse_spring_dampen*game_state->background_pulse_dt);
           game_state->background_pulse_t  += frame_dt*game_state->background_pulse_dt;
       } else {
           game_state->background_pulse_dt = 0.0f;
           game_state->background_pulse_t  = 0.0f;
       }
       
       v4 background_color = lerp(game_state->background_color, COLOR_YELLOW, clamp(game_state->background_pulse_t, -1.0f, 1.0f));
       push_clear(render_context, background_color);
       
       if (game_state->game_mode == GameMode_Ingame) {
           Entity* player = game_state->player;
           GameController* controller = &input->controller;
           if (player) {
               if (player->dead) {
                   Entity* checkpoint = game_state->last_activated_checkpoint;
                   assert(checkpoint);
                   if (game_state->player_respawn_timer > 0.0f) {
                       game_state->player_respawn_timer -= frame_dt / game_config->player_respawn_speed;
                       if (game_state->player_respawn_timer < 0.0f) {
                           game_state->player_respawn_timer = 0.0f;
                       }
                   } else {
                       player->dead                      = false;
                       player->p                         = checkpoint->respawn_p;
                       player->dp                        = vec2(0, 0);
                       player->ddp                       = vec2(0, 0);
                       player->support_dp                = vec2(0, 0);
                       player->retained_support_dp       = vec2(0, 0);
                       player->ballistic_dp              = vec2(0, 0);
                       player->was_supported             = false;
                       player->support                   = 0;
                       player->retained_support_dp_timer = 0.0f;
                   }
               } else if (!player->killed_this_frame) {
                   if (controller->back.is_down) {
                       game_state->player_self_destruct_timer += frame_dt;
                   } else {
                       game_state->player_self_destruct_timer = 0.0f;
                   }
                   
                   if (game_state->player_self_destruct_timer >= game_config->player_self_destruct_speed) {
                       game_state->player_self_destruct_timer = 0.0f;
                       kill_player(game_state);
                   }
               }
           }
       }
       
       //
       // Sim
       //
       
       if (game_state->game_mode == GameMode_Ingame) {
           simulate_entities(game_state, input, frame_dt);
           
           if (was_pressed(input->debug_fkeys[1])) {
               switch_game_mode(game_state, GameMode_Editor);
           }
       } else if (game_state->game_mode == GameMode_Editor) {
           if (was_pressed(input->debug_fkeys[1])) {
               switch_game_mode(game_state, GameMode_Ingame);
           }
       }
       
       //
       // Post-Sim
       //
       
       if (game_state->game_mode == GameMode_Ingame) {
           if (!game_state->camera_target) {
               game_state->camera_target = game_state->player;
           }
           
           Entity* camera_target = game_state->camera_target;
           Entity* camera_zone = game_state->active_camera_zone;
           
           if (camera_zone && camera_target) {
               CameraView view = get_camera_view(game_state, camera_zone, camera_target->p);
               
               b32 in_death_cam = false;
               Entity* player = game_state->player;
               if (camera_target == player) {
                   if (player->killed_this_frame) {
                       Entity* checkpoint = game_state->last_activated_checkpoint;
                       assert(checkpoint);
                       
                       game_state->death_cam_start = get_camera_view(game_state, camera_zone, player->p);

                       Entity* end_camera_zone = camera_zone;
                       for_entity_type (game_state, EntityType_CameraZone, test_camera_zone) {
                           if (is_in_entity_local_region(test_camera_zone, test_camera_zone->active_region, checkpoint->respawn_p)) {
                               end_camera_zone = test_camera_zone;
                               break;
                           }
                       }
                       
                       game_state->death_cam_end = get_camera_view(game_state, end_camera_zone, checkpoint->respawn_p);
                   } else if (player->dead) {
                       f32 t = smootherstep(1.0f - game_state->player_respawn_timer);
                       view = lerp_camera_views(game_state->death_cam_start, game_state->death_cam_end, t);
                       player->p = view.camera_p;
                       in_death_cam = true;
                   }
               }
               
               if (!in_death_cam) {
                   Entity* prev_camera_zone = game_state->previous_camera_zone;
                   if (prev_camera_zone && game_state->camera_transition_t > 0.0f) {
                       game_state->mid_camera_transition = true;
                       
                       CameraView prev_view = get_camera_view(game_state, prev_camera_zone, camera_target->p);
                       CameraView next_view = get_camera_view(game_state, camera_zone, camera_target->p);
                       
                       f32 t = smootherstep(1.0f - game_state->camera_transition_t);
                       view = lerp_camera_views(prev_view, next_view, t);
                       
                       game_state->camera_transition_t -= frame_dt / game_config->camera_transition_speed;
                       if (game_state->camera_transition_t < 0.0f) {
                           game_state->camera_transition_t = 0.0f;
                       }
                   } else {
                       game_state->mid_camera_transition = false;
                       view = get_camera_view(game_state, camera_zone, camera_target->p);
                   }
               }
               
               set_camera_view(render_context, view);
           }
       }
       
       execute_editor(game_state, editor, input);
       
       u32 render_entity_count = 0;
       Entity* render_entities = 0;
       if (game_state->game_mode == GameMode_Ingame) {
           render_entity_count = game_state->entity_count;
           render_entities = game_state->entities;
       } else if (game_state->game_mode == GameMode_Editor) {
           render_entity_count = game_state->active_level->entity_count;
           render_entities = game_state->active_level->entities;
       }
       
       // push_particle_system(render_context, default_transform2d(), &game_state->background_particles, -1000.0f);
       
       for (u32 entity_index = 1; entity_index < render_entity_count; entity_index++) {
           Entity* entity = render_entities + entity_index;
           assert(entity->type != EntityType_Null);
           
           //
           // Rendering
           //
           
           b32 should_draw = !entity->dead && !(entity->flags & EntityFlag_Invisible);
           
           if (editor->shown) {
               should_draw = !entity->dead;
               if (game_state->game_mode == GameMode_Editor) {
                   should_draw = true;
               }
           }
           
           if (should_draw) {
               f32 old_sort_bias = render_context->sort_key_bias;
               if (entity->flags & EntityFlag_Invisible) {
                   render_context->sort_key_bias += 100.0f;
               }
               
               v2 visual_p = entity->p;
               
               Entity* camera_zone = game_state->active_camera_zone;
               if (camera_zone) {
                   if (is_in_entity_local_region(camera_zone, camera_zone->active_region + entity->collision, visual_p)) {
                       visual_p += vec2(0.0f, -game_config->background_pulse_world_shake_intensity*game_state->background_pulse_t);
                   }
               }
               
               v4 color = entity->color;
               
               Transform2D transform = transform2d(visual_p);
               if (entity->type == EntityType_Player) {
                   // @Hack: Making the player just slightly oversized to avoid seeing a tiny gap between it and touching entities
                   transform.scale *= 1.02f;
                   color = lerp(color, COLOR_RED, game_state->player_self_destruct_timer / game_config->player_self_destruct_speed);
               }
               
               switch (entity->type) {
                   case EntityType_Wall: {
                       if (entity->type == EntityType_Wall && !(entity->flags & EntityFlag_Collides)) {
                           push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), entity->collision)), vec4(color.rgb, color.a*0.5f), ShapeRenderMode_Outline);
                       } else {
                           push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), entity->collision)), color);
                       }
                   } break;
                   
                   case EntityType_CameraZone: {
                       transform.rotation_arm = entity->camera_rotation_arm;
                       Image* sprite = get_image(&game_state->assets, entity->sprite);
                       if (sprite) {
                           push_image(render_context, transform, sprite, color);
                       } else {
                           INVALID_CODE_PATH;
                       }
                       if (editor->show_camera_zones) {
                           push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), entity->active_region)), color, ShapeRenderMode_Outline, 1000.0f);
                           push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), vec2(aspect_ratio*entity->view_region_height, entity->view_region_height))), color*vec4(1, 1, 1, 0.25f), ShapeRenderMode_Outline, 1000.0f);
                       }
                   } break;
                   
                   case EntityType_Checkpoint: {
                       Image* sprite = get_image(&game_state->assets, entity->sprite);
                       if (sprite) {
                           push_image(render_context, transform, sprite, color);
                       } else {
                           INVALID_CODE_PATH;
                       }
                       if (editor->show_checkpoint_zones) {
                           push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), entity->checkpoint_zone)), game_state->last_activated_checkpoint == entity ? COLOR_GREEN : COLOR_RED, ShapeRenderMode_Outline, 1000.0f);
                       }
                   } break;
                   
                   case EntityType_SoundtrackPlayer: {
                       Image* sprite = get_image(&game_state->assets, entity->sprite);
                       if (sprite) {
                           push_image(render_context, transform, sprite, color);
                       } else {
                           INVALID_CODE_PATH;
                       }
                       if (editor->show_soundtrack_player_zones) {
                           push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), entity->audible_zone + vec2(entity->horz_fade_region, entity->vert_fade_region))), vec4(COLOR_GREEN.rgb, 0.5f), ShapeRenderMode_Outline, 1000.0f);
                           push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), entity->audible_zone)), COLOR_RED, ShapeRenderMode_Outline, 1000.0f);
                       }
                   } break;
                   
                   case EntityType_TriggerZone: {
                       Image* sprite = get_image(&game_state->assets, entity->sprite);
                       if (sprite) {
                           push_image(render_context, transform, sprite, color);
                       } else {
                           INVALID_CODE_PATH;
                       }
                       if (editor->show_trigger_zones) {
                           color = COLOR_GREEN;
                           if (entity->touching_player && entity->enveloping_player) {
                               color = COLOR_PINK;
                           } else if (entity->touching_player) {
                               color = COLOR_CYAN;
                           } else if (entity->enveloping_player) {
                               color = COLOR_BLUE;
                           }
                           push_shape(render_context, transform, rectangle(aab_center_dim(vec2(0, 0), entity->trigger_zone)), color, ShapeRenderMode_Outline, 1000.0f);
                       }
                   } break;
                   
                   default: {
                       if (entity->sprite.value) {
                           Image* sprite = get_image(&game_state->assets, entity->sprite);
                           if (sprite) {
                               push_image(render_context, transform, sprite, color);
                           } else {
                               INVALID_CODE_PATH;
                           }
                       } else {
                           push_shape(render_context, transform, rectangle(entity->collision), color);
                       }
                   } break;
               }
               
               render_context->sort_key_bias = old_sort_bias;
           }
           
           if (entity->sprite.value) {
               entity->color = COLOR_WHITE;
           } else {
               entity->color = (entity->flags & EntityFlag_Hazard) ? COLOR_RED : game_state->foreground_color;
               if (entity->flags & EntityFlag_Invisible) {
                   entity->color.a *= 0.25f;
               }
           }
       }
       
       if (game_state->game_mode == GameMode_Ingame) {
           if (game_state->level_intro_timer > 0.0f) {
               game_state->level_intro_timer -= frame_dt / game_config->level_intro_speed;
               if (game_state->level_intro_timer < 0.0f) {
                   game_state->level_intro_timer = 0.0f;
               }
               // @TODO: AAAA!!!!!!! (Translation: Because setting render_screenspace sets the render context's camera p,
               // this breaks some stuff where the editor restores your camera position, or something. Bummer.)
               RenderContext rc_backup = *render_context;
               render_screenspace(render_context);
               push_rect(render_context, aab_min_dim(vec2(0, 0), screen_dim), vec4(0, 0, 0, game_state->level_intro_timer));
               *render_context = rc_backup;
           }
           
           if (game_state->level_outro_timer > 0.0f) {
               game_state->level_outro_timer -= frame_dt / game_config->level_outro_speed;
               if (game_state->level_outro_timer < 0.0f) {
                   game_state->level_outro_timer = 0.0f;
                   stop_all_midi_tracks(game_state);
                   stop_all_sounds(&game_state->game_audio);
                   change_volume(&game_state->game_audio, 0.0f, vec2(1, 1));
                   if (editor->shown) {
                       switch_game_mode(game_state, GameMode_Editor);
                   } else {
                       // @Hack: Double switch to menu to reset to the menu completely
                       switch_game_mode(game_state, GameMode_Menu);
                       switch_game_mode(game_state, GameMode_Menu);
                   }
               }
               
               f32 fade_to_black_t = clamp01_map_to_range(game_state->level_outro_timer, 1.0f, 0.7f);
               f32 text_fade_in_t  = clamp01_map_to_range(game_state->level_outro_timer, 0.6f, 0.5f);
               f32 text_fade_out_t = clamp01_map_to_range(game_state->level_outro_timer, 0.2f, 0.1f);
               
               f32 text_alpha = smoothstep(clamp01(text_fade_in_t - text_fade_out_t));
               
               // @TODO: AAAA!!!!!!! (Translation: Because setting render_screenspace sets the render context's camera p,
               // this breaks some stuff where the editor restores your camera position, or something. Bummer.)
               RenderContext rc_backup = *render_context;
               
               render_screenspace(render_context);
               push_rect(render_context, aab_min_dim(vec2(0, 0), screen_dim), vec4(0, 0, 0, smoothstep(fade_to_black_t)));
               
               UILayoutContext layout_context;
               layout_context.rc = render_context;
               layout_context.assets = &game_state->assets;
               layout_context.temp_arena = &game_state->transient_arena;
               
               UILayout outro_text = make_layout(layout_context, game_state->menu_state->big_font, 0.5f*screen_dim, Layout_CenterAlign);
               layout_print_line(&outro_text, vec4(COLOR_WHITE.rgb, text_alpha), "fin.");
               
               *render_context = rc_backup;
           }
       }
       
       if (game_state->player && game_state->player->killed_this_frame) {
           game_state->player->dead = true;
           game_state->player->killed_this_frame = false;
       }
       
       if (was_pressed(input->controller.start)) {
           switch_game_mode(game_state, GameMode_Menu);
       }
   }
}

internal GAME_GET_SOUND(game_get_sound) {
   assert(memory->initialized);
   GameState* game_state = cast(GameState*) memory->permanent_storage;
   
   output_playing_sounds(&game_state->audio_mixer, sound_buffer, &game_state->transient_arena);
}

internal GAME_POST_RENDER(game_post_render) {
   assert(memory->initialized);
   GameState* game_state = cast(GameState*) memory->permanent_storage;
   
   // @Note: One day I'll use this... Or not. The idea of this function was to be able to purge temporary memory
   // and such that I might want to use to send polygons generated during the frame to the renderer, but that
   // temporary memory could also just be purged next time we enter game_update_and_render.
   
   check_arena(&game_state->permanent_arena);
   check_arena(&game_state->transient_arena);
}
