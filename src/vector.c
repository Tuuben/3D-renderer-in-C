#include "vector.h"
#include <math.h>

// TODO: add implementation functions

vec3_t vec3_rotate_x(vec3_t vec, float angle)
{
    vec3_t rotated_vector = {
        .x = vec.x,
        .y = vec.y * cos(angle) - vec.z * sin(angle),
        .z = vec.y * sin(angle) + vec.z * cos(angle),
    };

    return rotated_vector;
}

vec3_t vec3_rotate_y(vec3_t vec, float angle)
{
    vec3_t rotated_vector = {
        .x = vec.x * cos(angle) - vec.z * sin(angle),
        .y = vec.y,
        .z = vec.x * sin(angle) + vec.z * cos(angle),
    };

    return rotated_vector;
}

vec3_t vec3_rotate_z(vec3_t vec, float angle)
{
    vec3_t rotated_vector = {
        .x = vec.x * cos(angle) - vec.y * sin(angle),
        .y = vec.x * sin(angle) + vec.y * cos(angle),
        .z = vec.z,
    };

    return rotated_vector;
}

/**
 * Vec2 functions
 */
float vec2_length(vec2_t v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}

vec2_t vec2_add(vec2_t a, vec2_t b)
{
    vec2_t result = {.x = a.x + b.x, .y = a.y + b.y};
    return result;
}

vec2_t vec2_sub(vec2_t a, vec2_t b)
{
    vec2_t result = {.x = a.x - b.x, .y = a.y - b.y};
    return result;
}

vec2_t vec2_mul(vec2_t v, float factor)
{
    vec2_t result = {.x = v.x * factor, .y = v.y * factor};
    return result;
}

vec2_t vec2_div(vec2_t v, float factor)
{
    vec2_t result = {
        .x = v.x / factor,
        .y = v.y / factor,
    };
    return result;
}

float vec2_dot(vec2_t a, vec2_t b)
{
    float result = (a.x * b.x) + (a.y * b.y);
    return result;
}

void vec2_normalize(vec2_t *v)
{
    float length = vec2_length(*v);
    v->x /= length;
    v->y /= length;
}

vec2_t vec2_from_vec4(vec4_t v)
{
    vec2_t result = {v.x, v.y};
    return result;
}

/**
 * Vec3 Functions
 */
float vec3_length(vec3_t v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3_t vec3_add(vec3_t a, vec3_t b)
{
    vec3_t result = {.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
    return result;
}

vec3_t vec3_sub(vec3_t a, vec3_t b)
{
    vec3_t result = {.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z};
    return result;
}

vec3_t vec3_mul(vec3_t v, float factor)
{
    vec3_t result = {
        .x = v.x * factor,
        .y = v.y * factor,
        .z = v.z * factor,
    };
    return result;
}

vec3_t vec3_div(vec3_t v, float factor)
{
    vec3_t result = {
        .x = v.x / factor,
        .y = v.y / factor,
        .z = v.z / factor,
    };
    return result;
}

vec3_t vec3_cross(vec3_t a, vec3_t b)
{
    vec3_t result = {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x,
    };

    return result;
}

float vec3_dot(vec3_t a, vec3_t b)
{
    float result = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
    return result;
}

void vec3_normalize(vec3_t *v)
{
    float length = vec3_length(*v);
    v->x /= length;
    v->y /= length;
    v->z /= length;
}

vec3_t vec3_from_vec4(vec4_t v)
{
    vec3_t result = {
        v.x,
        v.y,
        v.z,
    };
    return result;
}

// VECTOR 4
vec4_t vec4_from_vec3(vec3_t v)
{
    vec4_t result = {
        v.x,
        v.y,
        v.z,
        1,
    };

    return result;
}
