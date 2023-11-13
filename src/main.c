#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "upng.h"

// We need to tell SDL that we are doing the main instead of SDL.
#define SDL_MAIN_HANDLED 
#include <SDL2/SDL.h>

#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "matrix.h"
#include "light.h"
#include "triangle.h"
#include "texture.h"
#include "camera.h"
#include "clipping.h"

enum RENDER_MODE render_mode = RENDER_MODE_FILLED_WITH_WIREFRAME;
enum CULL_MODE cull_mode = CULL_BACKFACE;

// Pressing “1” displays the wireframe and a small red dot for each triangle vertex
// Pressing “2” displays only the wireframe lines
// Pressing “3” displays filled triangles with a solid color
// Pressing “4” displays both filled triangles and wireframe lines
// Pressing “c” we should enable back-face culling
// Pressing “d” we should disable the back-face culling


#define MAX_TRIANGLES_PER_MESH 10000
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int triangles_to_render_count = 0;


bool is_running = false;
int previous_frame_time = 0;
float delta_time = 0;

mat4_t projection_matrix;
mat4_t view_matrix;

light_t light = {.direction = {0.0, 0.0, 1.0}};

void setup() {
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
    float aspect_ratio_y = (float)window_height / (float)window_width ;
    float aspect_ratio_x = (float)window_width / (float)window_height ;
    
    float fovy = M_PI / 3.0; // the same as 180/3 (or 60 degrees).
    float fovx = atan(tan(fovy / 2) * aspect_ratio_x) * 2.0;
    float z_near = 0.1;
    float z_far = 100.0;
    projection_matrix = mat4_make_perspective(fovy, aspect_ratio_y, z_near, z_far);
    init_frustrum_planes(fovx, fovy, z_near, z_far);


    // mesh_texture = (uint32_t*)REDBRICK_TEXTURE;
    // loads the cube values in the mesh data structure
    // load_cube_mesh_data();
    // load_obj_file_data("assets/f117.obj");
    // load_png_texture_data("./assets/f117.png");

    // load_obj_file_data("assets/f22.obj");
    // load_png_texture_data("./assets/f22.png");

    // load_obj_file_data("assets/drone.obj");
    // load_png_texture_data("./assets/drone.png");
    load_obj_file_data("assets/crab.obj");
    load_png_texture_data("assets/crab.png");

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
            // basic program control
            if (event.key.keysym.sym == SDLK_ESCAPE){
                is_running = false;
                break;
            }

            // fps movement
            if (event.key.keysym.sym == SDLK_w) {
                camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time);
                camera.position = vec3_add(camera.position, camera.forward_velocity); 
            };

            if (event.key.keysym.sym == SDLK_s) {
                camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time);
                camera.position = vec3_sub(camera.position, camera.forward_velocity);
            }

            
            if (event.key.keysym.sym == SDLK_a) {
                camera.yaw -= 1.0 * delta_time;
            }

            if (event.key.keysym.sym == SDLK_d) {
                camera.yaw += 1.0 * delta_time;
            }

            if (event.key.keysym.sym == SDLK_UP ){
                camera.position.y += 3.0 * delta_time;
            }

            if (event.key.keysym.sym == SDLK_DOWN ){
                camera.position.y -= 3.0 * delta_time;
            }

            // rendering modes.

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
            if (event.key.keysym.sym == SDLK_5) {
                render_mode = RENDER_MODE_TEXTURED;
            }
            if (event.key.keysym.sym == SDLK_6) {
                render_mode = RENDER_MODE_TEXTURED_WITH_WIREFRAME;
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

// vec2_t project(vec3_t point) {
//     vec2_t projected_point = {.x = (fov_factor *point.x) / point.z, .y = (fov_factor *point.y) / point.z};
//     return projected_point;
// }

void update() {

    int time_to_wait = FRAME_TARGET_TIME_MS - (SDL_GetTicks() - previous_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME_MS) {
        SDL_Delay(time_to_wait);
    }

    // Get a delta time factor converted to seconds to be used to update our game objects.
    // /1000 because we get ms back from SDL.
    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0;



    // how many miliseconds have passed since the last frame?
    previous_frame_time = SDL_GetTicks();

    // initialize the counter of triangles to render for the current frame.
    triangles_to_render_count = 0;
    //@NOTE(SJM): we are moving to a static array of triangles to render.
    // initialize the array of triangles to render


    // change the mesh scale /rotation values per animation frame.
    // 0.6 radians per second.
    // mesh.rotation.x += 0.6 * delta_time;
    mesh.rotation.y += 0.6 * delta_time;
    // mesh.rotation.z += 0.6 * delta_time;

    // mesh.scale.x += 0.0002;
    // mesh.scale.y += 0.0002;
    // translate the vertex away from the camera.
    // mesh.translation.x += 0.001;
    mesh.translation.z = 4.0;

    // coimpute the new  camera rotation and translation for the fps camera movement.
    vec3_t up_direction = {0.0, 1.0, 0.0};
    // todo: find the target direction.

    // initialize the target 
    vec3_t target = {0.0,0.0,1.0};
    mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
    camera.direction = vec3_from_vec4(mat4_mul_vec4(camera_yaw_rotation, vec4_from_vec3(target)));

    // offset the camera position in the direction of where the camera is pointing at.
    target = vec3_add(camera.position, camera.direction);

    view_matrix = mat4_look_at(camera.position, target, up_direction);



    mat4_t translation_matrix = mat4_make_translate(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(
        mesh.rotation.x
    );
    mat4_t rotation_matrix_y = mat4_make_rotation_y(
        mesh.rotation.y
    );
    mat4_t rotation_matrix_z = mat4_make_rotation_z(
        mesh.rotation.z
    );


    int face_count = array_length(mesh.faces);
    // loop over faces
    for (int face_idx = 0; face_idx < face_count; ++face_idx) {
        //@NOTE(SJM): for now, just do one triangle
        face_t mesh_face = mesh.faces[face_idx];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a];
        face_vertices[1] = mesh.vertices[mesh_face.b];
        face_vertices[2] = mesh.vertices[mesh_face.c];

        vec4_t transformed_vertices[3]; 

        // for all three vertices 
        for (int vertex_idx = 0; vertex_idx <3; ++vertex_idx){

            

            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[vertex_idx]);

            mat4_t world_matrix = mat4_identity();
            world_matrix = mat4_mul_mat4(world_matrix, scale_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            // transform to (view / camera) space  by multiplying with view matrix.
            transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);
            
            transformed_vertices[vertex_idx]  = transformed_vertex;
        }

        // backface culling.
        vec3_t a = vec3_from_vec4(transformed_vertices[0]);
        vec3_t b = vec3_from_vec4(transformed_vertices[1]);
        vec3_t c = vec3_from_vec4(transformed_vertices[2]);

        vec3_t b_minus_a = vec3_sub(b, a);
        vec3_t c_minus_a = vec3_sub(c, a);
        vec3_normalize(&b_minus_a);
        vec3_normalize(&c_minus_a);

        // calculate the normal of the face
        vec3_t origin = {0.0,0.0,0.0};
        vec3_t normal = vec3_cross(b_minus_a, c_minus_a);
        // normalize the normal
        vec3_normalize(&normal);
        
        // calculate ray vector
        vec3_t camera_ray_vector = vec3_sub(origin, a);
        // calculate how aligned the camera ray is with the face normal
        float dot_normal_camera = vec3_dot(normal, camera_ray_vector);

        if (dot_normal_camera < 0 && (cull_mode == CULL_BACKFACE)) {
            continue;
        }

        // create a polygon from the original transformed triangle to be clipped.
        polygon_t polygon  = create_polygon_from_triangle(
            vec3_from_vec4(transformed_vertices[0]),
            vec3_from_vec4(transformed_vertices[1]),
            vec3_from_vec4(transformed_vertices[2]),
            mesh_face.a_uv,
            mesh_face.b_uv,
            mesh_face.c_uv
            );

        clip_polygon(&polygon);

        // break the clipped polygon apart back into individual triangles.
        triangle_t triangles_after_clipping[MAX_POLYGON_VERTEX_COUNT];
        int triangle_count_after_clipping = 0;

        // triangulate the polygon.
        triangles_from_polygon(&polygon, triangles_after_clipping, &triangle_count_after_clipping);

        // loop over all the assembled triangles after clipping
        for (int t = 0; t != triangle_count_after_clipping; ++t) {
            triangle_t triangle_after_clipping = triangles_after_clipping[t];

            vec4_t projected_points[3];

            for (int vertex_idx = 0; vertex_idx < 3; ++vertex_idx) {
                vec4_t projected_point = mat4_mul_vec4_project(projection_matrix, triangle_after_clipping.points[vertex_idx]);

                projected_points[vertex_idx] = projected_point;
                // scale into the view.
                projected_points[vertex_idx].x *= (window_width / 2.0);
                projected_points[vertex_idx].y *= (window_height / 2.0);

                // invert y, since screen space y is top -> bottom,
                projected_points[vertex_idx].y *= -1;

                // translate the projected points to the middle of the screen.
                projected_points[vertex_idx].x += (window_width  / 2.0);
                projected_points[vertex_idx].y += (window_height / 2.0);

            }

            //@NOTE(SJM): this is no longer necessary after introduction of z buffer
            // calculate the average depth for each face based on the vertices after transformation.
            // float average_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;

            // calculate the triangle color based on the light angle
            float light_intensity_vector = -vec3_dot(light.direction, normal);

            // ussed to be projected triangle.
            triangle_t triangle_to_render= {
                .points = {
                    {projected_points[0].x,  projected_points[0].y,projected_points[0].z, projected_points[0].w},
                    {projected_points[1].x,  projected_points[1].y,projected_points[1].z, projected_points[1].w},
                    {projected_points[2].x,  projected_points[2].y,projected_points[2].z, projected_points[2].w},
                },
                .texcoords = {
                    {triangle_after_clipping.texcoords[0].u, triangle_after_clipping.texcoords[0].v},
                    {triangle_after_clipping.texcoords[1].u, triangle_after_clipping.texcoords[1].v},
                    {triangle_after_clipping.texcoords[2].u, triangle_after_clipping.texcoords[2].v}
                },
                .color = light_apply_intensity(mesh_face.color, light_intensity_vector)
            };

            // save the projected triangle in the array of triangles to render.
            if (triangles_to_render_count < MAX_TRIANGLES_PER_MESH) {
                triangles_to_render[triangles_to_render_count] = triangle_to_render;
                triangles_to_render_count += 1;
            } else{
                assert(false && "TOO MANY TRIANGLES!");
            }
        }

      
        
    }

    //@NOTE(SJM): this is no longer necessary after introduction of z-buffer.
    // Painter's algorithm:
    // sort the triangles to render by their average_depth
    // int triangle_count = array_length(triangles_to_render);

    // for(int idx = 0; idx != triangle_count; ++idx) {
    //     for (int jdx = idx; jdx != triangle_count; ++jdx) {
    //         // idx is "nearer" than jdx: swap them.
    //         if (triangles_to_render[idx].average_depth < triangles_to_render[jdx].average_depth) {
    //             triangle_t temp = triangles_to_render[idx];
    //             triangles_to_render[idx] = triangles_to_render[jdx];
    //             triangles_to_render[jdx] = temp;
    //         }
    //     }
    // }

}

