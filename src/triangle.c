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

void draw_triangle_pixel(
    int x,
    int y,
    uint32_t color,
    vec4_t point_a,
    vec4_t point_b,
    vec4_t point_c
)
{
    // we do not need z or w for barycentric weights.
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    vec2_t point_p = { x,y };
    vec3_t weights = barycentric_weights(a, b, c, point_p);
    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    // variazble sto store the interpolated values of U,v, and also 1/W for the current pixel.
    float interpolated_reciprocal_w = 0;

    // interpolate 1/w for the current pixel.
    interpolated_reciprocal_w = alpha * (1 / point_a.w) + beta * (1 / point_b.w) + gamma * (1 / point_c.w);


    // adjust 1/w such that the pixels that are closer to the camera have smaller values.
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    // assert(x < window_width);
    // assert(y < window_height);

    if (x >= window_width || x < 0 || y >= window_height || y < 0) {
        printf("wanted to draw out of bounds, forcing early return.\n");
        return;
    }

    // only the pixel if the depth value is less than the one previously stored in z-buffer. (less meaning closer to the camera,.
    // since z is into the screen.)
    if (interpolated_reciprocal_w < z_buffer[(y * window_width) +  x]) {
        draw_pixel(x,y, color);
        z_buffer[(y * window_width) +  x] = interpolated_reciprocal_w;
    }
    
}




// draw a filled triangle with the flat-top / flat-bottom method.
// we split the original triangle in two, half flat-bottom and half flat-top
void draw_filled_triangle(
    int x0, int y0, float z0, float w0,
     int x1, int y1, float z1, float w1,
      int x2, int y2,float z2, float w2,
       uint32_t color) {
                           
    // TODO: loop over all the pixels of the triangle to render them based on  the color
    // that is sampled from the texture.
    // we need to sort vertices by y coordinate ascending (y0 < y1 < y2)
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);

        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);

        float_swap(&z1, &z2);
        float_swap(&w1, &w2);

    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);

        float_swap(&z0, &z1);
        float_swap(&w0, &w1);

    }

    // create vectors after we sort vertices.
    vec4_t point_a = {x0, y0, z0, w0};
    vec4_t point_b = {x1, y1, z1, w1};
    vec4_t point_c = {x2, y2, z2, w2};

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
                    draw_triangle_pixel(x,y, color, point_a, point_b, point_c);
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
                    draw_triangle_pixel(x,y, color, point_a, point_b, point_c);

                }
            }
        }
    }
}


void draw_texel(
    int x,
    int y,
    uint32_t* texture,
    vec4_t point_a,
    vec4_t point_b,
    vec4_t point_c,
    tex2_t a_uv,
    tex2_t b_uv,
    tex2_t c_uv) {        
    // we do not need z or w for barycentric weights.
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    vec2_t point_p = {x,y};
    vec3_t weights = barycentric_weights(a, b, c, point_p);
    float alpha = weights.x;
    float beta =  weights.y;
    float gamma = weights.z;

    // variazble sto store the interpolated values of U,v, and also 1/W for the current pixel.
    float interpolated_u = 0;
    float interpolated_v = 0;
    float interpolated_reciprocal_w = 0;

    // perform interpolation of all U and V  values using barycentric weights.
    interpolated_u = alpha * (a_uv.u / point_a.w) + beta * (b_uv.u / point_b.w) + gamma * (c_uv.u / point_c.w);
    interpolated_v = alpha * (a_uv.v / point_a.w) + beta * (b_uv.v / point_b.w) + gamma * (c_uv.v / point_c.w);

    // also interpolate the value of 1/w for the current pixel.
    interpolated_reciprocal_w = alpha * (1 / point_a.w) + beta * ( 1/ point_b.w) + gamma * (1 / point_c.w);

    // now we can divide back both interpolated values by 1/w .
    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;

    // modulo so we do not have invalid values (not really clamping, but rolling over.)
    int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
    int tex_y = abs((int)(interpolated_v * texture_width)) % texture_height;
    
    // adjust 1/w such that the pixels that are closer to the camera have smaller values.
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    if (x >= window_width || x < 0 || y >= window_height || y < 0) {
        printf("wanted to draw out of bounds, forcing early return.\n");
        return;
    }

    // only the pixel if the depth value is less than the one previously stored in z-buffer. (less meaning closer to the camera,.
    // since z is into the screen.)
    if (interpolated_reciprocal_w < z_buffer[(y * window_width) +  x]) {
        draw_pixel(x,y, texture[(texture_width * tex_y) + tex_x]);
        z_buffer[(y * window_width) +  x] = interpolated_reciprocal_w;
    }

}



// draw a textured traignle with the flat-top / flat-bottom method.
// we splti the orignal triangle in two, half flat bottom and half flat-top
void draw_textured_triangle(int x0, int y0, float z0, float w0, float u0, float v0,
                            int x1, int y1, float z1, float w1, float u1, float v1,
                            int x2, int y2, float z2, float w2, float u2, float v2,
                            uint32_t* texture) {    
                            
    // TODO: loop over all the pixels of the triangle to render them based on  the color
    // that is sampled from the texture.
    // we need to sort vertices by y coordinate ascending (y0 < y1 < y2)
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);

        float_swap(&z0, &z1);
        float_swap(&w0, &w1);

        
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);

        float_swap(&z1, &z2);
        float_swap(&w1, &w2);

        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);

        float_swap(&z0, &z1);
        float_swap(&w0, &w1);

        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    // flip the V component to account for inverted UV-coordinates (V grows downwards)
    v0  = 1-v0;
    v1  = 1-v1;
    v2  = 1-v2;


    // create vectors after we sort vertices.
    vec4_t point_a = {x0, y0, z0, w0};
    vec4_t point_b = {x1, y1, z1, w1};
    vec4_t point_c = {x2, y2, z2, w2};
    tex2_t a_uv = {u0, v0};
    tex2_t b_uv = {u1, v1};
    tex2_t c_uv = {u2, v2};

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
                        a_uv,
                        b_uv,
                        c_uv
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
                        a_uv,
                        b_uv,
                        c_uv
                    );
                }
            }
        }
    }
}
