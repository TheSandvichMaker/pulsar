#ifndef PULSAR_GJK_H
#define PULSAR_GJK_H

struct CollisionInfo {
    v2 vector; // @TODO: Figure out exactly what determines the direction this vector faces
    f32 depth;
};

#endif /* PULSAR_GJK_H */
