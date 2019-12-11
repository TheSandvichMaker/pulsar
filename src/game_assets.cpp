internal void load_assets(Assets* assets, MemoryArena* arena, char* file_name) {
    // @TODO: Load assets into the memory arena instead of the space allocated
    // by platform.read_entire_file
    EntireFile asset_file = platform.read_entire_file(file_name);
    if (asset_file.size > 0) {
        AssetPackHeader* header = cast(AssetPackHeader*) asset_file.data;
        assert(header->magic_value == ASSET_PACK_CODE('p', 'a', 'k', 'f'));
        assert(header->version == 0);
        assert(header->asset_count <= Asset_Count);

        PackedAsset* source_catalog = cast(PackedAsset*) (cast(u8*) header + header->asset_catalog);
        assets->asset_data = cast(u8*) header + header->asset_data;

        for (u32 asset_index = 0; asset_index < ARRAY_COUNT(assets->asset_catalog); asset_index++) {
            PackedAsset* source_asset = source_catalog + asset_index;
            Asset* dest_asset = assets->asset_catalog + asset_index;
            switch (source_asset->type) {
                case AssetType_Image: {
                    Image* image = &dest_asset->image;
                    image->w = source_asset->image.w;
                    image->h = source_asset->image.h;
                    image->center_point = vec2(source_asset->image.center_point_x, source_asset->image.center_point_y);
                    image->pixels = assets->asset_data + source_asset->data_offset;
                    image->handle = 0;
                } break;

                case AssetType_Sound: {
                    Sound* sound = &dest_asset->sound;
                    sound->channel_count = source_asset->sound.channel_count;
                    sound->sample_count = source_asset->sound.sample_count;
                    sound->samples = cast(s16*) (assets->asset_data + source_asset->data_offset);
                } break;
            }
        }
    } else {
        // @TODO: Elegant handling of asset file errors
        INVALID_CODE_PATH;
    }
}

inline Sound* get_sound(Assets* assets, AssetID asset_id) {
    assert(asset_id < Asset_Count);
    Sound* result = &assets->asset_catalog[asset_id].sound;
    return result;
}

inline Image* get_image(Assets* assets, AssetID asset_id) {
    assert(asset_id < Asset_Count);
    Image* result = &assets->asset_catalog[asset_id].image;
    return result;
}
