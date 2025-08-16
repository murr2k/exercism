#include "flower_field.h"
#include <stdlib.h>
#include <string.h>

char **annotate(const char **garden, const size_t rows) {
    if (rows == 0 || garden == NULL) {
        return NULL;
    }
    
    // Get the number of columns from the first row
    size_t cols = strlen(garden[0]);
    
    // Allocate memory for the result - add one extra for NULL terminator
    char **result = malloc((rows + 1) * sizeof(char *));
    if (result == NULL) {
        return NULL;
    }
    
    for (size_t i = 0; i < rows; i++) {
        result[i] = malloc((cols + 1) * sizeof(char));
        if (result[i] == NULL) {
            // Free previously allocated memory
            for (size_t j = 0; j < i; j++) {
                free(result[j]);
            }
            free(result);
            return NULL;
        }
        strcpy(result[i], garden[i]);
    }
    result[rows] = NULL; // NULL terminate the array
    
    // Count adjacent flowers for each empty space
    for (size_t row = 0; row < rows; row++) {
        for (size_t col = 0; col < cols; col++) {
            if (garden[row][col] == ' ') {
                int count = 0;
                
                // Check all 8 adjacent positions
                for (int dr = -1; dr <= 1; dr++) {
                    for (int dc = -1; dc <= 1; dc++) {
                        if (dr == 0 && dc == 0) continue; // Skip current position
                        
                        int new_row = (int)row + dr;
                        int new_col = (int)col + dc;
                        
                        // Check bounds
                        if (new_row >= 0 && new_row < (int)rows && 
                            new_col >= 0 && new_col < (int)cols) {
                            if (garden[new_row][new_col] == '*') {
                                count++;
                            }
                        }
                    }
                }
                
                // Set the count in the result
                if (count > 0) {
                    result[row][col] = '0' + count;
                }
            }
        }
    }
    
    return result;
}

void free_annotation(char **annotation) {
    if (annotation == NULL) {
        return;
    }
    
    // Free each row, using the NULL terminator we added
    for (size_t i = 0; annotation[i] != NULL; i++) {
        free(annotation[i]);
    }
    
    // Free the array of pointers
    free(annotation);
}
