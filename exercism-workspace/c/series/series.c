#include "series.h"
#include <stdlib.h>
#include <string.h>

slices_t slices(char *input_text, unsigned int substring_length) {
    slices_t result = {0, NULL};
    
    // Handle edge cases
    if (input_text == NULL || substring_length == 0 || 
        strlen(input_text) == 0 || substring_length > strlen(input_text)) {
        return result;
    }
    
    size_t input_length = strlen(input_text);
    unsigned int count = input_length - substring_length + 1;
    
    // Allocate array of string pointers
    result.substring = malloc(count * sizeof(char*));
    if (!result.substring) {
        return result;
    }
    
    // Generate all substrings
    for (unsigned int i = 0; i < count; i++) {
        // Allocate memory for each substring
        result.substring[i] = malloc((substring_length + 1) * sizeof(char));
        if (!result.substring[i]) {
            // Free previously allocated substrings on failure
            for (unsigned int j = 0; j < i; j++) {
                free(result.substring[j]);
            }
            free(result.substring);
            result.substring = NULL;
            return result;
        }
        
        // Copy substring
        strncpy(result.substring[i], input_text + i, substring_length);
        result.substring[i][substring_length] = '\0';
    }
    
    result.substring_count = count;
    return result;
}
