#include "matrix.h"

mat4_t mat4_identity() {
    mat4_t mat4 = {0};
    mat4.m[0][0] = 1;
    mat4.m[1][1] = 1;
    mat4.m[2][2] = 1;
    mat4.m[3][3] = 1;
    
    return mat4;
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
    mat4_t mat4 = mat4_identity();

    mat4.m[0][0] = sx;
    mat4.m[1][1] = sy;
    mat4.m[2][2] = sz;

    return mat4;
}

mat4_t mat4_make_rotation_x(float angle) {
    mat4_t mat4 = mat4_identity();
    mat4.m[1][1] = cos(angle);
    mat4.m[1][2] = -sin(angle);
    mat4.m[2][1] = sin(angle);
    mat4.m[2][2] = cos(angle);

    return mat4;
}

// notice how the sign changes here: this is necesssary to keep the rotation in the correct direction (???)
mat4_t mat4_make_rotation_y(float angle) {
    mat4_t mat4 = mat4_identity();
    mat4.m[0][0] = cos(angle);
    mat4.m[0][2] = sin(angle);
    mat4.m[2][0] = -sin(angle);
    mat4.m[2][2] = cos(angle);
    
    return mat4;
}


mat4_t mat4_make_rotation_z(float angle) {
    mat4_t mat4 = mat4_identity();
    mat4.m[0][0] = cos(angle);
    mat4.m[0][1] = -sin(angle);
    mat4.m[1][0] = sin(angle);
    mat4.m[1][1] = cos(angle);

    return mat4;
}



mat4_t mat4_make_translate(float tx, float ty, float tz) {
    mat4_t mat4 = mat4_identity();

    mat4.m[0][3] = tx;
    mat4.m[1][3] = ty;
    mat4.m[2][3] = tz;

    return mat4;
}



vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
    vec4_t result = {0};

    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z   + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z   + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z   + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z   + m.m[3][3] * v.w;

    return result;
}
