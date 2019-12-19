#include "game_main.h"

#include "asset_loading.h"
#include "asset_loading.cpp"

#include "game_assets.cpp"
#include "audio_mixer.cpp"

#define DEBUG_GJK_VISUALIZATION 0

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

    if (dot(rotate_clockwise(t.sweep, t.rotation_arm), d) > 0) {
        result += t.sweep;
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
    v2 vector; // @TODO: Figure out exactly what determines the direction this vector faces
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

    // @TODO: Add "core shapes" GJK collision info collection for shallow
    // intersections.

    if (info && result) {
        // @Note: EPA
        assert(pc == 3);
        assert(temp_arena);

        b32 simplex_wound_ccw = (((p[1].x - p[0].x)*(p[2].y - p[1].y)) - ((p[1].y - p[0].y)*(p[2].x-p[1].x))) > 0.0f;
#if DEBUG_GJK_VISUALIZATION
        viz_color = simplex_wound_ccw ? viz_color : vec4(1, 1, 0, 1);
#endif

        // @TODO: If I get to it, profile the performance difference between
        // this linked list version and some variety of a dynamic array version
        EpaPoint* epa_p = push_array(temp_arena, 3, EpaPoint, no_clear());
        epa_p[0].p = p[0];
        epa_p[0].next = epa_p + 1;
        epa_p[1].p = p[1];
        epa_p[1].next = epa_p + 2;
        epa_p[2].p = p[2];
        epa_p[2].next = 0;

        f32 epsilon = 1.0e-3f;
        for (;;) {
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
    dbg_draw_minkowski_difference_with_brute_force(t1, s1, t2, s2, viz_color);
#endif

    end_temporary_memory(temp);

    return result;
}

#if 0
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
#endif

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

inline u32 add_player(GameState* game_state, v2 starting_p) {
    assert(game_state->entity_count < ARRAY_COUNT(game_state->entities));

    u32 entity_index = game_state->entity_count++;
    Entity* entity = game_state->entities + entity_index;
    entity->type = EntityType_Player;
    entity->p = starting_p;
    entity->collision = game_state->player_collision;
    entity->color = vec4(1, 1, 1, 1);

    entity->flags |= EntityFlag_Moveable|EntityFlag_Collides;

    return entity_index;
}

inline u32 add_wall(GameState* game_state, Rect2 rect) {
    assert(game_state->entity_count < ARRAY_COUNT(game_state->entities));

    u32 entity_index = game_state->entity_count++;
    Entity* entity = game_state->entities + entity_index;
    entity->type = EntityType_Wall;
    entity->p = get_center(rect);
    entity->color = vec4(1, 1, 1, 1);
    entity->surface_friction = 5.0f;

    entity->flags |= EntityFlag_Collides;

    rect = offset(rect, -entity->p);

    // @TODO: Stop wasting memory on rectangular polygons
    v2* verts = push_array(&game_state->permanent_arena, 4, v2);
    verts[0] = { rect.min.x, rect.min.y };
    verts[1] = { rect.max.x, rect.min.y };
    verts[2] = { rect.max.x, rect.max.y };
    verts[3] = { rect.min.x, rect.max.y };
    entity->collision = polygon(4, verts);

    return entity_index;
}

#define MAX_COLLISION_ITERATIONS 4
inline void move_entity(GameState* game_state, Entity* entity, v2 ddp, f32 dt) {
    /*
     * @TODO: Figure out some of the cases in which the collision handling will
     * fail, which I'm sure there are some, such as _very_ extreme velocities
     * (which might happen in cases of frame drops if I choose to support
     * dynamic timesteps). One suspicion I have is that if an entity penetrates
     * deep enough into another, the collision might resolve to put it on the other
     * side, allowing very fast entities to clip through each other.
     */

    f32 epsilon = 0.01f;

    // @TODO: Some kind of relationship with real world units, get away from using pixels.
    f32 air_resistance = 0.0f;
    f32 gravity = 300.0f;
    ddp -= air_resistance*entity->dp;
    ddp.y -= gravity;

    if (entity->flags & EntityFlag_OnGround) {
        ddp.x -= entity->friction_of_last_touched_surface*entity->dp.x;
        f32 off_ground_time = 0.1f;
        if (entity->off_ground_timer < off_ground_time) {
            entity->off_ground_timer += dt;
        } else {
            entity->flags &= ~EntityFlag_OnGround;
        }
    }

    v2 delta = 0.5f*ddp*square(dt) + entity->dp*dt;
    entity->dp += ddp*dt;

    entity->friction_of_last_touched_surface = 0.0f;

    if (entity->flags & EntityFlag_Collides) {
        Transform2D t = transform2d(entity->p, delta);
        // @TODO: Optimized spatial indexing of sorts?
        for (u32 entity_index = 0; entity_index < game_state->entity_count; entity_index++) {
            Entity* test_entity = game_state->entities + entity_index;
            if (entity != test_entity) {
                entity->friction_of_last_touched_surface = test_entity->surface_friction;
                Transform2D test_t = transform2d(test_entity->p);
                if (test_entity->flags & EntityFlag_Collides) {
                    test_entity->sticking_entity = 0;
                    u32 iteration = 0;
                    do {
                        CollisionInfo collision;
                        if (gjk_intersect(t, entity->collision, test_t, test_entity->collision, &collision, &game_state->transient_arena)) {
                            if (collision.vector.y < -0.707f) {
                                entity->flags |= EntityFlag_OnGround;
                                entity->off_ground_timer = 0.0f;
                                test_entity->sticking_entity = entity;
                            }
                            f32 depth = collision.depth + epsilon;
                            f32 theta_times_length_of_delta = dot(collision.vector, delta);
                            if (theta_times_length_of_delta == 0) {
                                delta = -collision.vector*depth;
                                break;
                            } else {
                                f32 penetration_along_delta = depth / theta_times_length_of_delta;

                                v2 legal_move = delta*(1.0f-penetration_along_delta);
                                delta = delta*penetration_along_delta;
                                delta -= collision.vector*dot(delta, collision.vector);

                                entity->p += legal_move;
                                entity->dp -= collision.vector*dot(entity->dp, collision.vector);
                            }
                        } else {
                            break;
                        }

                        t.offset = entity->p;
                        t.sweep = delta;

                        iteration++;
                    } while (iteration < MAX_COLLISION_ITERATIONS && length_sq(delta) > epsilon);
                }
            }
        }
    }

    entity->p += delta;
}

internal GAME_UPDATE_AND_RENDER(game_update_and_render) {
    assert(memory->permanent_storage_size >= sizeof(GameState));

    platform = memory->platform_api;

    //
    // Initialization
    //

    GameState* game_state = cast(GameState*) memory->permanent_storage;
    if (!memory->initialized) {
        initialize_arena(&game_state->permanent_arena, memory->permanent_storage_size - sizeof(GameState), cast(u8*) memory->permanent_storage + sizeof(GameState));
        initialize_arena(&game_state->transient_arena, memory->transient_storage_size, memory->transient_storage);

        initialize_audio_mixer(&game_state->audio_mixer, &game_state->permanent_arena);

        load_assets(&game_state->assets, &game_state->transient_arena, "assets.pak");

        game_state->test_music = get_sound_by_name(&game_state->assets, "test_music");
        game_state->test_sound = get_sound_by_name(&game_state->assets, "test_sound");
        game_state->test_image = get_image_by_name(&game_state->assets, "test_image");

        game_state->test_image->handle = cast(void*) opengl_load_texture(
            &opengl_info,
            game_state->test_image->w,
            game_state->test_image->h,
            game_state->test_image->pixels
        );

        v2* square_verts = push_array(&game_state->permanent_arena, 4, v2);
        square_verts[0] = { -10.0f,  0.0f };
        square_verts[1] = {  10.0f,  0.0f };
        square_verts[2] = {  10.0f, 50.0f };
        square_verts[3] = { -10.0f, 50.0f };
        game_state->player_collision = polygon(4, square_verts);

        // @Note: I'm reserving the 0th entity index to signify the null entity.
        game_state->entity_count = 1;

        add_player(game_state, vec2(512, 516));
        add_wall(game_state, rect_center_dim(vec2(512, 500), vec2(512, 24)));

        // play_sound(&game_state->audio_mixer, game_state->test_music);

        memory->initialized = true;
    }

    v2 mouse_p = vec2(input->mouse_x, input->mouse_y);

    //
    // Gameplay Logic
    //

    for (u32 entity_index = 1; entity_index < game_state->entity_count; entity_index++) {
        Entity* entity = game_state->entities + entity_index;
        assert(entity->type != EntityType_Null);

        entity->ddp = vec2(0, 0);
        entity->was_on_ground = entity->flags & EntityFlag_OnGround;

        switch (entity->type) {
            case EntityType_Player: {
                if (entity->p.x < 0.0f) {
                    entity->p.x += width;
                } else if (entity->p.x >= width) {
                    entity->p.x -= width;
                }
                if (entity->p.y < 0.0f) {
                    entity->p.y += height;
                } else if (entity->p.y >= height) {
                    entity->p.y -= height;
                }
                GameController* controller = &input->controller;
                f32 move_speed = (entity->flags & EntityFlag_OnGround) ? 1000.0f : 200.0f;
                if (controller->move_left.is_down) {
                    entity->ddp.x -= move_speed;
                }
                if (controller->move_right.is_down) {
                    entity->ddp.x += move_speed;
                }

                if (entity->was_on_ground && was_pressed(controller->move_up)) {
                    play_sound(&game_state->audio_mixer, game_state->test_sound);
                    entity->ddp.y += 10000.0f;
                    entity->flags &= ~EntityFlag_OnGround;
                }
            } break;

            case EntityType_Wall: {
                v2 movement = vec2(2.0f*cos(entity->movement_t), 0.0f /* sin(entity->movement_t) */);
                entity->p += movement;
                entity->movement_t += input->frame_dt;
                Entity* sticking_entity = entity->sticking_entity;
                if (sticking_entity) {
                    sticking_entity->p += movement;
                    sticking_entity->sticking_dp = movement / input->frame_dt;
                }
            } break;

            INVALID_DEFAULT_CASE;
        }
    }

    //
    // Physics & Rendering
    //

    glViewport(0, 0, width, height);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    opengl_set_screenspace(width, height);
    for (u32 entity_index = 1; entity_index < game_state->entity_count; entity_index++) {
        Entity* entity = game_state->entities + entity_index;
        assert(entity->type != EntityType_Null);

        opengl_rectangle(rect_center_dim(entity->p + entity->sticking_dp, vec2(2, 2)), vec4(1, 0, 0, 1));
        if (entity->was_on_ground && !(entity->flags & EntityFlag_OnGround)) {
            entity->dp += entity->sticking_dp;
            entity->sticking_dp = vec2(0, 0);
        }

        if (entity->flags & EntityFlag_Moveable) {
            move_entity(game_state, entity, entity->ddp, input->frame_dt);
        }

        Transform2D transform = default_transform2d();
        transform.offset = entity->p;
        dbg_draw_shape(transform, entity->collision, entity->color);

        entity->color = vec4(1, 1, 1, 1);
    }

    glDisable(GL_BLEND);

    check_arena(&game_state->permanent_arena);
    check_arena(&game_state->transient_arena);
}

internal GAME_GET_SOUND(game_get_sound) {
    assert(memory->initialized);
    GameState* game_state = cast(GameState*) memory->permanent_storage;

    output_playing_sounds(&game_state->audio_mixer, sound_buffer, &game_state->transient_arena);
}
