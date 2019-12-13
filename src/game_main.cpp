#include "game_main.h"

#include "asset_loading.h"
#include "asset_loading.cpp"

#include "game_assets.cpp"
#include "audio_mixer.cpp"

inline void draw_shape(Transform2D transform, Shape2D shape, v4 color = vec4(1, 1, 1, 1)) {
    glBegin(GL_LINE_LOOP);
    glColor4fv(color.e);

    u32 circle_quality = 32;

    switch (shape.type) {
        case Shape_Polygon: {
            for (u32 vertex_index = 0; vertex_index < shape.vert_count; vertex_index++) {
                v2 v = rotate(transform.scale*shape.vertices[vertex_index], transform.rotation_arm) + transform.offset;
                glVertex2fv(v.e);
            }
        } break;

        case Shape_Circle: {
            for (f32 segment_angle = 0; segment_angle < TAU_32; segment_angle += (TAU_32 / cast(f32) circle_quality)) {
                v2 v = rotate(transform.scale*vec2(sin(segment_angle), cos(segment_angle))*shape.radius, transform.rotation_arm) + transform.offset;
                glVertex2fv(v.e);
            }
        } break;
    }

    glEnd();
}

inline v2 get_furthest_point_along(Shape2D s, v2 dir) {
    v2 result = vec2(0, 0);

    switch (s.type) {
        case Shape_Polygon: {
            if (s.vert_count > 0) {
                f32 best_dist = dot(s.vertices[0], dir);
                result = s.vertices[0];
                for (u32 vertex_index = 1; vertex_index < s.vert_count; vertex_index++) {
                    v2 vertex = s.vertices[vertex_index];
                    f32 dist_along_dir = dot(vertex, dir);
                    if (dist_along_dir > best_dist) {
                        result = vertex;
                        best_dist = dist_along_dir;
                    }
                }
            }
        } break;

        case Shape_Circle: {
            result = normalize_or_zero(dir)*s.radius;
        } break;
    }

    return result;
}

inline v2 support(Transform2D s1_t, Shape2D s1, Transform2D s2_t, Shape2D s2, v2 dir) {
    v2 a = s1_t.offset + rotate(s1_t.scale*get_furthest_point_along(s1,  rotate_clockwise(dir, s1_t.rotation_arm)), s1_t.rotation_arm);
    v2 b = s2_t.offset + rotate(s2_t.scale*get_furthest_point_along(s2, -rotate_clockwise(dir, s2_t.rotation_arm)), s2_t.rotation_arm);
    v2 ab = a - b;
    return ab;
}

inline v2 triple_product(v2 a, v2 b, v2 c) {
    // @TODO: Work through the proof that this equals AxBxC
    v2 result = b*dot(c, a) - a*(dot(c, b));
    return result;
}

inline b32 gjk_do_simplex(u32* pc, v2* p, v2* d) {
#define same_direction(x) dot(x, ao) > 0
    b32 result = false;

    v2 a = p[*pc - 1];
    v2 ao = -a;

    if (*pc == 2) {
        v2 b = p[0];
        v2 ab = b - a;
        *d = triple_product(ab, ao, ab);
    } else if (*pc == 3) {
        v2 b = p[1];
        v2 c = p[0];
        v2 ab = b - a;
        v2 ac = c - a;
        v2 ab_perp = triple_product(ac, ab, ab);
        v2 ac_perp = triple_product(ab, ac, ac);
        if (dot(ab_perp, ao) > 0) {
            *pc = 2;
            p[1] = a;
            p[0] = b;
            *d = ab_perp;
        } else {
            if (dot(ac_perp, ao) > 0) {
                *pc = 2;
                p[1] = a;
                p[0] = c;
                *d = ac_perp;
            } else {
                result = true;
            }
        }
    } else {
        INVALID_CODE_PATH;
    }

    return result;
#undef same_direction
}

