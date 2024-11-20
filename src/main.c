#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "matrix.h"
#include "light.h"
#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "camera.h"

triangle_t *triangles_to_render = NULL;

mat4_t projection_matrix;
mat4_t view_matrix;

bool is_running = false;
float delta_time = 0.0;
int previous_frame_time = 0;

typedef enum
{
    wireframe_verbose,
    wireframe,
    solid,
    textures,
    all
} rendering_mode_t;

rendering_mode_t render_mode = solid;

bool is_culling_enabled = true;

void setup(void)
{
    // Allocate the required memory in bytes to hold the color buffer
    color_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    z_buffer = (float *)malloc(sizeof(float) * window_width * window_height);

    // Creating a SDL texture that is used to display the color buffer
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height);

    // Init perspective matrix
    float fov = M_PI / 3.0; // = 180 / 3
    float aspect = (float)window_height / (float)window_width;
    float znear = 0.1;
    float zfar = 100.0;
    projection_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

    // Manually load hardcoded texture data
    /*mesh_texture = (uint32_t *)REDBRICK_TEXTURE;
    texture_width = 64;
    texture_height = 64;*/

    // Load from png

    // load_cube_mesh_data();
    load_obj_file_data("./assets/f22.obj");
    load_png_texture_data("./assets/f22.png");
}

void process_input(void)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type)
    {
    case SDL_QUIT:
        is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
            is_running = false;
        if (event.key.keysym.sym == SDLK_1)
            render_mode = wireframe_verbose;
        if (event.key.keysym.sym == SDLK_2)
            render_mode = wireframe;
        if (event.key.keysym.sym == SDLK_3)
            render_mode = solid;
        if (event.key.keysym.sym == SDLK_4)
            render_mode = all;
        if (event.key.keysym.sym == SDLK_5)
            render_mode = textures;
        if (event.key.keysym.sym == SDLK_c)
            is_culling_enabled = true;
        if (event.key.keysym.sym == SDLK_v)
            is_culling_enabled = false;
        // Camera up
        if (event.key.keysym.sym == SDLK_UP)
            camera.position.y += 3.0 * delta_time;
        if (event.key.keysym.sym == SDLK_DOWN)
            camera.position.y -= 3.0 * delta_time;
        // Rotate left/right
        if (event.key.keysym.sym == SDLK_a)
            camera.yaw += 1.0 * delta_time;
        if (event.key.keysym.sym == SDLK_d)
            camera.yaw -= 1.0 * delta_time;
        // Move forward/backward
        if (event.key.keysym.sym == SDLK_w)
        {
            camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time);
            camera.position = vec3_add(camera.position, camera.forward_velocity);
        }
        if (event.key.keysym.sym == SDLK_s)
        {
            camera.forward_velocity = vec3_mul(camera.direction, 5.0 * delta_time);
            camera.position = vec3_sub(camera.position, camera.forward_velocity);
        }
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Function that receives a 3D vector and returns a projected 2D point
////////////////////////////////////////////////////////////////////////////////
/*vec2_t project(vec3_t point)
{
    vec2_t projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = (fov_factor * point.y) / point.z};
    return projected_point;
}*/