void render(void) {
    // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
    // SDL_RenderClear(renderer);

    draw_grid();

    // loop over all projected triangles and render them.

    //@SPEED: this evaluates render_mode linear amount of times for the number of triangles.
    // not really necessary, but otherwise there is some ugly code duplication.
    for (int triangle_idx = 0; triangle_idx < triangles_to_render_count; ++triangle_idx) {
        triangle_t triangle = triangles_to_render[triangle_idx];

        // filled
        if (render_mode == RENDER_MODE_FILLED || render_mode == RENDER_MODE_FILLED_WITH_WIREFRAME) {
            draw_filled_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[0].z,
                triangle.points[0].w,

                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[1].z,
                triangle.points[1].w,

                triangle.points[2].x,
                triangle.points[2].y,
                triangle.points[2].z,
                triangle.points[2].w,
                triangle.color);
        }
        // textured
        if (render_mode == RENDER_MODE_TEXTURED || render_mode == RENDER_MODE_TEXTURED_WITH_WIREFRAME) {
            draw_textured_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[0].z,
                triangle.points[0].w,
                triangle.texcoords[0].u,
                triangle.texcoords[0].v,

                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[1].z,
                triangle.points[1].w,
                triangle.texcoords[1].u,
                triangle.texcoords[1].v,

                triangle.points[2].x,
                triangle.points[2].y,
                triangle.points[2].z,
                triangle.points[2].w,
                triangle.texcoords[2].u,
                triangle.texcoords[2].v,
                mesh_texture
            );
        }

        // wireframe
        if (render_mode == RENDER_MODE_FILLED_WITH_WIREFRAME ||  render_mode == RENDER_MODE_WIREFRAME || render_mode == RENDER_MODE_WIREFRAME_WITH_VERTICES || render_mode == RENDER_MODE_TEXTURED_WITH_WIREFRAME) {
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

    // test to see if we are right handed coordinate system (we are.)
    // draw_triangle(100,100, 500, 100,  300, 300, 0xFFFF00FF);


    render_color_buffer();
    clear_color_buffer(0xff000000);
    clear_z_buffer();

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
    free(z_buffer);

    upng_free(png_texture);

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