#ifndef LIGHT_H
#define LIGHT_H
#include "vector.h"
#include <stdint.h>

typedef struct
{
    vec3_t direction;
} light_t;

extern light_t global_light;

uint32_t light_apply_intensity(uint32_t original_color, float intensity_percentage);

#endif