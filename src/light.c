#include "light.h"

light_t global_light = {
    .direction = {.x = 0, .y = 0, .z = 1},
};

uint32_t light_apply_intensity(uint32_t original_color, float intensity_percentage)
{
    if (intensity_percentage < 0)
        intensity_percentage = 0.0;
    if (intensity_percentage > 1.0)
        intensity_percentage = 1.0;

    uint32_t r_base = 0x00FF0000;
    uint32_t g_base = 0x0000FF00;
    uint32_t b_base = 0x000000FF;

    uint32_t a = (original_color & 0xFF000000);
    uint32_t r = (original_color & r_base) * intensity_percentage;
    uint32_t g = (original_color & g_base) * intensity_percentage;
    uint32_t b = (original_color & b_base) * intensity_percentage;

    uint32_t new_color = a | (r & r_base) | (g & g_base) | (b & b_base);

    return new_color;
}