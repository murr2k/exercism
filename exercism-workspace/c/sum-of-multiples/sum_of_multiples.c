#include "sum_of_multiples.h"
#include <stdbool.h>

unsigned int sum(const unsigned int *factors, const size_t number_of_factors,
                 const unsigned int limit) {
    if (!factors || number_of_factors == 0 || limit <= 1) {
        return 0;
    }
    
    unsigned int total = 0;
    
    // Check each number from 1 to limit-1
    for (unsigned int i = 1; i < limit; i++) {
        bool is_multiple = false;
        
        // Check if i is a multiple of any factor
        for (size_t j = 0; j < number_of_factors; j++) {
            if (factors[j] != 0 && i % factors[j] == 0) {
                is_multiple = true;
                break;
            }
        }
        
        if (is_multiple) {
            total += i;
        }
    }
    
    return total;
}
