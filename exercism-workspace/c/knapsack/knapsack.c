#include "knapsack.h"
#include <stdlib.h>
#include <string.h>

unsigned int maximum_value(unsigned int maximum_weight, item_t *items, size_t item_count) {
    if (item_count == 0 || maximum_weight == 0) {
        return 0;
    }
    
    // Create DP table: dp[i][w] = maximum value using first i items with weight limit w
    // We'll use 1D array optimization: dp[w] = maximum value with weight limit w
    unsigned int *dp = calloc(maximum_weight + 1, sizeof(unsigned int));
    if (!dp) {
        return 0;
    }
    
    // For each item
    for (size_t i = 0; i < item_count; i++) {
        unsigned int item_weight = items[i].weight;
        unsigned int item_value = items[i].value;
        
        // Process weights in reverse order to avoid using updated values in same iteration
        for (int w = (int)maximum_weight; w >= (int)item_weight; w--) {
            unsigned int value_with_item = dp[w - item_weight] + item_value;
            if (value_with_item > dp[w]) {
                dp[w] = value_with_item;
            }
        }
    }
    
    unsigned int result = dp[maximum_weight];
    free(dp);
    return result;
}
