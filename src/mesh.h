#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

#define MESH_VERTICES_COUNT 8
extern vec3_t mesh_vertices[MESH_VERTICES_COUNT];

#define MESH_FACES_COUNT ( 6 * 2) // 6 cube faces, 2 triangles per face
extern face_t mesh_faces[MESH_FACES_COUNT];

# endif