void update(void)
{
    // Wait some time until the reach the target frame time in milliseconds
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    // Only delay execution if we are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    {
        SDL_Delay(time_to_wait);
    }

    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0;

    previous_frame_time = SDL_GetTicks();

    // Initialize the array of triangles to render
    triangles_to_render = NULL;

    mesh.rotation.y += 1 * delta_time;
    // mesh.rotation.x += 1 * delta_time;
    // mesh.rotation.z += 1 * delta_time;
    //  mesh.scale.x += 0.002;
    //  mesh.scale.y += 0.001;
    //  mesh.translation.x += 0.001;
    mesh.translation.z = 5;

    // Create view matrix
    vec3_t up = {0, 1, 0};

    vec3_t target = {0, 0, 1};
    mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
    camera.direction = vec3_from_vec4(mat4_mul_vec4(camera_yaw_rotation, vec4_from_vec3(target)));

    // offset the camera position in the direction
    target = vec3_add(camera.position, camera.direction);

    view_matrix = mat4_look_at(camera.position, target, up);

    // Create scaale matrix that will be used to multiple mesh verticies
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotation_x_matrix = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_y_matrix = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_z_matrix = mat4_make_rotation_z(mesh.rotation.z);

    int num_mesh_faces = array_length(mesh.faces);
    // Loop all triangle faces of our mesh
    for (int i = 0; i < num_mesh_faces; i++)
    {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a];
        face_vertices[1] = mesh.vertices[mesh_face.b];
        face_vertices[2] = mesh.vertices[mesh_face.c];

        vec4_t transformed_verticies[3];

        // Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++)
        {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Create a world matrix combining scale, translate, and rotation
            mat4_t world_matrix = mat4_identity();
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_x_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_y_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_z_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            // Multiply transformed_vertex by world_matrix
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            // Multiply view_matrix by the vector to transform the scene to camera space
            transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

            // Save tranformed verticies
            transformed_verticies[j] = transformed_vertex;
        }

        // Check backface culling
        vec3_t vector_a = vec3_from_vec4(transformed_verticies[0]); // A
        vec3_t vector_b = vec3_from_vec4(transformed_verticies[1]); // B
        vec3_t vector_c = vec3_from_vec4(transformed_verticies[2]); // C

        vec3_t vector_ab = vec3_sub(vector_b, vector_a);
        vec3_t vector_ac = vec3_sub(vector_c, vector_a);
        vec3_normalize(&vector_ab);
        vec3_normalize(&vector_ac);

        vec3_t normal = vec3_cross(vector_ab, vector_ac);
        vec3_normalize(&normal);

        vec3_t origin = {0, 0, 0};
        vec3_t camera_ray = vec3_sub(origin, vector_a);

        // calc alginment with camera
        float dot_alignment_to_camera = vec3_dot(normal, camera_ray);

        if (dot_alignment_to_camera < 0 && is_culling_enabled)
        {
            continue;
        }

        vec4_t projected_points[3];

        // Perform projection on verticies
        for (int j = 0; j < 3; j++)
        {
            // Project the current vertex
            projected_points[j] = mat4_mul_vec4_project(projection_matrix, transformed_verticies[j]);

            // Scale
            projected_points[j].x *= (window_width / 2.0);
            projected_points[j].y *= (window_height / 2.0);

            // Invert the y value to account for flipped y coordinates
            projected_points[j].y *= -1;

            // Translate the projected points to the middle of the screen
            projected_points[j].x += (window_width / 2.0);
            projected_points[j].y += (window_height / 2.0);
        }

        // Calculate avg depth for each face of the vertices z-value
        float avg_depth = (transformed_verticies[0].z + transformed_verticies[1].z + transformed_verticies[2].z) / 3.0;

        // Calculate flat shading on triangle
        float light_intensity_dot = -vec3_dot(normal, global_light.direction);
        uint32_t triangle_flat_shaded_color = light_apply_intensity(mesh_face.color, light_intensity_dot);

        triangle_t projected_triangle = {
            .points = {projected_points[0], projected_points[1], projected_points[2]},
            .texcoords = {
                {mesh_face.a_uv.u, mesh_face.a_uv.v},
                {mesh_face.b_uv.u, mesh_face.b_uv.v},
                {mesh_face.c_uv.u, mesh_face.c_uv.v},
            },
            .color = triangle_flat_shaded_color,
            .avg_depth = avg_depth,
        };

        // Save the projected triangle in the array of triangles to render
        array_push(triangles_to_render, projected_triangle);
    }

    // Sort triangles to render by their avg_depth
    sort_triangles(triangles_to_render);
}

void render(void)
{
    draw_grid();

    // Loop all projected triangles and render them
    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++)
    {
        triangle_t triangle = triangles_to_render[i];

        if (render_mode == solid || render_mode == all)
        {
            // Draw filled triangle
            draw_filled_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y,
                triangle.color);
        }

        if (render_mode == textures || render_mode == all)
        {
            // draw_textured_triangle()
            draw_textured_triangle(
                // 1
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[0].z,
                triangle.points[0].w,
                triangle.texcoords[0].u,
                triangle.texcoords[0].v,

                // 2
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[1].z,
                triangle.points[1].w,
                triangle.texcoords[1].u,
                triangle.texcoords[1].v,

                // 3
                triangle.points[2].x,
                triangle.points[2].y,
                triangle.points[2].z,
                triangle.points[2].w,
                triangle.texcoords[2].u,
                triangle.texcoords[2].v,

                // Texture
                mesh_texture);
        }

        // Draw outline triangle
        if (render_mode == wireframe || render_mode == wireframe_verbose || render_mode == all)
        {
            draw_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y,
                0xFFFFFFFF);
        }

        if (render_mode == wireframe_verbose || render_mode == all)
        {
            // Draw vertex points
            draw_rect(triangle.points[0].x, triangle.points[0].y, 5, 5, 0xFFFF0000);
            draw_rect(triangle.points[1].x, triangle.points[1].y, 5, 5, 0xFFFF0000);
            draw_rect(triangle.points[2].x, triangle.points[2].y, 5, 5, 0xFFFF0000);
        }
    }

    // draw_filled_triangle(300, 100, 50, 250, 450, 500, 0xFF00FFFF);

    render_color_buffer();

    clear_color_buffer(0xFF000000);
    clear_z_buffer();

    SDL_RenderPresent(renderer);
}

void free_resources(void)
{
    array_free(mesh.vertices);
    array_free(mesh.faces);
    array_free(triangles_to_render);
    free(color_buffer);
    free(z_buffer);
    upng_free(png_texture);
}

int main(void)
{
    is_running = initialize_window();

    setup();

    while (is_running)
    {
        process_input();
        update();
        render();
    }

    destroy_window();
    free_resources();

    return 0;
}
