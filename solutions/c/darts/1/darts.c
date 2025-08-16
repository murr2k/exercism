#include "darts.h"
#include <math.h>

uint8_t score(coordinate_t landing_position)
{
    float distance = sqrt(landing_position.x * landing_position.x + 
                         landing_position.y * landing_position.y);
    
    if (distance <= 1.0) {
        return 10;
    } else if (distance <= 5.0) {
        return 5;
    } else if (distance <= 10.0) {
        return 1;
    } else {
        return 0;
    }
}