#include "eliuds_eggs.h"

unsigned int egg_count(unsigned int number)
{
    unsigned int count = 0;
    
    // Count 1 bits by checking each bit position
    while (number > 0) {
        // If the least significant bit is 1, increment count
        if (number & 1) {
            count++;
        }
        // Right shift to check next bit
        number >>= 1;
    }
    
    return count;
}
