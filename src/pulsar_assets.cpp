internal void load_assets(Assets* assets, MemoryArena* arena, char* file_name) {
    // @TODO: Make load_assets ignorant of the platform's file system
    EntireFile asset_file = platform.read_entire_file(file_name, allocator(arena_allocator, arena));
    if (asset_file.size > 0) {
        AssetPackHeader* header = cast(AssetPackHeader*) asset_file.data;
        assert(header->magic_value == ASSET_PACK_CODE('p', 'l', 'a', 'f'));
        assert(header->version == 0);

        assets->asset_count = header->asset_count;
        assets->asset_catalog = push_array(arena, assets->asset_count, Asset, no_clear());

        char* name_store = cast(char*) header + header->asset_name_store;
        PackedAsset* source_catalog = cast(PackedAsset*) (cast(u8*) header + header->asset_catalog);
        assets->asset_data = cast(u8*) header + header->asset_data;

        for (u32 asset_index = 0; asset_index < assets->asset_count; asset_index++) {
            PackedAsset* source_asset = source_catalog + asset_index;
            Asset* dest_asset = assets->asset_catalog + asset_index;

            dest_asset->name.data = name_store + source_asset->name_offset;
            dest_asset->name.len  = cstr_length(dest_asset->name.data);
            // @Note: Not actually necessary, but nice for type safety I guess
            // @TODO: Make asset type checking compile out in release?
            dest_asset->type = source_asset->type;

            // @TODO: The actual loading of data and the loading in of just the
            // asset catalog info should be separate in a real world scenario
            // where we won't load the entire game's asset file into memory at
            // once.
            switch (source_asset->type) {
                case AssetType_Image: {
                    Image* image = &dest_asset->image;
                    image->packed_image = source_asset->image;
                    image->pixels = assets->asset_data + source_asset->data_offset;
                    image->handle = platform.allocate_texture(image->w, image->h, image->pixels, image->pixel_format);
                } break;

                case AssetType_Sound: {
                    Sound* sound = &dest_asset->sound;
                    sound->packed_sound = source_asset->sound;
                    sound->samples = cast(s16*) (assets->asset_data + source_asset->data_offset);
                } break;

                case AssetType_Font: {
                    Font* font = &dest_asset->font;
                    font->packed_font = source_asset->font;
                    u32 glyph_count = font->one_past_last_codepoint - font->first_codepoint;
                    font->glyph_table = cast(ImageID*) (assets->asset_data + source_asset->data_offset);
                    font->kerning_table = cast(f32*) (font->glyph_table + sizeof(ImageID)*glyph_count);
                } break;

                case AssetType_Midi: {
                    MidiTrack* midi_track = &dest_asset->midi_track;
                    midi_track->packed_midi = source_asset->midi;
                    midi_track->events = cast(MidiEvent*) (assets->asset_data + source_asset->data_offset);
                } break;

                case AssetType_Soundtrack: {
                    Soundtrack* soundtrack = &dest_asset->soundtrack;
                    soundtrack->packed_soundtrack = source_asset->soundtrack;
                    soundtrack->midi_tracks = cast(MidiID*) (assets->asset_data + source_asset->data_offset);
                } break;
            }
        }
    } else {
        // @TODO: Elegant handling of asset file errors
        INVALID_CODE_PATH;
    }
}

inline AssetID get_asset_id_by_name(Assets* assets, String name, AssetType asset_type = AssetType_Unknown) {
    AssetID result = { 0 };

    for (u32 asset_index = 1; asset_index < assets->asset_count; asset_index++) {
        Asset* asset = assets->asset_catalog + asset_index;
        if (asset->name.len && strings_are_equal(asset->name, name)) {
            if (asset_type == AssetType_Unknown || asset->type == asset_type) {
                result = { asset_index };
                break;
            }
        }
    }

    if (!result.value) {
        // @TODO: Check if the asset was present, but there was a type mismatch
        log_print(LogLevel_Error, "Could not find asset '%.*s'", PRINTF_STRING(name));
    }

    return result;
}

inline ImageID get_image_id_by_name(Assets* assets, String name) {
    ImageID result = { get_asset_id_by_name(assets, name, AssetType_Image).value };
    return result;
}

inline SoundID get_sound_id_by_name(Assets* assets, String name) {
    SoundID result = { get_asset_id_by_name(assets, name, AssetType_Sound).value };
    return result;
}

inline FontID get_font_id_by_name(Assets* assets, String name) {
    FontID result = { get_asset_id_by_name(assets, name, AssetType_Font).value };
    return result;
}

inline SoundtrackID get_soundtrack_id_by_name(Assets* assets, String name) {
    SoundtrackID result = { get_asset_id_by_name(assets, name, AssetType_Soundtrack).value };
    return result;
}

inline Asset* get_asset(Assets* assets, AssetID asset_id) {
    assert(asset_id.value < assets->asset_count);

    Asset* result = 0;
    if (asset_id.value > 0 && asset_id.value < assets->asset_count) {
        result = assets->asset_catalog + asset_id.value;
    } else {
        log_print(LogLevel_Error, "Asset ID %u was out of range", asset_id.value);
    }

    return result;
}