inline b32 gjk_intersect(Transform2D s1_t, Shape2D s1, Transform2D s2_t, Shape2D s2) {
    b32 result = false;

    u32 pc = 0;
    v2 p[3] = {};
    v2 s = support(s1_t, s1, s2_t, s2, vec2(0, 1));

    p[pc++] = s;
    v2 d = -s;
    for (;;) {
        v2 a = support(s1_t, s1, s2_t, s2, d);
        if (dot(a, d) <= 0) {
            break;
        }
        p[pc++] = a;

        if (gjk_do_simplex(&pc, p, &d)) {
            result = true;
            break;
        }
    }

    return result;
}

inline void draw_arrow(v2 start, v2 end, v4 color = vec4(1, 1, 1, 1)) {
    v2 dir = end - start;
    v2 perp = perpendicular(dir);
    v2 fin = 0.1f*perp;
    v2 neck = start + 0.9f*dir;
    v2 arrow_verts[] = { start, neck, neck + fin, end, neck - fin, neck };
    Shape2D arrow;
    arrow.vert_count = ARRAY_COUNT(arrow_verts);
    arrow.vertices = arrow_verts;
    draw_shape(default_transform2d(), arrow, color);
}

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

        // play_sound(&game_state->audio_mixer, game_state->test_music);
        // play_sound(&game_state->audio_mixer, game_state->test_sound);

        memory->initialized = true;
    }

    v2 mouse_p = vec2(input->mouse_x, input->mouse_y);

    Entity* player = &game_state->player;
    GameController* keyboard = get_controller(input, PLATFORM_KEYBOARD_CONTROLLER);
    if (keyboard->is_connected) {
        if (keyboard->move_left.is_down) {
            // player->p.x -= 2.0f;
            player->rotation -= TAU_32/100.0f;
        }
        if (keyboard->move_right.is_down) {
            // player->p.x += 2.0f;
            player->rotation += TAU_32/100.0f;
        }
        v2 x_axis = vec2(cos(player->rotation), sin(player->rotation));
        v2 y_axis = perpendicular(x_axis);
        if (keyboard->move_down.is_down) {
            player->p -= 2.0f*y_axis;
        }
        if (keyboard->move_up.is_down) {
            player->p += 2.0f*y_axis;
        }
    }

    if (player->rotation > PI_32) {
        player->rotation -= TAU_32;
    }

    glViewport(0, 0, width, height);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    opengl_set_screenspace(width, height);

    v2 vertices[] = { { -1.0f, -1.0f }, { 1.0f, -1.0f }, { 0.0f, 1.0f } };
    Shape2D shape = polygon(ARRAY_COUNT(vertices), vertices);

    Transform2D transform = default_transform2d();
    transform.offset = vec2(512, 512);
    transform.scale = 100.0f;
    transform.rotation_arm = arm2(game_state->rotation);

    Transform2D player_transform = default_transform2d();
    player_transform.offset = player->p;
    player_transform.scale = 50.0f;
    player_transform.rotation_arm = arm2(player->rotation);

    game_state->rotation += input->frame_dt;

    v4 color = vec4(1, 0, 0, 1);
    if (gjk_intersect(transform, shape, player_transform, shape)) {
        color = vec4(0, 1, 0, 1);
    }

    draw_shape(transform, shape, color);
    draw_shape(player_transform, shape, color);

    v2 x_axis = vec2(cos(player->rotation), sin(player->rotation));
    v2 y_axis = perpendicular(x_axis);
    v2 min_p = -(x_axis*cast(f32) game_state->test_image->w + y_axis*cast(f32) game_state->test_image->h)*game_state->test_image->center_point;
    x_axis *= cast(f32) game_state->test_image->w;
    y_axis *= cast(f32) game_state->test_image->h;
    opengl_texture(cast(GLuint) game_state->test_image->handle, player->p + min_p, x_axis, y_axis, vec4(1, 1, 1, 1));

    glDisable(GL_BLEND);

    check_arena(&game_state->permanent_arena);
    check_arena(&game_state->transient_arena);
}

internal GAME_GET_SOUND(game_get_sound) {
    assert(memory->initialized);
    GameState* game_state = cast(GameState*) memory->permanent_storage;

    output_playing_sounds(&game_state->audio_mixer, sound_buffer, &game_state->transient_arena);
}
