#ifndef CLIPPING_H
#define CLIPPING_H
#include "vector.h"
#include <stdbool.h>

enum {
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE
};

typedef struct {
    vec3_t point;
    vec3_t normal;
} plane_t;

void init_frustrum_planes(float fov, float z_near, float z_far);

bool point_inside_plane(vec3_t point, plane_t plane);

#endif