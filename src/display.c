#include "display.h"
#include <assert.h>


static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static uint32_t* color_buffer = NULL;
static float* z_buffer = NULL;
static SDL_Texture* color_buffer_texture = NULL;
static int window_width = 800;
static int window_height = 600;

static enum RENDER_MODE render_mode = RENDER_MODE_FILLED_WITH_WIREFRAME;
static enum CULL_MODE cull_mode = CULL_BACKFACE;




int DEFAULT_WINDOW_WIDTH = 800;
int DEFAULT_WINDOW_HEIGHT = 600;
bool is_fullscreen = false;

void set_render_mode(int render_mode_in) {
    render_mode = render_mode_in;
}

void set_cull_mode(int cull_mode_in) {
    cull_mode = cull_mode_in;
}

bool should_cull_backface(void) {
    return (cull_mode == CULL_BACKFACE);
}

bool should_render_filled_triangles(void) {
    return (render_mode == RENDER_MODE_FILLED || render_mode == RENDER_MODE_FILLED_WITH_WIREFRAME);
}

bool should_render_textured_triangles(void) {
    return (render_mode == RENDER_MODE_TEXTURED || render_mode == RENDER_MODE_TEXTURED_WITH_WIREFRAME);
}

bool should_render_wireframe(void) {
    return (render_mode == RENDER_MODE_FILLED_WITH_WIREFRAME ||  render_mode == RENDER_MODE_WIREFRAME || render_mode == RENDER_MODE_WIREFRAME_WITH_VERTICES || render_mode == RENDER_MODE_TEXTURED_WITH_WIREFRAME);
}

bool should_render_wireframe_with_vertices(void) {
    return (render_mode == RENDER_MODE_WIREFRAME_WITH_VERTICES);
}

int get_window_width(void) {
    return window_width;
}
int get_window_height(void) {
    return window_height;
}

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

    //NOTE(SJM): is too late to understand  for me what happens here.
    bool pixelated = false;
    if (pixelated) {
        window_width /= 3;
        window_height /= 3;

    }

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

    // allocate the required bytes.
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
    z_buffer = (float*)malloc(sizeof(float) * window_width * window_height);

    color_buffer_texture = SDL_CreateTexture(
        renderer, 
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

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
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return;
    }
    color_buffer[(window_width * y) + x] = color;

}

void draw_rect(int start_x, int start_y, int width, int height, uint32_t color) {
    
    if (start_y + height >= window_height) {
        return;
    }
    if (start_x + width >= window_width) {
        return;
    }
    // assert(start_y + height < window_height);
    // assert(start_x + width < window_width);

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
    SDL_RenderPresent(renderer);
}

void clear_color_buffer(uint32_t color) {
    for (int idx = 0; idx < window_width * window_height; ++idx) {
        color_buffer[idx] = color;
    }

}

void clear_z_buffer() {
    for (int idx = 0; idx < window_width * window_height; ++idx) {
            z_buffer[idx] = 1.0;
    }

}


uint32_t get_color_buffer_at(int x, int y) {
      if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return 1; // sentinel value of 1?
    };

    return color_buffer[(window_width * y) + x];

}
void update_color_buffer_at(int x, int y, uint32_t color) {
     if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return;
     }

    color_buffer[(window_width * y) + x] = color;
}


float get_zbuffer_at(int x, int y) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return 1.0; // sentinel value of 1?
    };
    return z_buffer[(window_width * y) + x];
}
void update_zbuffer_at(int x, int y, float value) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return;
    };

    z_buffer[(window_width * y) + x] = value;
}



void destroy_window(void) {
    // free the color and z buffer.
    free(color_buffer);
    free(z_buffer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}