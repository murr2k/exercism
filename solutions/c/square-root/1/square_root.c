#include "square_root.h"
#include <stdint.h>

uint16_t square_root(uint16_t number)
{
    if (number == 0) return 0;
    if (number == 1) return 1;
    
    // Using binary search for integer square root
    uint16_t left = 1;
    uint16_t right = number / 2;
    uint16_t result = 0;
    
    while (left <= right) {
        uint16_t mid = left + (right - left) / 2;
        
        // Check if mid * mid == number
        if (mid <= number / mid) {
            left = mid + 1;
            result = mid;
        } else {
            right = mid - 1;
        }
    }
    
    return result;
}