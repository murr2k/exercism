#include "largest_series_product.h"
#include <string.h>
#include <ctype.h>

int64_t largest_series_product(char *digits, size_t span) {
    // Handle edge cases
    if (!digits) return -1;
    
    size_t len = strlen(digits);
    
    // Check for negative span (though size_t is unsigned, so this handles SIZE_MAX case)
    if (span > len) return -1;
    
    // Check for empty string with non-zero span
    if (len == 0 && span > 0) return -1;
    
    // Check for span of 0 - technically should return 1 (empty product)
    if (span == 0) return 1;
    
    // Validate all characters are digits
    for (size_t i = 0; i < len; i++) {
        if (!isdigit(digits[i])) return -1;
    }
    
    int64_t max_product = 0;
    
    // Calculate products for all possible spans
    for (size_t i = 0; i <= len - span; i++) {
        int64_t product = 1;
        
        // Calculate product for current span
        for (size_t j = i; j < i + span; j++) {
            int digit = digits[j] - '0';
            product *= digit;
        }
        
        // Update maximum
        if (product > max_product) {
            max_product = product;
        }
    }
    
    return max_product;
}
