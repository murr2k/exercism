#include "binary.h"
#include <string.h>
#include <stdbool.h>

int convert(const char *input)
{
    if (input == NULL || strlen(input) == 0) {
        return INVALID;
    }
    
    int len = strlen(input);
    int result = 0;
    int power = 1;  // Represents 2^0, 2^1, 2^2, etc.
    
    // Validate input - only '0' and '1' are allowed
    for (int i = 0; i < len; i++) {
        if (input[i] != '0' && input[i] != '1') {
            return INVALID;
        }
    }
    
    // Convert from right to left
    for (int i = len - 1; i >= 0; i--) {
        if (input[i] == '1') {
            result += power;
        }
        power *= 2;
    }
    
    return result;
}
