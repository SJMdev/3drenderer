#include "triangle.h"
#include "display.h"
#include "vector.h"
#include <assert.h>

void int_swap(int*a ,int* b){
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void float_swap(float*a, float* b) {
    float tmp = *a;
    *a = *b;
    *b = tmp;
}


void fill_flat_bottom_triangle(int x0, int y0, int x1,int y1, int x2, int y2, uint32_t color) {

   // Find the two slopes (two triangle legs)
    float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
    float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

    // Start x_start and x_end from the top vertex (x0,y0)
    float x_start = x0;
    float x_end = x0;

    // Loop all the scanlines from top to bottom
    for (int y = y0; y <= y2; y++) {
        draw_line(x_start, y, x_end, y, color);
        x_start += inv_slope_1;
        x_end += inv_slope_2;
    }

}
void fill_flat_top_triangle(int x0, int y0, int x1,int y1, int x2, int y2, uint32_t color) {
    // Find the two slopes (two triangle legs)
    float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
    float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);

    // Start x_start and x_end from the bottom vertex (x2,y2)
    float x_start = x2;
    float x_end = x2;

    // Loop all the scanlines from bottom to top
    for (int y = y2; y >= y0; y--) {
        draw_line(x_start, y, x_end, y, color);
        x_start -= inv_slope_1;
        x_end -= inv_slope_2;
    }
}


// draw a filled triangle with the flat-top / flat-bottom method.
// we split the original triangle in two, half flat-bottom and half flat-top
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // we need to sort vertices by y coordinate ascending (y0 < y1 < y2)
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    // establish "midpoint" for flat top / flat bottom.
  if (y1 == y2) {
        // Draw flat-bottom triangle
        fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
    } else if (y0 == y1) {
        // Draw flat-top triangle
        fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
    } else {
        // Calculate the new vertex (Mx,My) using triangle similarity
        int My = y1;
        int Mx = (((x2 - x0) * (y1 - y0)) / (y2 - y0)) + x0;

        // Draw flat-bottom triangle
        fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);

        // Draw flat-top triangle
        fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
    }
}


void draw_texel(
    int x,
    int y,
    uint32_t* texture,
    vec2_t point_a,
    vec2_t point_b,
    vec2_t point_c,
    float u0, float v0,
    float u1, float v1,
    float u2, float v2) {
    
    vec2_t point_p = {x,y};
    vec3_t weights = barycentric_weights(point_a, point_b, point_c, point_p);
    float alpha = weights.x;
    float beta =  weights.y;
    float gamma = weights.z;

    // perofrm interpolation of all U and V  values using barycentric weights.
    float interpolated_u = alpha * u0 + beta * u1 + gamma * u2;
    float interpolated_v = alpha * v0 + beta * v1 + gamma * v2;

    int tex_x = abs((int)(interpolated_u * texture_width));
    int tex_y = abs((int)(interpolated_v * texture_width));
    if(tex_y >= texture_height) 
    {
        tex_y = texture_height - 1;
    }
    // assert(tex_x < texture_width);
    if (tex_x >= texture_width) {
        tex_x = texture_width - 1;
    }


    draw_pixel(x,y, texture[(texture_width * tex_y + tex_x)]);
}



// draw a textured traignle with the flat-top / flat-bottom method.
// we splti the orignal triangle in two, half flat bottom and half flat-top
void draw_textured_triangle(int x0, int y0, float u0, float v0,
                            int x1, int y1, float u1, float v1,
                            int x2, int y2, float u2, float v2,
                            uint32_t* texture) {    
                            
    // TODO: loop over all the pixels of the triangle to render them based on  the color
    // that is sampled from the texture.
    // we need to sort vertices by y coordinate ascending (y0 < y1 < y2)
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);

        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);

        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    // create vectors after we sort vertices.
    vec2_t point_a = {x0, y0};
    vec2_t point_b = {x1, y1};
    vec2_t point_c = {x2, y2};

    //////////////////////////////////////////////////////
    // render the upper part of the triangle (flat bottom)
    //////////////////////////////////////////////////////
    {
        float inv_slope_1 = 0;
        float inv_slope_2 = 0;

        if ((y1 - y0) != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
        if ((y2 - y0) != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);


        if (y1 - y0 != 0) {
            // scanline for scanline
            for (int y = y0; y  <= y1; y++) {
                int x_start = x1 + (y - y1) * inv_slope_1;
                int x_end = x0 + (y - y0) * inv_slope_2;

                // 
                if  (x_end < x_start) {
                    int_swap(&x_start, &x_end);
                }
                // pixel for pixel
                for (int x = x_start; x < x_end; x++) {
                    // todo: draw our pixel with the color that comes from the texture.
                    draw_texel(x, y, texture,
                        point_a,
                        point_b,
                        point_c,
                        u0, v0,
                        u1, v1,
                        u2, v2
                    );
                }
            }
        }

    }

    {
        float inv_slope_1 = 0;
        float inv_slope_2 = 0;

        if ((y2 - y1) != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
        if ((y2 - y0) != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);


        if (y2 - y1 != 0) {
            // scanline for scanline
            for (int y = y1; y  <= y2; y++) {
                int x_start = x1 + (y - y1) * inv_slope_1;
                int x_end = x0 + (y - y0) * inv_slope_2;

                // 
                if  (x_end < x_start) {
                    int_swap(&x_start, &x_end);
                }
                // pixel for pixel
                for (int x = x_start; x < x_end; x++) {
                    // todo: draw our pixel with the color that comes from the texture.
                    draw_texel(x, y, texture,
                        point_a,
                        point_b,
                        point_c,
                        u0, v0,
                        u1, v1,
                        u2, v2
                    );
                }
            }
        }
    }
}
