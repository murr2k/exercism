#include "high_scores.h"
#include <stdlib.h>

int32_t latest(const int32_t *scores, size_t scores_len)
{
    if (scores_len == 0) {
        return 0;
    }
    return scores[scores_len - 1];
}

int32_t personal_best(const int32_t *scores, size_t scores_len)
{
    if (scores_len == 0) {
        return 0;
    }
    
    int32_t max = scores[0];
    for (size_t i = 1; i < scores_len; i++) {
        if (scores[i] > max) {
            max = scores[i];
        }
    }
    return max;
}

// Comparison function for qsort (descending order)
static int compare_desc(const void *a, const void *b)
{
    int32_t x = *(const int32_t*)a;
    int32_t y = *(const int32_t*)b;
    
    if (x > y) return -1;
    if (x < y) return 1;
    return 0;
}

size_t personal_top_three(const int32_t *scores, size_t scores_len,
                          int32_t *output)
{
    if (scores_len == 0) {
        return 0;
    }
    
    // Create a copy of the scores array to sort
    int32_t *temp = malloc(scores_len * sizeof(int32_t));
    if (temp == NULL) {
        return 0;
    }
    
    for (size_t i = 0; i < scores_len; i++) {
        temp[i] = scores[i];
    }
    
    // Sort in descending order
    qsort(temp, scores_len, sizeof(int32_t), compare_desc);
    
    // Copy the top scores (up to 3) to output
    size_t count = scores_len < 3 ? scores_len : 3;
    for (size_t i = 0; i < count; i++) {
        output[i] = temp[i];
    }
    
    free(temp);
    return count;
}
