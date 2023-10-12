#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

// We need to tell SDL that we are doing the main instead of SDL.
#define SDL_MAIN_HANDLED 
#include <SDL2/SDL.h>

#include "display.h"
#include "vector.h"
#include "mesh.h"

#define POINT_COUNT (9 * 9 *9) // 9x9x9 cube.

triangle_t triangles_to_render[MESH_FACES_COUNT];

vec3_t camera_position = {.x = 0, .y= 0, .z =-5};
vec3_t cube_rotation = {.x = 0, .y = 0, .z = 0};

float fov_factor = 640;
bool is_running = false;
int previous_frame_time = 0;

void setup() {
    // allocate the required bytes.
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

}
void process_input() {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT: // x button of window.
        {
            is_running = false;
            break;
        }
        case SDL_KEYDOWN:
        {
            if (event.key.keysym.sym == SDLK_ESCAPE){
                is_running = false;
                break;
            }
        }

        default:
            // fprintf(stderr, "unknown event type.");
            break;

    }
}

vec2_t project(vec3_t point) {
    vec2_t projected_point = {.x = (fov_factor *point.x) / point.z, .y = (fov_factor *point.y) / point.z};
    return projected_point;
}

void update() {

    int time_to_wait = FRAME_TARGET_TIME_MS - (SDL_GetTicks() - previous_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME_MS) {
        SDL_Delay(time_to_wait);
    }

    // how many miliseconds have passed since the last frame?
    previous_frame_time = SDL_GetTicks();

    cube_rotation.x += 0.001;
    cube_rotation.y += 0.001;
    cube_rotation.z += 0.001;


    // loop over faces?
    for (int face_idx = 0; face_idx < MESH_FACES_COUNT; ++face_idx) {
        face_t mesh_face = mesh_faces[face_idx];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh_vertices[mesh_face.a - 1]; // index displacement because face thing.
        face_vertices[1] = mesh_vertices[mesh_face.b - 1];
        face_vertices[2] = mesh_vertices[mesh_face.c - 1];

        triangle_t projected_triangle= {0};
        // for all three vertices 
        for (int vertex_idx = 0; vertex_idx <3; ++vertex_idx){
            vec3_t transformed_vertex = face_vertices[vertex_idx];
            transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z); //

            // translate the vertex away from the camera.
            transformed_vertex.z -= camera_position.z;

            // project the current vertex.
            vec2_t projected_point = project(transformed_vertex);
            // scale and translate the projected points to the middle of the screen.
            projected_point.x += (window_width  / 2);
            projected_point.y += (window_height / 2);

            projected_triangle.points[vertex_idx] = projected_point;
        }

        // save the projected triangle in the array of triangles to render.

        triangles_to_render[face_idx] = projected_triangle;
    }



}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
    SDL_RenderClear(renderer);

    draw_grid();

    for (int face_idx = 0; face_idx < MESH_FACES_COUNT; ++face_idx) {
        triangle_t triangle = triangles_to_render[face_idx];
        draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
        draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
        draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);


    }
    render_color_buffer();
    clear_color_buffer(0xff000000);

    SDL_RenderPresent(renderer);
}

void transform_points() {
    // rotate
    // scale
    // translate
}


int main(int argc, char *argv[]) {
    // create an SDL window.
    is_running = initialize_window();
    setup();


    while(is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    return 0;
    // printf("hello world!\n");
}