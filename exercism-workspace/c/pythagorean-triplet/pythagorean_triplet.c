#include "pythagorean_triplet.h"
#include <stdlib.h>
#include <stdbool.h>

triplets_t *triplets_with_sum(uint16_t sum) {
    triplets_t *result = malloc(sizeof(triplets_t));
    if (!result) return NULL;
    
    result->count = 0;
    result->triplets = NULL;
    
    // Initial allocation for triplets array
    size_t capacity = 10;
    result->triplets = malloc(capacity * sizeof(triplet_t));
    if (!result->triplets) {
        free(result);
        return NULL;
    }
    
    // Find all pythagorean triplets where a + b + c = sum
    // We need a < b < c and a^2 + b^2 = c^2
    // Also a + b + c = sum, so c = sum - a - b
    // For a < b < c to hold: a < b and b < c = sum - a - b
    // This gives us: b < sum - a - b, so 2b < sum - a, so b < (sum - a)/2
    // Also, for c to be positive and greater than b: sum - a - b > b, so b < (sum - a)/2
    
    for (uint16_t a = 1; a <= sum / 3; a++) {
        for (uint16_t b = a + 1; b <= (sum - a) / 2; b++) {
            uint16_t c = sum - a - b;
            
            // Ensure a < b < c
            if (b >= c) continue;
            
            // Check if it's a valid pythagorean triplet
            if (a * a + b * b == c * c) {
                // Expand array if needed
                if (result->count >= capacity) {
                    capacity *= 2;
                    triplet_t *new_triplets = realloc(result->triplets, capacity * sizeof(triplet_t));
                    if (!new_triplets) {
                        free_triplets(result);
                        return NULL;
                    }
                    result->triplets = new_triplets;
                }
                
                // Add the triplet
                result->triplets[result->count].a = a;
                result->triplets[result->count].b = b;
                result->triplets[result->count].c = c;
                result->count++;
            }
        }
    }
    
    // Shrink array to actual size
    if (result->count > 0) {
        triplet_t *final_triplets = realloc(result->triplets, result->count * sizeof(triplet_t));
        if (final_triplets) {
            result->triplets = final_triplets;
        }
    } else {
        free(result->triplets);
        result->triplets = NULL;
    }
    
    return result;
}

void free_triplets(triplets_t *triplets) {
    if (triplets) {
        free(triplets->triplets);
        free(triplets);
    }
}
