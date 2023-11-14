#include "camera.h"

static camera_t camera = {
    .position = {0.0,0.0,0.0},
    .direction = {0.0, 0.0, 1.0},
    .forward_velocity = {0.0, 0.0, 0.0},
    .yaw = 0,
    .pitch = 0
};



vec3_t get_camera_position() {
    return camera.position;
}
void set_camera_position(vec3_t position) {
    camera.position = position;
}
void set_camera_direction(vec3_t direction) {
    camera.direction = direction;
}

vec3_t get_camera_direction() {
    return camera.direction;
}

vec3_t get_camera_forward_velocity() {
    return camera.forward_velocity;
}
void set_camera_forward_velocity(vec3_t forward_velocity) {
    camera.forward_velocity = forward_velocity;
}
float get_camera_yaw() {
    return camera.yaw;
}
void set_camera_yaw(float yaw) {
    camera.yaw = yaw;
 }

 float get_camera_pitch() {
    return camera.pitch;
}
void set_camera_pitch(float pitch) {
    camera.pitch = pitch;
 }