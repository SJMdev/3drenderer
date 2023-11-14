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
#include "matrix.h"
#include "light.h"
#include "triangle.h"
#include "texture.h"
#include "camera.h"
#include "clipping.h"
#include "mesh.h"
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

void setup() {


    set_render_mode(RENDER_MODE_WIREFRAME);
    set_cull_mode(CULL_BACKFACE);

    // initialize the scene light direction
    init_light(vec3_new(0.0,0.0,1.0));

    float aspect_ratio_y = (float)get_window_height() / (float)get_window_width() ;
    float aspect_ratio_x = (float)get_window_width() / (float)get_window_height() ;
    
    float fovy = M_PI / 3.0; // the same as 180/3 (or 60 degrees).
    float fovx = atan(tan(fovy / 2) * aspect_ratio_x) * 2.0;
    float z_near = 0.1;
    float z_far = 100.0;
    projection_matrix = mat4_make_perspective(fovy, aspect_ratio_y, z_near, z_far);
    init_frustrum_planes(fovx, fovy, z_near, z_far);

    // Loads mesh entities
    load_mesh("./assets/runway.obj", "./assets/runway.png", vec3_new(1, 1, 1), vec3_new(0, -1.5, +23), vec3_new(0, 0, 0));
    load_mesh("./assets/f22.obj", "./assets/f22.png", vec3_new(1, 1, 1), vec3_new(0, -1.3, +5), vec3_new(0, -M_PI/2, 0));
    load_mesh("./assets/efa.obj", "./assets/efa.png", vec3_new(1, 1, 1), vec3_new(-2, -1.3, +9), vec3_new(0, -M_PI/2, 0));
    load_mesh("./assets/f117.obj", "./assets/f117.png", vec3_new(1, 1, 1), vec3_new(+2, -1.3, +9), vec3_new(0, -M_PI/2, 0));

}

void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
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
                    set_camera_forward_velocity(vec3_mul(get_camera_direction(), 5.0 * delta_time));
                    set_camera_position(vec3_add(get_camera_position(), get_camera_forward_velocity())); 
                    break;
                };

                if (event.key.keysym.sym == SDLK_s) {
                    set_camera_forward_velocity(vec3_mul(get_camera_direction(), 5.0 * delta_time));
                    set_camera_position(vec3_sub(get_camera_position(), get_camera_forward_velocity()));
                    break;
                }

                
                if (event.key.keysym.sym == SDLK_r) {
                    float pitch = get_camera_pitch();
                    pitch = pitch + 3.0 * delta_time;
                    set_camera_pitch(pitch);
                    break;
                }

                if (event.key.keysym.sym == SDLK_f) {
                    float pitch = get_camera_pitch();
                    pitch = pitch - 3.0 * delta_time;
                    set_camera_pitch(pitch);
                    break;
                }



                if (event.key.keysym.sym == SDLK_a) {
                    set_camera_yaw(get_camera_yaw() - 1.0 * delta_time);
                    break;

                }

                if (event.key.keysym.sym == SDLK_d) {
                    set_camera_yaw(get_camera_yaw() + 1.0 * delta_time);
                    break;

                }

                if (event.key.keysym.sym == SDLK_UP ){
                    vec3_t camera_position = get_camera_position();
                    camera_position.y = camera_position.y + 3.0 * delta_time;
                    set_camera_position(camera_position);
                    break;

                }

                if (event.key.keysym.sym == SDLK_DOWN ){
                    vec3_t camera_position = get_camera_position();
                    camera_position.y = camera_position.y - 3.0 * delta_time;
                    set_camera_position(camera_position);
                    break;

                }

                // rendering modes.
                if (event.key.keysym.sym == SDLK_1) {
                    set_render_mode( RENDER_MODE_WIREFRAME_WITH_VERTICES);
                    break;

                }   

                if (event.key.keysym.sym == SDLK_2) {
                    set_render_mode( RENDER_MODE_WIREFRAME);
                    break;

                }
                if (event.key.keysym.sym == SDLK_3) {
                    set_render_mode( RENDER_MODE_FILLED_WITH_WIREFRAME);
                    break;

                }
                if (event.key.keysym.sym == SDLK_4) {
                    set_render_mode( RENDER_MODE_FILLED);
                    break;

                }
                if (event.key.keysym.sym == SDLK_5) {
                    set_render_mode( RENDER_MODE_TEXTURED);
                    break;

                }
                if (event.key.keysym.sym == SDLK_6) {
                    set_render_mode( RENDER_MODE_TEXTURED_WITH_WIREFRAME);
                    break;

                }
                if (event.key.keysym.sym == SDLK_c) {
                    set_cull_mode(CULL_BACKFACE);
                    break;

                }
                if (event.key.keysym.sym == SDLK_b) {
                    set_cull_mode(CULL_NONE);
                    break;
                }
            }

            default:
                // fprintf(stderr, "unknown event type.");
                break;

        }
    }
    
}



