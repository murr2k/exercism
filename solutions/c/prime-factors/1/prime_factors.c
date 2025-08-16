#include "prime_factors.h"

size_t find_factors(uint64_t n, uint64_t factors[static MAXFACTORS])
{
    size_t count = 0;
    
    // Handle the case where n = 1
    if (n <= 1) {
        return 0;
    }
    
    // Check for factors starting from 2
    for (uint64_t factor = 2; factor * factor <= n && count < MAXFACTORS; factor++) {
        while (n % factor == 0 && count < MAXFACTORS) {
            factors[count++] = factor;
            n /= factor;
        }
    }
    
    // If n is still greater than 1, then it's a prime factor
    if (n > 1 && count < MAXFACTORS) {
        factors[count++] = n;
    }
    
    return count;
}
