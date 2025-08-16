#include "luhn.h"
#include <string.h>
#include <ctype.h>

bool luhn(const char *num) {
    if (!num) return false;
    
    // First pass: validate characters and count digits
    int digit_count = 0;
    for (int i = 0; num[i] != '\0'; i++) {
        if (isdigit(num[i])) {
            digit_count++;
        } else if (num[i] != ' ') {
            // Invalid character (not digit or space)
            return false;
        }
    }
    
    // Strings with 1 or fewer digits are invalid
    if (digit_count <= 1) return false;
    
    // Second pass: apply Luhn algorithm
    int sum = 0;
    int position_from_right = 0;
    
    // Process from right to left
    for (int i = strlen(num) - 1; i >= 0; i--) {
        if (isdigit(num[i])) {
            int digit = num[i] - '0';
            
            // Every second digit from the right (position 1, 3, 5, ...)
            if (position_from_right % 2 == 1) {
                digit *= 2;
                if (digit > 9) {
                    digit -= 9;
                }
            }
            
            sum += digit;
            position_from_right++;
        }
        // Skip spaces
    }
    
    return sum % 10 == 0;
}
