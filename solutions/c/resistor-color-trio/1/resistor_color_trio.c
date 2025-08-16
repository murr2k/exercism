#include "resistor_color_trio.h"

resistor_value_t color_code(resistor_band_t colors[])
{
    resistor_value_t result;
    
    // Calculate base value from first two colors
    uint64_t base_value = colors[0] * 10 + colors[1];
    
    // Apply multiplier (add zeros) from third color
    uint64_t total_value = base_value;
    for (int i = 0; i < (int)colors[2]; i++) {
        total_value *= 10;
    }
    
    // Determine appropriate unit and value
    if (total_value >= 1000000000) {
        // Gigaohms
        result.value = total_value / 1000000000;
        result.unit = GIGAOHMS;
    } else if (total_value >= 1000000) {
        // Megaohms
        result.value = total_value / 1000000;
        result.unit = MEGAOHMS;
    } else if (total_value >= 1000) {
        // Kiloohms
        result.value = total_value / 1000;
        result.unit = KILOOHMS;
    } else {
        // Ohms
        result.value = total_value;
        result.unit = OHMS;
    }
    
    return result;
}
