#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

// We need to tell SDL that we are doing the main instead of SDL.
#define SDL_MAIN_HANDLED 
#include <SDL2/SDL.h>

#include "display.h"
#include "vector.h"

#define POINT_COUNT (9 * 9 *9) // 9x9x9 cube.
vec3_t cube_points[POINT_COUNT];
vec2_t projected_points[POINT_COUNT];

vec3_t camera_position = {.x = 0, .y= 0, .z =-5};
float fov_factor = 640;
bool is_running = false;


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

    // start loading my array of vectors.
    // from -1 to 1 (in this 9x9x9 cube)
    int point_count = 0;
    for (float x = -1; x <= 1; x += 0.25) {
        for (float y= -1; y <= 1; y += 0.25) {
            for (float z = -1; z <= 1; z += 0.25) {
                vec3_t new_point = {.x = x, .y = y, .z = z};
                cube_points[point_count] = new_point;
                point_count += 1;
            }
        }
    }

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
    // assert(false && "NOT IMPLEMENTED!");
    for (int point_idx = 0; point_idx < POINT_COUNT; ++point_idx) {
        vec3_t point = cube_points[point_idx];

        // move the points away from the camera.
        point.z -= camera_position.z;
        // project the current point and save it in the array of projected points.
        vec2_t projected_point = project(point);
        projected_points[point_idx] = projected_point;
    }
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
    SDL_RenderClear(renderer);

    draw_grid();
    // draw_rect(300, 200, 300, 150, 0xFFFF00FF);

    for (int point_idx = 0; point_idx < POINT_COUNT; ++point_idx) {
        vec2_t projected_point = projected_points[point_idx];
        draw_rect(
            projected_point.x + (window_width / 2),
            projected_point.y + (window_height / 2),
            4, 
            4,
            0xFFFFFF00);
    }

    render_color_buffer();
    clear_color_buffer(0xff000000);

    SDL_RenderPresent(renderer);
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