///////////////////////////////////////////////////////////////////////////////
// Process the graphics pipeline stages for all the mesh triangles
///////////////////////////////////////////////////////////////////////////////
// +-------------+
// | Model space |  <-- original mesh vertices
// +-------------+
// |   +-------------+
// `-> | World space |  <-- multiply by world matrix
//     +-------------+
//     |   +--------------+
//     `-> | Camera space |  <-- multiply by view matrix
//         +--------------+
//         |    +------------+
//         `--> |  Clipping  |  <-- clip against the six frustum planes
//              +------------+
//              |    +------------+
//              `--> | Projection |  <-- multiply by projection matrix
//                   +------------+
//                   |    +-------------+
//                   `--> | Image space |  <-- apply perspective divide
//                        +-------------+
//                        |    +--------------+
//                        `--> | Screen space |  <-- ready to render
//                             +--------------+
///////////////////////////////////////////////////////////////////////////////

void process_graphics_pipeline_stages(mesh_t* mesh) {
        // compute the new camera rotation and translation for the fps camera movement.
        vec3_t up_direction = {0.0, 1.0, 0.0};
        
        // find the target direction.
        // initialize the target 
        vec3_t target = {0.0,0.0,1.0};

        mat4_t camera_rotation = mat4_identity();
        mat4_t camera_pitch_rotation = mat4_make_rotation_x(get_camera_pitch());
        mat4_t camera_yaw_rotation = mat4_make_rotation_y(get_camera_yaw());
        camera_rotation = mat4_mul_mat4(camera_pitch_rotation,camera_rotation);
        camera_rotation = mat4_mul_mat4(camera_yaw_rotation, camera_rotation);
        set_camera_direction(vec3_from_vec4(mat4_mul_vec4(camera_rotation, vec4_from_vec3(target))));
        // offset the camera position in the direction of where the camera is pointing at.
        target = vec3_add(get_camera_position(), get_camera_direction());

        view_matrix = mat4_look_at(get_camera_position(), target, up_direction);

        mat4_t translation_matrix = mat4_make_translate(mesh->translation.x, mesh->translation.y, mesh->translation.z);
        mat4_t scale_matrix = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
        mat4_t rotation_matrix_x = mat4_make_rotation_x(
            mesh->rotation.x
        );
        mat4_t rotation_matrix_y = mat4_make_rotation_y(
            mesh->rotation.y
        );
        mat4_t rotation_matrix_z = mat4_make_rotation_z(
            mesh->rotation.z
        );


        int face_count = array_length(mesh->faces);
        // loop over faces
        for (int face_idx = 0; face_idx < face_count; ++face_idx) {
            //@NOTE(SJM): for now, just do one triangle
            face_t mesh_face = mesh->faces[face_idx];

            vec3_t face_vertices[3];
            face_vertices[0] = mesh->vertices[mesh_face.a];
            face_vertices[1] = mesh->vertices[mesh_face.b];
            face_vertices[2] = mesh->vertices[mesh_face.c];

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


            vec3_t face_normal = get_triangle_normal(transformed_vertices);
            // backface culling.
            if ( should_cull_backface()) {
                vec3_t origin = {0.0,0.0,0.0};
                vec3_t camera_ray_vector = vec3_sub(origin, vec3_from_vec4(transformed_vertices[0]));
                float dot_normal_camera = vec3_dot(face_normal, camera_ray_vector);
                if (dot_normal_camera < 0.0) {
                        continue;

                }
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
                    projected_points[vertex_idx].x *= (get_window_width() / 2.0);
                    projected_points[vertex_idx].y *= (get_window_height() / 2.0);

                    // invert y, since screen space y is top -> bottom,
                    projected_points[vertex_idx].y *= -1;

                    // translate the projected points to the middle of the screen.
                    projected_points[vertex_idx].x += (get_window_width()  / 2.0);
                    projected_points[vertex_idx].y += (get_window_height() / 2.0);

                }

                //@NOTE(SJM): this is no longer necessary after introduction of z buffer
                // calculate the average depth for each face based on the vertices after transformation.
                // float average_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;

                // calculate the triangle color based on the light angle
                float light_intensity_vector = -vec3_dot(face_normal, get_light_direction());

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
                    .color = light_apply_intensity(mesh_face.color, light_intensity_vector),
                    .texture= mesh->texture
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
}

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
    //@NOTE(SJM): we are moving to a static array of triangles to render.
    triangles_to_render_count = 0;
    // loop over all the meshes.
    for (int mesh_idx =0; mesh_idx != get_mesh_count(); ++mesh_idx) {
        mesh_t* mesh = get_mesh(mesh_idx);

        // change the mesh scale /rotation values per animation frame.
        // 0.6 radians per second.
        // mesh->rotation.x += 0.6 * delta_time;
        // mesh->rotation.y += 0.6 * delta_time;
        // mesh->rotation.z += 0.6 * delta_time;
        // mesh->scale.x += 0.0002;
        // mesh->scale.y += 0.0002;
        // translate the vertex away from the camera.
        // mesh->translation.x += 0.001;
        // mesh->translation.z = 4.0;
        process_graphics_pipeline_stages(mesh);
    }
}

