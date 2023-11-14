#ifndef CAMERA_H
#define CAMERA_H
#include "vector.h"

typedef struct { 
    vec3_t position;
    vec3_t direction;
    vec3_t forward_velocity;
    float yaw; // rotation around y!
    float pitch; // rotation around x.
} camera_t;


vec3_t get_camera_position();
void set_camera_position(vec3_t position);
void set_camera_direction(vec3_t direcition);
vec3_t get_camera_direction();


vec3_t get_camera_forward_velocity();
void set_camera_forward_velocity(vec3_t forward_velocity);
float get_camera_yaw();
void set_camera_yaw(float yaw);

void set_camera_pitch(float pitch);
float get_camera_pitch(void);


#endif