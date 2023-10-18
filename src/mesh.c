#include <stdio.h>
#include "mesh.h"
#include "array.h"

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = {0,0,0}
};

vec3_t cube_vertices[CUBE_VERTICES_COUNT] = {
    {.x = -1, .y = -1, .z = -1}, // 1
    {.x = -1, .y =  1, .z = -1}, // 2
    {.x =  1, .y =  1, .z = -1}, // 3
    {.x =  1, .y = -1, .z = -1}, // 4
    {.x =  1, .y =  1, .z =  1}, // 5
    {.x =  1, .y = -1, .z =  1}, // 6
    {.x = -1, .y =  1, .z =  1}, // 7
    {.x = -1, .y = -1, .z =  1}, // 8
};

face_t cube_faces[CUBE_FACES_COUNT] = {
    // front
    {.a = 1, .b = 2, .c = 3, .color = 0xFFFF0000},
    {.a = 1, .b = 3, .c = 4, .color = 0xFFFF0000},
    // right
    {.a = 4, .b = 3, .c = 5, .color = 0xFF00FF00},
    {.a = 4, .b =5 , .c = 6, .color = 0xFF00FF00},
    // back
    {.a = 6, .b = 5, .c = 7, .color = 0xFFFF00FF},
    {.a = 6, .b = 7, .c = 8, .color = 0xFFFF00FF},
    // left
    {.a = 8, .b = 7, .c = 2, .color = 0xFFFFFF00},
    {.a = 8, .b = 2, .c = 1, .color = 0xFFFFFF00},
    // top
    {.a = 2, .b = 7, .c = 5, .color = 0xFFFF00FF},
    {.a = 2, .b = 5, .c = 3, .color = 0xFFFF00FF},
    // bottom
    {.a = 6, .b = 8, .c = 1, .color = 0xFF00FFFF},
    {.a = 6, .b = 1, .c = 4, .color = 0xFF00FFFF}
}; 

void load_cube_mesh_data(void) {
    for (int idx = 0; idx != CUBE_VERTICES_COUNT; ++idx) {
        array_push(mesh.vertices, cube_vertices[idx]);
    }

    for (int idx = 0; idx != CUBE_FACES_COUNT; ++idx) {
        array_push(mesh.faces, cube_faces[idx]);
    }
}
void load_obj_file_data(char* filename) {
    // todo: read the contents of the obj file
    // and load the vertices and faces in our mesh.vertices and mesh.faces
    FILE *file;

    // Open the file for reading
    file = fopen(filename, "r");

    // Check if the file was opened successfully
    if (file == NULL) {
        printf("Unable to open the file %s. \n", filename);
        return;
    }

    // Buffer to store each line
    char line[1024]; // You can adjust the size as needed

    // Read the file line by line
    while (fgets(line, sizeof(line), file) != NULL) {
        
        // Variables to store parsed data
        vec3_t position;
        face_t face;
        int vertex_indices[3];
        int texture_indices[3];
        int normal_indices[3];

        // Use sscanf to parse the line
        if (sscanf(line, "v %f %f %f", &position.x, &position.y, &position.z) == 3) {
            // Process the parsed data
            printf("x: %f, y: %f, z: %f\n", position.x, position.y, position.z);
            array_push(mesh.vertices, position);

        } else if (sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", 
            &vertex_indices[0],
            &texture_indices[0],
            &normal_indices[0],
            &vertex_indices[1],
            &texture_indices[1],
            &normal_indices[1],
            &vertex_indices[2],
            &texture_indices[2],
            &normal_indices[2]) == 9){
           
            // printf("a0: %d, b0: %d, c0: %d\n", face_0.a, face_0.b, face_0.c);
            // printf("a1: %d, b1: %d, c2: %d\n", face_1.a, face_1.b, face_1.c);
            // printf("a2: %d, b2: %d, c2: %d\n", face_2.a, face_2.b, face_2.c);


            //@NOTE(SJM): we need to subtract all indices by 1 since we start at 1. 
            face.a = vertex_indices[0];
            face.b = vertex_indices[1];
            face.c = vertex_indices[2];


            array_push(mesh.faces, face);


        } else {
            // printf("Failed to parse the line: %s\n", line);
        }
    }

    // Close the file
    fclose(file);
}