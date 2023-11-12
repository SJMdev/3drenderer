#include "matrix.h"
#include <math.h>

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

// note that this does not do perspective divide. we are storing the original z value inside w. (with the 1.0)
 mat4_t mat4_make_perspective(float fov, float aspect_ratio, float z_near, float z_far) {
    mat4_t mat4 = {{{ 0}}};

    mat4.m[0][0] = aspect_ratio * (1 / tan(fov / 2.0));
    mat4.m[1][1] = 1 / tan(fov / 2.0);
    mat4.m[2][2] = z_far / (z_far - z_near);
    mat4.m[2][3] = (-z_far * z_near) / (z_far - z_near);
    mat4.m[3][2] = 1.0;

    return mat4;
}

mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up) {
    // since we move the camera to the origin, all other vertices in the scene
    // have to be translated by (-x, -y, -z).
    // furthermore, we need to undo the camera "orientation" by multiplying by the inverse
    // rotation.
    // the rotation matrix is normally defined by the coordinate system three column vectors of x,y,z.
    // however, we need to invert the rotation matrix because we need to undo.
    
    //@NOTE(SJM): this is actually just speculation, but whatever
    // this inversion does not apply to the translation matrix because it is easy to invert it by just 
    // multiplying by minus one.

    vec3_t z = vec3_sub(target, eye);
    vec3_normalize(&z); // forward
    vec3_t x = vec3_cross(up, z);
    vec3_normalize(&x); // right 
    vec3_t y = vec3_cross(z,x); // up!

    // x.x, x.y, x.z,  -dot(x, eye),
    // y.x, y.y, y.z   -dot(y, eye),
    // z.x, z.y, z.z   -dot(z, eye),
    //  0   0     0          1


    mat4_t view_matrix = {{
        {x.x, x.y, x.z, -vec3_dot(x, eye) },
        {y.x, y.y, y.z, -vec3_dot(y, eye)},
        {z.x, z.y, z.z, -vec3_dot(z, eye)},
        {0,   0,   0,    1}
    }};

    return view_matrix;
}

    
vec4_t mat4_mul_vec4_project(mat4_t projection_matrix, vec4_t v) {
    vec4_t result = mat4_mul_vec4(projection_matrix, v);
    // perform perspective divide with original z value.
    if (result.w != 0.0) {
        result.x = result.x / result.w;
        result.y = result.y / result.w;
        result.z = result.z / result.w;
    }

    return result;
}



vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
    vec4_t result = {0};

    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z   + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z   + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z   + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z   + m.m[3][3] * v.w;

    return result;
}

mat4_t mat4_mul_mat4(mat4_t lhs, mat4_t rhs) {
    mat4_t result = mat4_identity();
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j <4; ++j) {
            result.m[i][j] = lhs.m[i][0] * rhs.m[0][j] +  lhs.m[i][1] * rhs.m[1][j] + lhs.m[i][2] * rhs.m[2][j] + lhs.m[i][3] * rhs.m[3][j];
        }
    }
    return result;
}

