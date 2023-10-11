#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

// We need to tell SDL that we are doing the main instead of SDL.
#define SDL_MAIN_HANDLED 
#include <SDL2/SDL.h>

bool is_running = false;
const bool is_fullscreen = false;


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;


uint32_t* color_buffer = NULL;
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;


//@NOTE(SMIA): empty args means you can call this function with varargs..
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

    int window_width = WINDOW_WIDTH;
    int window_height = WINDOW_HEIGHT;
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

void setup() {
    // allocate the required bytes.
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * WINDOW_WIDTH * WINDOW_HEIGHT);

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
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
void update() {
    // assert(false && "NOT IMPLEMENTED!");

}

void clear_color_buffer(uint32_t color) {

    for (int y = 0; y < WINDOW_HEIGHT; ++y){
        for (int x =0; x < WINDOW_WIDTH; ++x) {
            color_buffer[(WINDOW_WIDTH * y) + x] = color;
        }
    }

}

// copy the color_buffer to the color_buffer_texture
void render_color_buffer() {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(WINDOW_WIDTH * sizeof(uint32_t))
    );
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void draw_rect(int start_x, int start_y, int width, int height, uint32_t color) {
    assert(start_y + height < WINDOW_HEIGHT);
    assert(start_x + width < WINDOW_WIDTH);

    for (int y = start_y; y < start_y + height; ++y) {
        for (int x = start_x; x < start_x + width; ++x) {
            color_buffer[(WINDOW_WIDTH * y) + x] = color;
        }
    }
}

void draw_grid() {

    // @NOTE: actual solution:
    for (int y = 0; y  < WINDOW_HEIGHT; ++y) {
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            if (x % 10 == 0 || y% 10 == 0) {
                color_buffer[(WINDOW_WIDTH * y) + x] = 0x0f333333;
            }
        }
    }
    
    // my solution:
    // horizontal lines.
    // for (int y = 0; y < WINDOW_HEIGHT; y+= 10) {
    //     for (int x =0; x < WINDOW_WIDTH; ++x) {
    //         color_buffer[(WINDOW_WIDTH * y) + x] = 0x0f333333;
    //     }
    // }
    // // vertical lines.
    // for (int y = 0; y < WINDOW_HEIGHT; ++y) {
    //     for (int x =0; x < WINDOW_WIDTH; x+= 10) {
    //         color_buffer[(WINDOW_WIDTH * y) + x] = 0x0f0a0a0a;
    //     }
    // }
}


void render(void) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
    SDL_RenderClear(renderer);

    draw_grid();
    draw_rect(300, 200, 300, 150, 0xFFFF00FF);
    render_color_buffer();
    clear_color_buffer(0xff000000);

    SDL_RenderPresent(renderer);
}

void destroy_window(void) {
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    // TODO: create an SDL window.
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