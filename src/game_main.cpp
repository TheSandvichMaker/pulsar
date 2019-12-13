#include "game_main.h"

#include "asset_loading.h"
#include "asset_loading.cpp"

#include "game_assets.cpp"
#include "audio_mixer.cpp"

enum ShapeType {
    Shape_Circle,
    Shape_Polygon,
};

struct Shape2D {
    ShapeType type;
    union {
        f32 radius;
        struct {
            size_t vert_count;
            v2* vertices;
        };
    };
};

inline void draw_shape(Shape2D shape, v2 offset, v4 color = vec4(1, 1, 1, 1)) {
    glBegin(GL_LINE_LOOP);
    glColor4fv(color.e);

    for (u32 vertex_index = 0; vertex_index < shape->vert_count; vertex_index++) {
        v2 v = shape->vertices[vertex_index] + offset;
        glVertex2fv(v.e);
    }

    glEnd();
}

inline v2 get_furthest_point_along(Shape2D s, v2 dir) {
    v2 best_point = vec2(0, 0);

    if (s.vert_count > 0) {
        f32 best_dist = dot(s.vertices[0], dir);
        best_point = s.vertices[0];
        for (u32 vertex_index = 1; vertex_index < s.vert_count; vertex_index++) {
            v2 vertex = s.vertices[vertex_index];
            f32 dist_along_dir = dot(vertex, dir);
            if (dist_along_dir > best_dist) {
                best_point = vertex;
                best_dist = dist_along_dir;
            }
        }
    }

    return best_point;
}

inline v2 support(Shape2D s1, Shape2D s2, v2 dir) {
    v2 a = get_furthest_point_along(s1, dir);
    v2 b = get_furthest_point_along(s2, -dir);
    v2 ab = a - b;
    return ab;
}

inline v2 triple_product(v2 a, v2 b, v2 c) {
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
}

inline b32 gjk_intersect(Shape2D s1, Shape2D s2) {
    b32 result = false;

    u32 pc = 0;
    v2 p[3] = {};
    v2 s = support(s1, s2, vec2(0, 1));

    p[pc++] = s;
    v2 d = -s;
    for (;;) {
        v2 a = support(s1, s2, d);
        if (dot(a, d) <= 0) {
            break;
        }
        p[pc++] = a;

        Shape2D simplex;
        simplex.vert_count = pc;
        simplex.vertices = p;
        draw_shape(simplex, s1.vertices[0]);

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
    draw_shape(arrow, vec2(0, 0), color);
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

    v4 color = vec4(1, 1, 1, 1);

    v2 mouse_p = vec2(input->mouse_x, input->mouse_y);
    game_state->p2 = mouse_p;
    if (input->mouse_buttons[PlatformMouseButton_Left].is_down) {
        color = vec4(0, 1, 0, 1);
    }

    GameController* keyboard = get_controller(input, PLATFORM_KEYBOARD_CONTROLLER);
    if (keyboard->is_connected) {
        if (keyboard->move_left.is_down) {
            game_state->p1.x -= 2.0f;
        }
        if (keyboard->move_right.is_down) {
            game_state->p1.x += 2.0f;
        }
        if (keyboard->move_down.is_down) {
            game_state->p1.y -= 2.0f;
        }
        if (keyboard->move_up.is_down) {
            game_state->p1.y += 2.0f;
        }
    }

    glViewport(0, 0, width, height);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    opengl_set_screenspace(width, height);

    v2 offsets[2] = { game_state->p1, game_state->p2 };

    v2 vertices[2][4] = {
        { { 0.0f, 0.0f }, { 50.0f, 0.0f }, { 60.0f, 100.0f }, { -60.0f, 100.0f } },
        { { 0.0f, 0.0f }, { 20.0f, 0.0f }, { 80.0f, 20.0f }, { -60.0f, 50.0f } },
    };

    Shape2D shapes[2];
    shapes[0].vert_count = 4;
    shapes[0].vertices = vertices[0];
    shapes[1].vert_count = 4;
    shapes[1].vertices = vertices[1];

    v2 test_dir = mouse_p - game_state->p1;
    v2 best_p = game_state->p1 + get_furthest_point_along(shapes[0], test_dir);

    draw_arrow(game_state->p1, mouse_p, vec4(1, 1, 0, 1));

    v2 a = 10.0f*vec2(-4, -3);
    v2 b = 10.0f*vec2(-1, 5);
    v2 ab = b - a;
    v2 ao = -a;
    v2 perp = ao*length_sq(ab) - ab*dot(ab, ao);

    draw_arrow(mouse_p + a, mouse_p + b, vec4(1, 0, 0, 1));
    draw_arrow(mouse_p + 0.5f*(b+a), mouse_p + 0.5f*(b+a) + 100.0f*normalize_or_zero(perp), vec4(0, 1, 0, 1));

    opengl_rectangle(best_p - vec2(8, 8), best_p + vec2(8, 8), vec4(0, 1, 0, 1));

    for (u32 shape_index = 0; shape_index < 2; shape_index++) {
        Shape2D* shape = shapes + shape_index;
        v2 offset = offsets[shape_index];

        for (u32 vert_index = 0; vert_index < shape->vert_count; vert_index++) {
            shape->vertices[vert_index] += offset;
        }

    }

    v4 col = vec4(1, 0, 0, 1);
    if (gjk_intersect(shapes[0], shapes[1])) {
        col = vec4(0, 1, 0, 1);
    }

    draw_shape(shapes[0], vec2(0, 0), col);
    draw_shape(shapes[1], vec2(0, 0), col);

    glDisable(GL_BLEND);

    check_arena(&game_state->permanent_arena);
    check_arena(&game_state->transient_arena);
}

internal GAME_GET_SOUND(game_get_sound) {
    assert(memory->initialized);
    GameState* game_state = cast(GameState*) memory->permanent_storage;

    output_playing_sounds(&game_state->audio_mixer, sound_buffer, &game_state->transient_arena);
}
