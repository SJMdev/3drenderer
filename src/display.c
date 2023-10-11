#include "display.h"
#include <assert.h>


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;
int window_width = 0;
int window_height = 0;
int DEFAULT_WINDOW_WIDTH = 800;
int DEFAULT_WINDOW_HEIGHT = 600;
bool is_fullscreen = false;

bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "SDL_Init failed.");
        return false;
    }

    // use sdl to query what is the fullscreen max width and height
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    int max_window_width = display_mode.w;
    int max_window_height = display_mode.h;

    window_width = DEFAULT_WINDOW_WIDTH;
    window_height = DEFAULT_WINDOW_HEIGHT;
    if (is_fullscreen){
        window_width = max_window_width;
        window_height = max_window_height;
    }
    // create SDL window.
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
    );

    if (!window) {
        fprintf(stderr, "Error creating window.");
        return false;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1, // I don't care: get the first display device.
        0
    );
    if (!renderer) {
        fprintf(stderr, "Error creating window.");
    }
    if (is_fullscreen)
    {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    }

    return true;
}



void draw_grid() {

    // @NOTE: actual solution:
    for (int y = 0; y  < window_height; ++y) {
        for (int x = 0; x < window_width; ++x) {
            if (x % 10 == 0 || y% 10 == 0) {
                color_buffer[(window_width * y) + x] = 0x0f333333;
            }
        }
    }
}

void draw_pixel(int x, int y, uint32_t color) {
    assert(x > 0 && y > 0);
    assert(x < window_width && y < window_height);
    color_buffer[(window_width * y) + x] = color;
}

void draw_rect(int start_x, int start_y, int width, int height, uint32_t color) {
    assert(start_y + height < window_height);
    assert(start_x + width < window_width);

    for (int y = start_y; y < start_y + height; ++y) {
        for (int x = start_x; x < start_x + width; ++x) {
            draw_pixel(x, y, color);
        }
    }
}

// copy the color_buffer to the color_buffer_texture
void render_color_buffer() {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(window_width * sizeof(uint32_t))
    );
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color) {
    for (int y = 0; y < window_height; ++y){
        for (int x =0; x < window_width; ++x) {
            color_buffer[(window_width * y) + x] = color;
        }
    }

}


void destroy_window(void) {
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}