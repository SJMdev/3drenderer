#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include "upng.h"

typedef struct {
    vec3_t* vertices; // dynamic array of vertices
    face_t* faces; // dynamic array of faces
    upng_t* texture; // mesh PNG texture pointer.
    vec3_t rotation; // mesh rotation xyz
    vec3_t scale; // scale with xyz values
    vec3_t translation;  // mesh translation with x,y, and z values

} mesh_t;

void load_mesh(char* obj_filename, char* png_filename, vec3_t scale, vec3_t translation, vec3_t rotation);
void load_mesh_obj_data(char* filename, mesh_t* mesh);
void load_mesh_png_data(char* filename, mesh_t* mesh);

int get_mesh_count();
mesh_t* get_mesh(int idx);
void free_meshes(void);


# endif