#include "resistor_color_duo.h"

uint16_t color_code(resistor_band_t colors[])
{
    // Only use the first two colors to form a two-digit number
    // First color is tens digit, second color is ones digit
    return colors[0] * 10 + colors[1];
}
