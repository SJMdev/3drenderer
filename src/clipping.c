#include "clipping.h"
#include <math.h>


#define PLANE_COUNT 6
plane_t frustum_planes[PLANE_COUNT];

void init_frustrum_planes(float fov, float z_near, float z_far) {
    float cos_half_fov = cos(fov / 2.0);
    float sin_half_fov = sin(fov / 2.0);

    vec3_t origin = {0.0,0.0,0.0};

    frustum_planes[LEFT_FRUSTUM_PLANE].point = origin;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fov;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fov;

    frustum_planes[RIGHT_FRUSTUM_PLANE].point = origin;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov;
    
    frustum_planes[TOP_FRUSTUM_PLANE].point = origin;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0.0;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov;;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov;
    
    frustum_planes[BOTTOM_FRUSTUM_PLANE].point = origin;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov;
    
    frustum_planes[NEAR_FRUSTUM_PLANE].point = vec3_new(0.0, 0.0, z_near);
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;
    
    frustum_planes[FAR_FRUSTUM_PLANE].point = vec3_new(0.0, 0.0, z_far);
    frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}

bool point_inside_plane(vec3_t point, plane_t plane) {

    vec3_t vector = vec3_sub(point, plane.point);
    vec3_normalize(&vector);

    float dot = vec3_dot(plane.normal, vector);

    if (dot > 0.0) return true;

    return false; 
}

// vec3_t find_intersection(vec3_t point_inside, vec3_t point_outside, plane_t plane) {


// }