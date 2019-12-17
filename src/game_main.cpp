#include "game_main.h"

#include "asset_loading.h"
#include "asset_loading.cpp"

#include "game_assets.cpp"
#include "audio_mixer.cpp"

#define DEBUG_GJK_VISUALIZATION 1

inline v2 get_furthest_point_along(Transform2D t, Shape2D s, v2 d) {
    v2 result = vec2(0, 0);

    switch (s.type) {
        case Shape_Polygon: {
            if (s.vert_count > 0) {
                f32 best_dist = dot(s.vertices[0], d);
                result = s.vertices[0];
                for (u32 vertex_index = 1; vertex_index < s.vert_count; vertex_index++) {
                    v2 vertex = s.vertices[vertex_index];
                    f32 dist_along_d = dot(vertex, d);
                    if (dist_along_d > best_dist) {
                        result = vertex;
                        best_dist = dist_along_d;
                    }
                }
            }
        } break;

        case Shape_Circle: {
            result = normalize_or_zero(d)*s.radius;
        } break;
    }

    result = rotate(t.scale*result, t.rotation_arm) + t.offset;

    if (dot(rotate_clockwise(s.sweep, t.rotation_arm), d) > 0) {
        result += s.sweep;
    }

    return result;
}

inline v2 support(Transform2D t1, Shape2D s1, Transform2D t2, Shape2D s2, v2 d) {
    v2 a = get_furthest_point_along(t1, s1,  rotate_clockwise(d, t1.rotation_arm));
    v2 b = get_furthest_point_along(t2, s2, -rotate_clockwise(d, t2.rotation_arm));
    v2 p = a - b;
    return p;
}

