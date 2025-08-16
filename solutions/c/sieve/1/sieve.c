#include "sieve.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

uint32_t sieve(uint32_t limit, uint32_t *primes, size_t max_primes)
{
    if (limit < 2 || max_primes == 0) {
        return 0;
    }
    
    // Create sieve array (false means prime)
    bool *is_composite = calloc(limit + 1, sizeof(bool));
    if (!is_composite) {
        return 0;
    }
    
    // Sieve of Eratosthenes
    for (uint32_t i = 2; i * i <= limit; i++) {
        if (!is_composite[i]) {
            // Mark all multiples of i as composite
            for (uint32_t j = i * i; j <= limit; j += i) {
                is_composite[j] = true;
            }
        }
    }
    
    // Collect primes
    uint32_t count = 0;
    for (uint32_t i = 2; i <= limit && count < max_primes; i++) {
        if (!is_composite[i]) {
            primes[count++] = i;
        }
    }
    
    free(is_composite);
    return count;
}