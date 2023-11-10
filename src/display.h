#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#define FPS 120
#define FRAME_TARGET_TIME_MS (1000 / FPS)


extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern uint32_t* color_buffer;
extern SDL_Texture* color_buffer_texture;
extern float* z_buffer;


extern int window_width;
extern int window_height;
extern int DEFAULT_WINDOW_WIDTH;
extern int DEFAULT_WINDOW_HEIGHT;

extern bool is_fullscreen;

enum RENDER_MODE {
    RENDER_MODE_UNUSED = 0,
    RENDER_MODE_WIREFRAME_WITH_VERTICES, // 1
    RENDER_MODE_WIREFRAME, // 2
    RENDER_MODE_FILLED, // 3
    RENDER_MODE_FILLED_WITH_WIREFRAME,  // 4,
    RENDER_MODE_TEXTURED, // 5
    RENDER_MODE_TEXTURED_WITH_WIREFRAME //6 
};

enum CULL_MODE {
    CULL_NONE,
    CULL_BACKFACE
};

bool initialize_window(void);


void draw_grid(void);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_rect(int start_x, int start_y, int width, int height, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void clear_color_buffer(uint32_t color);
void clear_z_buffer();
void render_color_buffer(void);
void destroy_window(void);

#endif