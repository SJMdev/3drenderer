#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

// We need to tell SDL that we are doing the main instead of SDL.
#define SDL_MAIN_HANDLED 
#include <SDL2/SDL.h>

#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"

//@NOTE(SJM): we can do the enum trick? << 0, << 1, etc.

enum RENDER_MODE render_mode = RENDER_MODE_FILLED_WITH_WIREFRAME;
enum CULL_MODE cull_mode = CULL_BACKFACE;

// Pressing “1” displays the wireframe and a small red dot for each triangle vertex
// Pressing “2” displays only the wireframe lines
// Pressing “3” displays filled triangles with a solid color
// Pressing “4” displays both filled triangles and wireframe lines
// Pressing “c” we should enable back-face culling
// Pressing “d” we should disable the back-face culling



triangle_t* triangles_to_render = NULL;

vec3_t camera_position = {.x = 0, .y= 0, .z = 0};

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

    // loads the cube values in the mesh data structure
    load_cube_mesh_data();
    //load_obj_file_data("assets/cube.obj");

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

            if (event.key.keysym.sym == SDLK_1) {
                render_mode = RENDER_MODE_WIREFRAME_WITH_VERTICES;
            }   

            if (event.key.keysym.sym == SDLK_2) {
                render_mode = RENDER_MODE_WIREFRAME;
            }
            if (event.key.keysym.sym == SDLK_3) {
                render_mode = RENDER_MODE_FILLED_WITH_WIREFRAME;
            }
            if (event.key.keysym.sym == SDLK_4) {
                render_mode = RENDER_MODE_FILLED;
            }
            if (event.key.keysym.sym == SDLK_c) {
                cull_mode = CULL_BACKFACE;
            }
            if (event.key.keysym.sym == SDLK_b) {
                cull_mode = CULL_NONE;
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

    // initialize he array of triangles to render
    triangles_to_render = NULL; // uh, are we not leaking?

    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.02;

    int face_count = array_length(mesh.faces);
    // loop over faces
    for (int face_idx = 0; face_idx < face_count; ++face_idx) {
        face_t mesh_face = mesh.faces[face_idx];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1]; // index displacement because face thing.
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec3_t transformed_vertices[3]; 

        // for all three vertices 
        for (int vertex_idx = 0; vertex_idx <3; ++vertex_idx){
            vec3_t transformed_vertex = face_vertices[vertex_idx];

            transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z); //

            // translate the vertex away from the camera.
            transformed_vertex.z += 5;

            transformed_vertices[vertex_idx]  = transformed_vertex;
        }

        // backface culling.
        vec3_t a = transformed_vertices[0];
        vec3_t b = transformed_vertices[1];
        vec3_t c = transformed_vertices[2];

        vec3_t b_minus_a = vec3_sub(b, a);
        vec3_t c_minus_a = vec3_sub(c, a);
        vec3_normalize(&b_minus_a);
        vec3_normalize(&c_minus_a);

        // calculate the normal of the face
        vec3_t normal = vec3_cross(b_minus_a, c_minus_a);
        // normalize the normal
        vec3_normalize(&normal);
        
        // calculate ray vector
        vec3_t camera_ray_vector = vec3_sub(camera_position, a);
        // calculate how aligned the camera ray is with the face normal
        float dot_normal_camera = vec3_dot(normal, camera_ray_vector);

        if (dot_normal_camera < 0 && (cull_mode == CULL_BACKFACE)) {
            continue;
        }


        vec2_t projected_points[3];

        for (int vertex_idx = 0; vertex_idx < 3; ++vertex_idx) {
            projected_points[vertex_idx] = project(transformed_vertices[vertex_idx]);
            // scale and translate the projected points to the middle of the screen.
            projected_points[vertex_idx].x += (window_width  / 2);
            projected_points[vertex_idx].y += (window_height / 2);

        }

        // calculate the average depth for each face based on the vertices after transformation.
        float average_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;

        triangle_t projected_triangle= {
            .points = {
                {projected_points[0].x, projected_points[0].y},
                {projected_points[1].x,  projected_points[1].y},
                {projected_points[2].x,  projected_points[2].y},
            },
            .color = mesh_face.color,
            .average_depth = average_depth
        };

        // save the projected triangle in the array of triangles to render.
        // triangles_to_render[face_idx] = projected_triangle;
        array_push(triangles_to_render, projected_triangle);
    }

    // Painter's algorithm:
    // sort the triangles to render by their average_depth
    int triangle_count = array_length(triangles_to_render);

    for(int idx = 0; idx != triangle_count; ++idx) {
        for (int jdx = idx; jdx != triangle_count; ++jdx) {
            // idx is "nearer" than jdx: swap them.
            if (triangles_to_render[idx].average_depth < triangles_to_render[jdx].average_depth) {
                triangle_t temp = triangles_to_render[idx];
                triangles_to_render[idx] = triangles_to_render[jdx];
                triangles_to_render[jdx] = temp;
            }
        }
    }

}

void render(void) {
    // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
    // SDL_RenderClear(renderer);

    draw_grid();

    int triangle_count = array_length(triangles_to_render);
    // loop over all projected triangles and render them.

    //@SPEED: this evaluates render_mode linear amount of times for the number of triangles.
    // not really necessary, but otherwise there is some ugly code duplication.
    for (int triangle_idx = 0; triangle_idx < triangle_count; ++triangle_idx) {
        triangle_t triangle = triangles_to_render[triangle_idx];

        // filled
        if (render_mode == RENDER_MODE_FILLED || render_mode == RENDER_MODE_FILLED_WITH_WIREFRAME) {
            draw_filled_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y,
                triangle.color);
        }
        // wireframe
        if (render_mode == RENDER_MODE_FILLED_WITH_WIREFRAME ||  render_mode == RENDER_MODE_WIREFRAME || render_mode == RENDER_MODE_WIREFRAME_WITH_VERTICES) {
            draw_triangle(
                        triangle.points[0].x,
                        triangle.points[0].y,
                        triangle.points[1].x,
                        triangle.points[1].y,
                        triangle.points[2].x,
                        triangle.points[2].y,
                        0xFF00FF00);
        }

        if (render_mode == RENDER_MODE_WIREFRAME_WITH_VERTICES) {
            draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000);
        }


    }

    // clear the array of triangles to render every frame loop
    array_free(triangles_to_render);

    render_color_buffer();
    clear_color_buffer(0xff000000);

    SDL_RenderPresent(renderer);
}

void transform_points() {
    // rotate
    // scale
    // translate
}

// free the memory that was dynamically allocated by the program.
void free_resources(void) {
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
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
    free_resources();

    return 0;
    // printf("hello world!\n");
}