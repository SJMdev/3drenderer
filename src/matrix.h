#ifndef MATRIX_H
#define MATRIX_H
#include "vector.h"

typedef struct {
    float m[4][4];
} mat4_t;

mat4_t mat4_identity();

mat4_t mat4_make_scale(float sx, float sy, float sz);
mat4_t mat4_make_translate(float tx, float ty, float tz);
mat4_t mat4_make_rotation_x(float angle);
mat4_t mat4_make_rotation_y(float angle);
mat4_t mat4_make_rotation_z(float angle);
mat4_t mat4_make_perspective( float fov,float aspect_ratio, float z_near, float z_far);
mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up);

vec4_t mat4_mul_vec4_project(mat4_t projection_matrix, vec4_t v);

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v);

mat4_t mat4_mul_mat4(mat4_t lhs, mat4_t rhs);

#endif // MATRIX_H