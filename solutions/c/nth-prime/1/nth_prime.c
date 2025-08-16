#include "nth_prime.h"
#include <stdbool.h>

static bool is_prime(uint32_t num) {
    if (num < 2) return false;
    if (num == 2) return true;
    if (num % 2 == 0) return false;
    
    // Check odd divisors up to sqrt(num)
    for (uint32_t i = 3; i * i <= num; i += 2) {
        if (num % i == 0) return false;
    }
    
    return true;
}

uint32_t nth(uint32_t n) {
    if (n == 0) return 0;  // There is no zeroth prime
    
    uint32_t count = 0;
    uint32_t candidate = 2;
    
    while (count < n) {
        if (is_prime(candidate)) {
            count++;
            if (count == n) {
                return candidate;
            }
        }
        
        // After checking 2, only check odd numbers
        if (candidate == 2) {
            candidate = 3;
        } else {
            candidate += 2;
        }
    }
    
    return 0;  // Should never reach here
}
