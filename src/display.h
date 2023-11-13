#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#define FPS 120
#define FRAME_TARGET_TIME_MS (1000 / FPS)




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

int get_window_height(void);
int get_window_width(void);

void set_render_mode(int render_mode);
void set_cull_mode(int cull_mode);

bool should_cull_backface(void);
bool should_render_filled_triangles(void);
bool should_render_textured_triangles(void);
bool should_render_wireframe(void);
bool should_render_wireframe_with_vertices(void);
bool initialize_window(void);






void draw_grid(void);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_rect(int start_x, int start_y, int width, int height, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void clear_z_buffer();

uint32_t get_color_buffer_at(int x, int y);
void update_color_buffer_at(int x, int y, uint32_t color);

float get_zbuffer_at(int x, int y);
void update_zbuffer_at(int x, int y, float value);

void destroy_window(void);

#endif