inline Sound* get_sound(Assets* assets, SoundID id) {
    Sound* result = 0;
    Asset* asset = get_asset(assets, { id.value });
    if (asset) {
        if (asset->type == AssetType_Sound) {
            result = &asset->sound;
        } else {
            log_print(LogLevel_Error, "Asset type mismatch for '%.*s' (%u): Expected type AssetType_Sound, asset is of type %s", PRINTF_STRING(asset->name), id.value, enum_name(AssetType, asset->type));
        }
    }

    return result;
}

inline Sound* get_sound_by_name(Assets* assets, String name) {
    Sound* result = 0;

    AssetID asset_id = get_asset_id_by_name(assets, name);
    if (asset_id.value) {
        result = get_sound(assets, { asset_id.value });
    }

    return result;
}

inline Image* get_image(Assets* assets, ImageID id) {
    Image* result = 0;
    Asset* asset = get_asset(assets, { id.value });
    if (asset) {
        if (asset->type == AssetType_Image) {
            result = &asset->image;
        } else {
            log_print(LogLevel_Error, "Asset type mismatch for '%.*s' (%u): Expected type AssetType_Image, asset is of type %s", PRINTF_STRING(asset->name), id.value, enum_name(AssetType, asset->type));
        }
    }

    return result;
}


inline Image* get_image_by_name(Assets* assets, String name) {
    Image* result = 0;

    AssetID asset_id = get_asset_id_by_name(assets, name);
    if (asset_id.value) {
        result = get_image(assets, { asset_id.value });
    }

    return result;
}

inline Font* get_font(Assets* assets, FontID id) {
    Font* result = 0;
    Asset* asset = get_asset(assets, { id.value });
    if (asset) {
        if (asset->type == AssetType_Font) {
            result = &asset->font;
        } else {
            log_print(LogLevel_Error, "Asset type mismatch for '%.*s' (%u): Expected type AssetType_Font, asset is of type %s", PRINTF_STRING(asset->name), id.value, enum_name(AssetType, asset->type));
        }
    }

    return result;
}

inline Font* get_font_by_name(Assets* assets, String name) {
    Font* result = 0;

    AssetID asset_id = get_asset_id_by_name(assets, name);
    if (asset_id.value) {
        result = get_font(assets, { asset_id.value });
    }

    return result;
}

inline b32 in_font_range(Font* font, u32 codepoint) {
    u32 glyph_table_index = codepoint - font->first_codepoint;
    u32 glyph_count = font->one_past_last_codepoint - font->first_codepoint;
    b32 result = codepoint >= font->first_codepoint && glyph_table_index < glyph_count;
    return result;
}

inline ImageID get_glyph_id_for_codepoint(Font* font, u32 codepoint) {
    u32 glyph_table_index = codepoint - font->first_codepoint;
    u32 glyph_count = font->one_past_last_codepoint - font->first_codepoint;
    ImageID result = { 0 };
    if (in_font_range(font, codepoint)) {
        result = font->glyph_table[glyph_table_index];
    }
    return result;
}

inline f32 get_advance_for_codepoint_pair(Font* font, u32 c1, u32 c2) {
    u32 g1 = c1 - font->first_codepoint;
    u32 g2 = c2 - font->first_codepoint;
    u32 glyph_count = font->one_past_last_codepoint - font->first_codepoint;
    f32 result = font->kerning_table[glyph_count*g2 + g1];
    return result;
}

inline f32 get_baseline(Font* font) {
    f32 result = font->descent;
    return result;
}

inline f32 get_baseline_from_top(Font* font) {
    f32 result = font->ascent;
    return result;
}

inline f32 get_line_spacing(Font* font) {
    f32 result = font->ascent + font->descent + font->line_gap;
    return result;
}

inline MidiTrack* get_midi(Assets* assets, MidiID id) {
    MidiTrack* result = 0;
    Asset* asset = get_asset(assets, { id.value });
    if (asset) {
        if (asset->type == AssetType_Midi) {
            result = &asset->midi_track;
        } else {
            log_print(LogLevel_Error, "Asset type mismatch for '%.*s' (%u): Expected type AssetType_Midi, asset is of type %s", PRINTF_STRING(asset->name), id.value, enum_name(AssetType, asset->type));
        }
    }

    return result;
}

inline MidiTrack* get_midi_by_name(Assets* assets, String name) {
    MidiTrack* result = 0;

    AssetID asset_id = get_asset_id_by_name(assets, name);
    if (asset_id.value) {
        result = get_midi(assets, { asset_id.value });
    }

    return result;
}

inline Soundtrack* get_soundtrack(Assets* assets, SoundtrackID id) {
    Soundtrack* result = 0;
    Asset* asset = get_asset(assets, { id.value });
    if (asset) {
        if (asset->type == AssetType_Soundtrack) {
            result = &asset->soundtrack;
        } else {
            log_print(LogLevel_Error, "Asset type mismatch for '%.*s' (%u): Expected type AssetType_Soundtrack, asset is of type %s", PRINTF_STRING(asset->name), id.value, enum_name(AssetType, asset->type));
        }
    }

    return result;
}

inline Soundtrack* get_soundtrack_by_name(Assets* assets, String name) {
    Soundtrack* result = 0;

    AssetID asset_id = get_asset_id_by_name(assets, name);
    if (asset_id.value) {
        result = get_soundtrack(assets, { asset_id.value });
    }

    return result;
}
