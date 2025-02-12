#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "texture.h"

typedef struct
{
    int a;
    int b;
    int c;
    tex2_t a_uv;
    tex2_t b_uv;
    tex2_t c_uv;
    uint32_t color;
} face_t;

typedef struct
{
    vec4_t points[3];
    tex2_t texcoords[3];
    uint32_t color;
    float avg_depth;
} triangle_t;

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void sort_triangles(triangle_t *array);

void draw_texel(int x, int y, uint32_t *texture,                           // texture
                vec4_t point_a, vec4_t point_b, vec4_t point_c,            // vertex points
                float u0, float v0, float u1, float v1, float u2, float v2 // UV coords
);

void draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0, // 1
    int x1, int y1, float z1, float w1, float u1, float v1, // 2
    int x2, int y2, float z2, float w2, float u2, float v2, // 3
    uint32_t *texture);

#endif
