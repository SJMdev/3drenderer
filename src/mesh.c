#include <stdio.h>
#include "mesh.h"
#include "array.h"
#include "texture.h"

#define MAX_MESH_COUNT 10
static mesh_t meshes[MAX_MESH_COUNT];
static int active_mesh_count = 0;

int get_mesh_count(void) {
    return active_mesh_count;
}

mesh_t* get_mesh(int idx) {
    return &meshes[idx];
}

void load_mesh(char* obj_filename, char* png_filename, vec3_t scale, vec3_t translation, vec3_t rotation) {
    // load the png file information to mesh texture
    // initialize scale, translation and rotation 
    mesh_t* mesh = &meshes[active_mesh_count];
    // load the obj file to our mesh
    load_mesh_obj_data(obj_filename, mesh);
    load_mesh_png_data(png_filename, mesh);

    mesh->scale = scale;
    mesh->translation = translation;
    mesh->rotation = rotation;

    active_mesh_count += 1;

}

void load_mesh_png_data(char* filename, mesh_t* mesh) {
    upng_t* png_image = upng_new_from_file(filename);
    if (png_image != NULL) {
        upng_decode(png_image);
        if (upng_get_error(png_image) == UPNG_EOK) {
            mesh->texture = png_image;
        } 
    }
}

void load_mesh_obj_data(char* obj_filename, mesh_t* mesh) {
    // todo: read the contents of the obj file
    // and load the vertices and faces in our mesh.vertices and mesh.faces
    FILE *file;

    // Open the file for reading
    file = fopen(obj_filename, "r");

    // Check if the file was opened successfully
    if (file == NULL) {
        printf("Unable to open the file %s. \n", obj_filename);
        return;
    }

    // Buffer to store each line
    char line[1024]; // You can adjust the size as needed
    tex2_t* texture_coordinates = NULL;

    // Read the file line by line
    while (fgets(line, sizeof(line), file) != NULL) {
        
        // Variables to store parsed data
        vec3_t position;
        face_t face;
        int vertex_indices[3];
        int texture_indices[3];
        int normal_indices[3];
        tex2_t vertex_uv;

        // Use sscanf to parse the line
        if (sscanf(line, "v %f %f %f", &position.x, &position.y, &position.z) == 3) {
            // Process the parsed data
            printf("x: %f, y: %f, z: %f\n", position.x, position.y, position.z);
            array_push(mesh->vertices, position);

        } else if (sscanf(line, "vt %f %f", &vertex_uv.u,  &vertex_uv.v) == 2) {
            array_push(texture_coordinates, vertex_uv);
        }else if (sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", 
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
            face.a = vertex_indices[0] - 1;
            face.b = vertex_indices[1] - 1;
            face.c = vertex_indices[2] - 1;
            face.a_uv = texture_coordinates[texture_indices[0] -1];
            face.b_uv = texture_coordinates[texture_indices[1] -1];
            face.c_uv = texture_coordinates[texture_indices[2] -1];

            face.color = 0xFFFFFFFF;


            array_push(mesh->faces, face);


        } else {
            // printf("Failed to parse the line: %s\n", line);
        }
    }
    array_free(texture_coordinates);

    // Close the file
    fclose(file);
}

void free_meshes(void) {

    for (int mesh_idx = 0; mesh_idx != active_mesh_count; ++mesh_idx) {
        upng_free(meshes[mesh_idx].texture);
        array_free(meshes[mesh_idx].faces);
        array_free(meshes[mesh_idx].vertices);
    }
    
}