#include "clipping.h"
#include <math.h>
#include <assert.h>

#define PLANE_COUNT 6
plane_t frustum_planes[PLANE_COUNT];

void init_frustrum_planes(float fovx, float fovy, float z_near, float z_far) {
    float cos_half_fovy = cos(fovy / 2.0);
    float sin_half_fovy = sin(fovy / 2.0);
    float cos_half_fovx = cos(fovx / 2.0);
    float sin_half_fovx = sin(fovx / 2.0);


    vec3_t origin = {0.0,0.0,0.0};

    frustum_planes[LEFT_FRUSTUM_PLANE].point = origin;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fovx;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fovx;

    frustum_planes[RIGHT_FRUSTUM_PLANE].point = origin;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fovx;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fovx;
    
    frustum_planes[TOP_FRUSTUM_PLANE].point = origin;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0.0;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fovy;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fovy;
    
    frustum_planes[BOTTOM_FRUSTUM_PLANE].point = origin;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fovy;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fovy;
    
    frustum_planes[NEAR_FRUSTUM_PLANE].point = vec3_new(0.0, 0.0, z_near);
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;
    
    frustum_planes[FAR_FRUSTUM_PLANE].point = vec3_new(0.0, 0.0, z_far);
    frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}



polygon_t create_polygon_from_triangle(
    vec3_t v0,
    vec3_t v1,
    vec3_t v2,
    tex2_t t0,
    tex2_t t1,
    tex2_t t2
) {
    polygon_t polygon = {
        .vertices =  {v0, v1, v2},
        .texcoords = {t0, t1, t2},
        .vertex_count = 3
    };

    return polygon;
}

float float_lerp(float a, float b, float t) {
    return a + t * (b - a);
}

void clip_polygon_against_plane(polygon_t* polygon, int plane)
{
    vec3_t plane_point = frustum_planes[plane].point;
    vec3_t plane_normal = frustum_planes[plane].normal; 

    vec3_t inside_vertices[MAX_POLYGON_VERTEX_COUNT];    
    tex2_t inside_texcoords[MAX_POLYGON_VERTEX_COUNT];
    int inside_vertex_count = 0;

    // start the current vertex with thte first polygon vertex and texture coordinate
    vec3_t* current_vertex = &polygon->vertices[0];
    tex2_t* current_texcoords = &polygon->texcoords[0];

    // start the previous vertex with the last polygon vertex and texture coordinate

    vec3_t* previous_vertex = &polygon->vertices[polygon->vertex_count - 1];
    tex2_t* previous_texcoords = &polygon->texcoords[polygon->vertex_count - 1];


    float current_dot = 0;
    float previous_dot = vec3_dot(vec3_sub(*previous_vertex, plane_point), plane_normal);

    // loop while the current vertex is different from the last vertex
    while (current_vertex != &polygon->vertices[polygon->vertex_count]) {
        current_dot = vec3_dot(vec3_sub(*current_vertex, plane_point), plane_normal);

        // if we changed from isde to outside or vice-versa
        if (current_dot * previous_dot < 0.0) {
            // find the interpolation factor t
            // calculate the intersection point I = Q1 + t(Q2 - Q1)
            
            float t = previous_dot / (previous_dot - current_dot);

            vec3_t intersection_point = {
                .x = float_lerp(previous_vertex->x, current_vertex->x, t),
                .y = float_lerp(previous_vertex->y, current_vertex->y, t),
                .z =float_lerp(previous_vertex->z, current_vertex->z, t)
            };
            // intersection_point = vec3_sub(intersection_point, *previous_vertex); //I = (Qc - Qp)
            // intersection_point = vec3_mul(intersection_point, t); // I = t(Qc - Qp)
            // intersection_point = vec3_add(intersection_point, *previous_vertex); // I = Qp + t(Qc - Qp)

            // use the linear interpolation formula to get the interpolated U  and V texture coordinates.
            tex2_t interpolated_texcoords = {
                .u = float_lerp(previous_texcoords->u, current_texcoords->u, t),
                .v = float_lerp(previous_texcoords->v, current_texcoords->v, t)
            };

            inside_texcoords[inside_vertex_count] = interpolated_texcoords;

            inside_vertices[inside_vertex_count] = vec3_clone(&intersection_point);
            inside_vertex_count += 1;
        }

        // if current point is inside the plane
        if (current_dot > 0.0) {
            // insert current vertex in the list of inside vertices
            inside_vertices[inside_vertex_count] = vec3_clone(current_vertex);
            inside_texcoords[inside_vertex_count] = tex2_clone(current_texcoords);
            inside_vertex_count += 1;
        }

        previous_dot = current_dot;
        previous_vertex = current_vertex;
        previous_texcoords = current_texcoords;
        current_vertex++; // ouch.
        current_texcoords++;
    }

    // @NOTE(SJM): can it be the case that we end up with less vertices than we started with?
    // at the end, copy the list of inside vertices into  the destination polygon (out parameter)
    for (int idx = 0; idx != inside_vertex_count; ++idx) {
        polygon->vertices[idx] = vec3_clone(&inside_vertices[idx]);
        polygon->texcoords[idx] = tex2_clone(&inside_texcoords[idx]);
    }

    polygon->vertex_count = inside_vertex_count;
}

void clip_polygon(polygon_t* polygon) {

    clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);
}

void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* triangle_count) {

    if (polygon->vertex_count < 3) {
        *triangle_count = 0;
        return;
    }

    assert(polygon->vertex_count >= 3);

    for (int idx = 0; idx != polygon->vertex_count - 2; ++idx) {
        int index0 = 0;  // triangulate from the first point like a "fan" 
        int index1 = idx + 1;
        int index2 = idx + 2;

        triangles[idx].points[0] = vec4_from_vec3(polygon->vertices[index0]);
        triangles[idx].points[1] = vec4_from_vec3(polygon->vertices[index1]);
        triangles[idx].points[2] = vec4_from_vec3(polygon->vertices[index2]);

        triangles[idx].texcoords[0] = polygon->texcoords[index0];
        triangles[idx].texcoords[1] = polygon->texcoords[index1];
        triangles[idx].texcoords[2] = polygon->texcoords[index2];
    } 

    // this is some property, apparently.
    *triangle_count = polygon->vertex_count - 2;
}

