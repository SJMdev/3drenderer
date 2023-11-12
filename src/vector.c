#include "vector.h"


#include <math.h>



// implementations of 2D Vector functions
vec2_t vec2_new(float x, float y) {
    vec2_t result = {x,y};

    return result;
}

float vec2_length(vec2_t v) {
    float result = sqrtf(v.x * v.x + v.y * v.y); 
    return result;
}

vec2_t vec2_add(vec2_t lhs, vec2_t rhs) {
    vec2_t result = {
        .x = lhs.x + rhs.x,
        .y = lhs.y + rhs.y
    };
    return result;
}

vec2_t vec2_div(vec2_t lhs, float scalar) {
    vec2_t result = {
        .x = lhs.x / scalar,
        .y = lhs.y / scalar
    };

    return result;
}

vec2_t vec2_mul(vec2_t lhs, float scalar) {
    vec2_t result = {
        .x = lhs.x * scalar,
        .y = lhs.y * scalar
    };
    return result;
}
vec2_t vec2_sub(vec2_t lhs, vec2_t rhs) {
    vec2_t result = {
        .x = lhs.x - rhs.x,
        .y = lhs.y - rhs.y
    };
    return result;
}
 
float vec2_dot(vec2_t lhs, vec2_t rhs) { 
    float result = lhs.x * rhs.x + lhs.y * rhs.y;
    return result;
}

void vec2_normalize(vec2_t* v) {
    float length = vec2_length(*v);
    v->x = v->x / length;
    v->y = v->y / length;
}

vec2_t vec2_from_vec4(vec4_t v) {
    vec2_t result = {
        .x = v.x,
        .y = v.y
    };

    return result;
}


//-----------------------------------------
// implementations of 3D Vector functions

vec3_t vec3_clone(vec3_t* v) {
    vec3_t result = {
        v->x,
        v->y,
        v->z
    };

    return result;
}

vec3_t vec3_new(float x, float y, float z) {
    vec3_t result = {x,y,z};

    return result;
}

float vec3_length(vec3_t v) {
    float result = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    return result;
}

vec3_t vec3_add(vec3_t lhs, vec3_t rhs) {
    vec3_t result = {
        .x = lhs.x + rhs.x,
        .y = lhs.y + rhs.y,
        .z = lhs.z + rhs.z
    };

    return result;
}

vec3_t vec3_div(vec3_t lhs, float scalar) {
    vec3_t result = {
        .x = lhs.x / scalar,
        .y = lhs.y / scalar,
        .z = lhs.z / scalar
    };
    return result;
}

vec3_t vec3_mul(vec3_t lhs, float scalar) {
    vec3_t result = {
        .x = lhs.x * scalar,
        .y = lhs.y * scalar,
        .z = lhs.z * scalar
    };

    return result;
}

vec3_t vec3_sub(vec3_t lhs, vec3_t rhs) {
    vec3_t result = {
        .x = lhs.x - rhs.x,
        .y = lhs.y - rhs.y,
        .z = lhs.z - rhs.z
    };

    return result;
}

float vec3_dot(vec3_t lhs, vec3_t rhs) {
    float result = lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    return result;
}

vec3_t vec3_cross(vec3_t lhs, vec3_t rhs) {
    vec3_t result = {
        .x = (lhs.y * rhs.z) - (lhs.z * rhs.y),
        .y = (lhs.z * rhs.x) - (lhs.x * rhs.z),
        .z = (lhs.x * rhs.y) - (lhs.y * rhs.x)
    };

    return result;
}

void vec3_normalize(vec3_t* v)
{
    float length = vec3_length(*v);
    v->x = v->x / length;
    v->y = v->y / length;
    v->z = v->z / length;

}



vec3_t vec3_rotate_z(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        .z = v.z
    };
    return rotated_vector;
}

vec3_t vec3_rotate_y(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = v.x * cos(angle) - v.z * sin(angle),
        .y = v.y,
        .z = v.x * sin(angle) + v.z * cos(angle)
    };

    return rotated_vector;
}

vec3_t vec3_rotate_x(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle)
    };
    return rotated_vector;
}

vec3_t vec3_from_vec4(vec4_t v) {
    vec3_t result = {
        .x = v.x, 
        .y = v.y,
        .z = v.z
    };

    return result;
}

vec4_t vec4_from_vec3(vec3_t v) {
    vec4_t vec4 = {
        .x = v.x,
        .y = v.y,
        .z = v.z,
        .w = 1
    };

    return vec4;
}

// // x, y,z maps to alpha, beta, gamma
vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
    vec2_t ac = vec2_sub(c, a); 
    vec2_t ab = vec2_sub(b, a);
    vec2_t pc = vec2_sub(c, p);
    vec2_t pb = vec2_sub(b, p);
    vec2_t ap = vec2_sub(p, a);

    float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x); //  || AC x AB ||
    float alpha  = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc;
    float beta  = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;
    float gamma  = 1 - alpha - beta;

    vec3_t weights =  {
        .x = alpha,
        .y = beta,
        .z = gamma
    };

    return weights;
}


