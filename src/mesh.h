#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

#define CUBE_VERTICES_COUNT 8
extern vec3_t cube_vertices[CUBE_VERTICES_COUNT];

#define CUBE_FACES_COUNT ( 6 * 2) // 6 cube faces, 2 triangles per face
extern face_t cube_faces[CUBE_FACES_COUNT];


typedef struct {
    vec3_t* vertices; // dynamic array of vertices
    face_t* faces; // 
    vec3_t rotation;
    vec3_t scale;
    vec3_t translation; 
} mesh_t;

extern mesh_t mesh;

void load_cube_mesh_data(void);
void load_obj_file_data(char* filename);

# endif