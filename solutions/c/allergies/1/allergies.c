#include "allergies.h"
#include <string.h>

bool is_allergic_to(allergen_t allergen, int score)
{
    // Each allergen has a power of 2 value
    // ALLERGEN_EGGS = 0 -> 2^0 = 1
    // ALLERGEN_PEANUTS = 1 -> 2^1 = 2
    // ALLERGEN_SHELLFISH = 2 -> 2^2 = 4
    // etc.
    
    // Check if the bit for this allergen is set in the score
    return (score & (1 << allergen)) != 0;
}

allergen_list_t get_allergens(int score)
{
    allergen_list_t list;
    list.count = 0;
    
    // Initialize all allergens to false
    memset(list.allergens, false, sizeof(list.allergens));
    
    // Only consider the first 8 bits (allergens 0-7)
    // Ignore any higher bits
    score = score & 0xFF;
    
    // Check each allergen
    for (int i = 0; i < ALLERGEN_COUNT; i++) {
        if (is_allergic_to(i, score)) {
            list.allergens[i] = true;
            list.count++;
        }
    }
    
    return list;
}