inline void dbg_draw_shape(Transform2D transform, Shape2D shape, v4 color = vec4(1, 1, 1, 1)) {
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

inline void dbg_draw_shape_with_brute_force(Transform2D transform, Shape2D shape, v4 color = vec4(1, 1, 1, 1)) {
    glBegin(GL_LINE_LOOP);
    glColor4fv(color.e);

    f32 quality = 1024.0f;
    v2 previous_p = vec2(FLT_MAX, FLT_MAX);
    for (f32 segment_angle = 0; segment_angle < TAU_32; segment_angle += (TAU_32 / quality)) {
        v2 p = get_furthest_point_along(transform, shape, arm2(segment_angle));
        if (!vectors_equal(p, previous_p)) {
            previous_p = p;
            glVertex2fv(p.e);
        }
    }

    glEnd();
}

inline void dbg_draw_minkowski_difference_with_brute_force(Transform2D t1, Shape2D s1, Transform2D t2, Shape2D s2, v4 color = vec4(1, 1, 1, 1)) {
    glBegin(GL_LINE_LOOP);
    glColor4fv(color.e);

    f32 quality = 4096.0f;
    v2 previous_p = vec2(FLT_MAX, FLT_MAX);
    for (f32 segment_angle = 0; segment_angle < TAU_32; segment_angle += (TAU_32 / quality)) {
        v2 p = support(t1, s1, t2, s2, arm2(segment_angle));
        if (!vectors_equal(p, previous_p)) {
            previous_p = p;
            p += t1.offset;
            glVertex2fv(p.e);
        }
    }

    glEnd();
}

inline v2 triple_product(v2 a, v2 b, v2 c) {
    // @TODO: Work through the proof that this equals AxBxC
    v2 result = b*dot(c, a) - a*(dot(c, b));
    return result;
}

inline b32 gjk_do_simplex(u32* pc, v2* p, v2* d) {
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

struct EpaPoint {
    v2 p;
    EpaPoint* next;
};

struct EpaEdge {
    EpaPoint* point;
    v2 normal;
    f32 distance;
};

inline EpaEdge epa_find_closest_edge(EpaPoint* p_sentinel, b32 simplex_wound_ccw) {
    EpaEdge result = {};
    result.distance = FLT_MAX;

    for (EpaPoint* p = p_sentinel; p; p = p->next) {
        EpaPoint* next_p = p->next ? p->next : p_sentinel;
        v2 a = p->p;
        v2 b = next_p->p;
        v2 e = b - a;
        v2 n = simplex_wound_ccw ? perp(e) : perp_clockwise(e);
        n = normalize_or_zero(n);
        f32 d = dot(n, a);
        if (d < result.distance) {
            result.point = p;
            result.normal = n;
            result.distance = d;
        }
    }

    return result;
}

struct CollisionInfo {
    v2 vector;
    f32 depth;
};

inline b32 gjk_intersect(Transform2D t1, Shape2D s1, Transform2D t2, Shape2D s2, CollisionInfo* info = 0, MemoryArena* temp_arena = 0) {
    b32 result = false;

#if DEBUG_GJK_VISUALIZATION
    v4 viz_color = vec4(1, 0, 1, 1);
#endif

    TemporaryMemory temp = begin_temporary_memory(temp_arena);

    u32 pc = 0;
    v2 p[3] = {};
    v2 s = support(t1, s1, t2, s2, vec2(0, 1));

    p[pc++] = s;
    v2 d = -s;
    for (;;) {
        v2 a = support(t1, s1, t2, s2, d);
        if (dot(a, d) <= 0) {
            break;
        }
        p[pc++] = a;

#if DEBUG_GJK_VISUALIZATION
        Shape2D dbg_shape = polygon(pc, p);
        Transform2D dbg_transform = default_transform2d();
        dbg_transform.offset = t1.offset;
        dbg_draw_shape(dbg_transform, dbg_shape);
#endif

        if (gjk_do_simplex(&pc, p, &d)) {
            result = true;
            if (!info || pc == 3) {
                break;
            }
        }
    }

    if (info && result) {
        // @Note: EPA
        assert(pc == 3);
        assert(temp_arena);

        // @TODO: Check if this is actually correct and works!!!!!!
        b32 simplex_wound_ccw = (((p[1].x - p[0].x)*(p[2].y - p[1].y)) - ((p[1].y - p[0].y)*(p[2].x-p[1].x))) > 0.0f;
#if DEBUG_GJK_VISUALIZATION
        viz_color = simplex_wound_ccw ? viz_color : vec4(0, 1, 1, 1);
#endif

        u32 iter_count = 0;
        EpaPoint* epa_p = push_array(temp_arena, 3, EpaPoint, no_clear());
        epa_p[0].p = p[0];
        epa_p[0].next = &epa_p[1];
        epa_p[1].p = p[1];
        epa_p[1].next = &epa_p[2];
        epa_p[2].p = p[2];
        epa_p[2].next = 0;

        f32 epsilon = 1.0e-3f;
        for (;;) {
            if (iter_count > 4) {
                int y = 0;
            }
            EpaEdge e = epa_find_closest_edge(epa_p, simplex_wound_ccw);
            v2 s = support(t1, s1, t2, s2, e.normal);
            f32 d = dot(s, e.normal);
            if ((d - e.distance) < epsilon) {
                info->vector = e.normal;
                info->depth = d;
                break;
            } else {
                EpaPoint* new_p = push_struct(temp_arena, EpaPoint, no_clear());
                new_p->p = s;
                new_p->next = e.point->next;
                e.point->next = new_p;
            }
            iter_count++;
        }

#if DEBUG_GJK_VISUALIZATION
        glBegin(GL_LINE_LOOP);
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        for (EpaPoint* test_p = epa_p; test_p; test_p = test_p->next) {
            v2 vert = test_p->p + t1.offset;
            glVertex2fv(vert.e);
        }
        glEnd();
#endif
    }

#if DEBUG_GJK_VISUALIZATION
    dbg_draw_minkowski_difference_with_brute_force(t1, s1, t2, s2, vec4(1, 0, 1, 1));
#endif


    end_temporary_memory(temp);

    return result;
}

inline Shape2D dbg_convex_hull(Shape2D source_shape, MemoryArena* perm_arena) {
    u32 vert_count = source_shape.vert_count;
    v2* vertices = source_shape.vertices;

    u32 leftmost_index = 0;
    {
        v2 dir = vec2(-1, 0);
        f32 best_dist = dot(vertices[0], dir);
        for (u32 vertex_index = 1; vertex_index < vert_count; vertex_index++) {
            v2 vertex = vertices[vertex_index];
            f32 dist_along_dir = dot(vertex, dir);
            if (dist_along_dir > best_dist) {
                leftmost_index = vertex_index;
                best_dist = dist_along_dir;
            }
        }
    }

    u32 start_index = leftmost_index;
    u32 end_index = 0;
    // @Note: size 1 array to start, we will grow it dynamically. @TODO: Should have some kind of official mechanism for this?
    v2* result_vertices = push_struct(perm_arena, v2);
    result_vertices[0] = vertices[start_index];
    Shape2D result = polygon(1, result_vertices);
    do {
        for (u32 test_index = 0; test_index < vert_count; test_index++) {
            v2 test_point = vertices[test_index];
            v2 test_line = test_point - vertices[start_index];
            v2 line_perp = perp(vertices[end_index] - vertices[start_index]);
            if ((start_index == end_index) || dot(line_perp, test_line) > 0.0f) {
                end_index = test_index;
            }
        }

        // @TODO: Again, a bit janky. Need some kind of memory arena dynamic
        // array mechanism?
        *push_struct(perm_arena, v2, align_no_clear(1)) = vertices[end_index];
        result.vert_count++;

        start_index = end_index;
    } while (end_index != leftmost_index && result.vert_count < vert_count);

    return result;
}

inline Shape2D dbg_minkowski_sum(Shape2D s1, Shape2D s2, MemoryArena* perm_arena, MemoryArena* temp_arena) {
    assert(s1.type == Shape_Polygon && s2.type == Shape_Polygon);

    TemporaryMemory temp_mem = begin_temporary_memory(temp_arena);

    u32 max_vert_count = s1.vert_count*s2.vert_count;
    Shape2D raw_shape = polygon(max_vert_count, push_array(temp_arena, max_vert_count, v2));
    v2* dest = raw_shape.vertices;
    for (u32 vert_index1 = 0; vert_index1 < s1.vert_count; vert_index1++) {
        v2 vert1 = s1.vertices[vert_index1];
        for (u32 vert_index2 = 0; vert_index2 < s2.vert_count; vert_index2++) {
            v2 vert2 = s2.vertices[vert_index2];
            *dest++ = vert1 + vert2;
        }
    }

    Shape2D result = dbg_convex_hull(raw_shape, perm_arena);

    end_temporary_memory(temp_mem);

    return result;
}

inline void dbg_draw_arrow(v2 start, v2 end, v4 color = vec4(1, 1, 1, 1)) {
    v2 dir = end - start;
    v2 dir_perp = perp(dir);
    v2 fin = 0.1f*dir_perp;
    v2 neck = start + 0.9f*dir;
    v2 arrow_verts[] = { start, neck, neck + fin, end, neck - fin, neck };
    Shape2D arrow;
    arrow.vert_count = ARRAY_COUNT(arrow_verts);
    arrow.vertices = arrow_verts;
    dbg_draw_shape(default_transform2d(), arrow, color);
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
            player->rotation += TAU_32/100.0f;
        }
        if (keyboard->move_right.is_down) {
            player->rotation -= TAU_32/100.0f;
        }
        v2 x_axis = arm2(player->rotation);
        v2 y_axis = perp(x_axis);
        if (keyboard->move_down.is_down) {
            player->p -= 6.0f*y_axis;
        }
        if (keyboard->move_up.is_down) {
            player->p += 6.0f*y_axis;
        }
        if (keyboard->action_left.is_down) {
            game_state->rotation += TAU_32/100.0f;
        }
        if (keyboard->action_right.is_down) {
            game_state->rotation -= TAU_32/100.0f;
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

    v2 vertices[] = { vec2(-1.0f, -1.0f), vec2(1.0f, -1.0f), vec2(0.0f, 1.0f) };
    Shape2D shape = polygon(ARRAY_COUNT(vertices), vertices);

    Transform2D transform = default_transform2d();
    transform.offset = vec2(512, 512);
    transform.scale = 100.0f;
    transform.rotation_arm = arm2(game_state->rotation);

    Transform2D player_transform = default_transform2d();
    player_transform.offset = player->p;
    player_transform.scale = 50.0f;
    player_transform.rotation_arm = arm2(player->rotation);

    Shape2D swept_shape = shape; // circle(1.0f, vec2(20.5f, 50.0f));
    swept_shape.sweep = mouse_p - transform.offset; // vec2(100.0f, 200.0f);

    v4 color = vec4(1, 0, 0, 1);
    CollisionInfo collision_info;
    if (gjk_intersect(transform, swept_shape, player_transform, shape, &collision_info, &game_state->transient_arena)) {
        color = vec4(0, 1, 0, 1);
        player->p += collision_info.vector*collision_info.depth;
        player_transform.offset = player->p;
    }

    dbg_draw_shape_with_brute_force(transform, swept_shape, color);
    dbg_draw_shape(player_transform, shape, color);

    v2 x_axis = player_transform.rotation_arm;
    v2 y_axis = perp(x_axis);
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
