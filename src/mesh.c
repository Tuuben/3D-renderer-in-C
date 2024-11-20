#include "mesh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    {.x = -1, .y = -1, .z = -1}, // 1
    {.x = -1, .y = 1, .z = -1},  // 2
    {.x = 1, .y = 1, .z = -1},   // 3
    {.x = 1, .y = -1, .z = -1},  // 4
    {.x = 1, .y = 1, .z = 1},    // 5
    {.x = 1, .y = -1, .z = 1},   // 6
    {.x = -1, .y = 1, .z = 1},   // 7
    {.x = -1, .y = -1, .z = 1}   // 8
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    {.a = 1, .b = 2, .c = 3, .a_uv = {0, 1}, .b_uv = {0, 0}, .c_uv = {1, 0}, .color = 0xFFFFFFFF},
    {.a = 1, .b = 3, .c = 4, .a_uv = {0, 1}, .b_uv = {1, 0}, .c_uv = {1, 1}, .color = 0xFFFFFFFF},
    // right
    {.a = 4, .b = 3, .c = 5, .a_uv = {0, 1}, .b_uv = {0, 0}, .c_uv = {1, 0}, .color = 0xFFFFFFFF},
    {.a = 4, .b = 5, .c = 6, .a_uv = {0, 1}, .b_uv = {1, 0}, .c_uv = {1, 1}, .color = 0xFFFFFFFF},
    // back
    {.a = 6, .b = 5, .c = 7, .a_uv = {0, 1}, .b_uv = {0, 0}, .c_uv = {1, 0}, .color = 0xFFFFFFFF},
    {.a = 6, .b = 7, .c = 8, .a_uv = {0, 1}, .b_uv = {1, 0}, .c_uv = {1, 1}, .color = 0xFFFFFFFF},
    // left
    {.a = 8, .b = 7, .c = 2, .a_uv = {0, 1}, .b_uv = {0, 0}, .c_uv = {1, 0}, .color = 0xFFFFFFFF},
    {.a = 8, .b = 2, .c = 1, .a_uv = {0, 1}, .b_uv = {1, 0}, .c_uv = {1, 1}, .color = 0xFFFFFFFF},
    // top
    {.a = 2, .b = 7, .c = 5, .a_uv = {0, 1}, .b_uv = {0, 0}, .c_uv = {1, 0}, .color = 0xFFFFFFFF},
    {.a = 2, .b = 5, .c = 3, .a_uv = {0, 1}, .b_uv = {1, 0}, .c_uv = {1, 1}, .color = 0xFFFFFFFF},
    // bottom
    {.a = 6, .b = 8, .c = 1, .a_uv = {0, 1}, .b_uv = {0, 0}, .c_uv = {1, 0}, .color = 0xFFFFFFFF},
    {.a = 6, .b = 1, .c = 4, .a_uv = {0, 1}, .b_uv = {1, 0}, .c_uv = {1, 1}, .color = 0xFFFFFFFF},
};

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = {.x = 0, .y = 0, .z = 0},
    .scale = {1, 1, 1},
    .translation = {0, 0, 0},
};

void load_cube_mesh_data(void)
{
    for (int i = 0; i < N_CUBE_VERTICES; i++)
    {
        array_push(mesh.vertices, cube_vertices[i]);
    }

    for (int i = 0; i < N_CUBE_FACES; i++)
    {
        array_push(mesh.faces, cube_faces[i]);
    }
}

void load_obj_file_data(char *filename)
{
    FILE *file;
    file = fopen(filename, "r");
    char line[1024];

    tex2_t *texcoords = NULL;

    while (fgets(line, 1024, file))
    {
        if (strncmp(line, "v ", 2) == 0)
        {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
        }

        if (strncmp(line, "vt ", 3) == 0)
        {
            tex2_t texcoord;
            sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v);
            array_push(texcoords, texcoord);
        }

        if (strncmp(line, "f ", 2) == 0)
        {
            int vertex_indicies[3];
            int texture_indicies[3];
            int normal_indicies[3];

            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                   &vertex_indicies[0], &texture_indicies[0], &normal_indicies[0],
                   &vertex_indicies[1], &texture_indicies[1], &normal_indicies[1],
                   &vertex_indicies[2], &texture_indicies[2], &normal_indicies[2]);

            face_t face = {
                .a = vertex_indicies[0] - 1,
                .b = vertex_indicies[1] - 1,
                .c = vertex_indicies[2] - 1,
                .a_uv = texcoords[texture_indicies[0] - 1],
                .b_uv = texcoords[texture_indicies[1] - 1],
                .c_uv = texcoords[texture_indicies[2] - 1],
                .color = 0xFFFFFFFF,
            };

            array_push(mesh.faces, face);
        }
    }

    array_free(texcoords);
}
