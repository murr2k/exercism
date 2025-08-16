#include "pascals_triangle.h"
#include <stdlib.h>

uint8_t **create_triangle(size_t rows) {
    // Special case: 0 rows returns one row with a single zero
    if (rows == 0) {
        uint8_t **triangle = malloc(sizeof(uint8_t*));
        if (!triangle) return NULL;
        
        triangle[0] = malloc(sizeof(uint8_t));
        if (!triangle[0]) {
            free(triangle);
            return NULL;
        }
        triangle[0][0] = 0;
        return triangle;
    }
    
    // Allocate array of row pointers
    uint8_t **triangle = malloc(rows * sizeof(uint8_t*));
    if (!triangle) return NULL;
    
    // Allocate each row and compute Pascal's triangle values
    for (size_t i = 0; i < rows; i++) {
        triangle[i] = calloc(rows, sizeof(uint8_t)); // calloc initializes to 0
        if (!triangle[i]) {
            // Free previously allocated rows on failure
            for (size_t j = 0; j < i; j++) {
                free(triangle[j]);
            }
            free(triangle);
            return NULL;
        }
        
        // Fill in Pascal's triangle values for this row
        for (size_t j = 0; j <= i; j++) {
            if (j == 0 || j == i) {
                // First and last elements are always 1
                triangle[i][j] = 1;
            } else {
                // Sum of the two elements above
                triangle[i][j] = triangle[i-1][j-1] + triangle[i-1][j];
            }
        }
        // The rest of the row is already 0 from calloc
    }
    
    return triangle;
}

void free_triangle(uint8_t **triangle, size_t rows) {
    if (!triangle) return;
    
    for (size_t i = 0; i < rows; i++) {
        if (triangle[i]) {
            free(triangle[i]);
        }
    }
    free(triangle);
}
