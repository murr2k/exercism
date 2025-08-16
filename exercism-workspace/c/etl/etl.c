#include "etl.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Comparison function for qsort
static int compare_new_map(const void *a, const void *b) {
    const new_map *map_a = (const new_map *)a;
    const new_map *map_b = (const new_map *)b;
    return map_a->key - map_b->key;
}

int convert(const legacy_map *input, const size_t input_len, new_map **output) {
    if (!input || !output) return 0;
    
    // First pass: count total letters
    size_t total_letters = 0;
    for (size_t i = 0; i < input_len; i++) {
        if (input[i].keys) {
            total_letters += strlen(input[i].keys);
        }
    }
    
    if (total_letters == 0) {
        *output = NULL;
        return 0;
    }
    
    // Allocate output array
    *output = malloc(total_letters * sizeof(new_map));
    if (!*output) return 0;
    
    // Second pass: convert each letter
    size_t output_index = 0;
    for (size_t i = 0; i < input_len; i++) {
        if (input[i].keys) {
            for (const char *c = input[i].keys; *c; c++) {
                (*output)[output_index].key = tolower(*c);
                (*output)[output_index].value = input[i].value;
                output_index++;
            }
        }
    }
    
    // Sort the output by key (alphabetical order)
    qsort(*output, total_letters, sizeof(new_map), compare_new_map);
    
    return (int)total_letters;
}
