#include "game_main.h"

#include "game_assets.cpp"
#include "audio_mixer.cpp"

internal GAME_UPDATE_AND_RENDER(game_update_and_render) {
    assert(memory->permanent_storage_size >= sizeof(GameState));

    platform = memory->platform_api;

    GameState* game_state = cast(GameState*) memory->permanent_storage;
    if (!memory->initialized) {
        initialize_arena(&game_state->permanent_arena, memory->permanent_storage_size - sizeof(GameState), cast(u8*) memory->permanent_storage + sizeof(GameState));
        initialize_arena(&game_state->transient_arena, memory->transient_storage_size, memory->transient_storage);

        initialize_audio_mixer(&game_state->audio_mixer, &game_state->permanent_arena);

        load_assets(&game_state->assets, &game_state->transient_arena, "assets.pak");
        game_state->test_music = get_sound(&game_state->assets, Asset_TestMusic);
        game_state->test_sound = get_sound(&game_state->assets, Asset_TestSound);
        game_state->test_image = get_image(&game_state->assets, Asset_TestImage);

        game_state->test_image->handle = cast(void*) opengl_load_texture(
            &opengl_info,
            game_state->test_image->w,
            game_state->test_image->h,
            game_state->test_image->pixels
        );

        play_sound(&game_state->audio_mixer, game_state->test_music);
        play_sound(&game_state->audio_mixer, game_state->test_sound);

        memory->initialized = true;
    }

    opengl_set_screenspace(width, height);
    opengl_texture(cast(GLuint) game_state->test_image->handle, vec2(0, 0), vec2(width / 2, height / 2), vec4(1, 1, 1, 1));

    check_arena(&game_state->permanent_arena);
    check_arena(&game_state->transient_arena);
}

internal GAME_GET_SOUND(game_get_sound) {
    assert(memory->initialized);
    GameState* game_state = cast(GameState*) memory->permanent_storage;

    output_playing_sounds(&game_state->audio_mixer, sound_buffer, &game_state->transient_arena);
    if (game_state->sound_timer > 120) {
        play_sound(&game_state->audio_mixer, game_state->test_sound);
        game_state->sound_timer = 0;
    }
    game_state->sound_timer++;
}