void render(void) {
    // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
    // SDL_RenderClear(renderer);
    clear_color_buffer(0xff000000);
    clear_z_buffer();

    draw_grid();

    // loop over all projected triangles and render them.

    //@SPEED: this evaluates render_mode linear amount of times for the number of triangles.
    // not really necessary, but otherwise there is some ugly code duplication.
    for (int triangle_idx = 0; triangle_idx < triangles_to_render_count; ++triangle_idx) {
        triangle_t triangle = triangles_to_render[triangle_idx];

        // filled
        if (should_render_filled_triangles()) {
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
        if (should_render_textured_triangles()) {
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
                triangle.texture
            );
        }

        // wireframe
        if (should_render_wireframe()) {
            draw_triangle(
                        triangle.points[0].x,
                        triangle.points[0].y,
                        triangle.points[1].x,
                        triangle.points[1].y,
                        triangle.points[2].x,
                        triangle.points[2].y,
                        0xFF00FF00);
        }

        if (should_render_wireframe_with_vertices()) {
            draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000);
        }

    }

    // test to see if we are right handed coordinate system (we are.)
    // draw_triangle(100,100, 500, 100,  300, 300, 0xFFFF00FF);
    render_color_buffer();


}

void transform_points() {
    // rotate
    // scale
    // translate
}

// free the memory that was dynamically allocated by the program.
void free_resources(void) {
    free_meshes();
    destroy_window();
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

    free_resources();

    return 0;
    // printf("hello world!\n");
}