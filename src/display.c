#include "display.h"
#include <assert.h>


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
uint32_t* color_buffer = NULL;
float* z_buffer = NULL;
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

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;

    int side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y); 

    // find out how much we should increment in both x and y each step.
    float x_inc = delta_x / (float)side_length;
    float y_inc = delta_y / (float)side_length;

    float current_x = x0;
    float current_y = y0;

    for (int idx = 0; idx <= side_length; ++idx) {
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }

}
void draw_pixel(int x, int y, uint32_t color) {
    if (x < 0 && y < 0) {
        fprintf(stderr, "drawing outside bounds at: x:%d, y:%d\n", x , y);
        // assert(x > 0 && y > 0);
        return;
    }
    if (x > window_width && y > window_height) {
        fprintf(stderr, "drawing outside bounds at: x:%d, y:%d\n", x , y);
        // assert(x < window_width && y < window_height);
        return;
    }
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

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x0, y0, x2, y2, color);
    draw_line(x1, y1, x2, y2, color);
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

void clear_z_buffer() {
    for (int y = 0; y < window_height; ++y){
        for (int x = 0; x < window_width; ++x) {
            z_buffer[(window_width * y) + x] = 1.0;
        }
    }

}

void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}