#include "spiral_matrix.h"
#include <stdlib.h>

spiral_matrix_t *spiral_matrix_create(int size)
{
    spiral_matrix_t *spiral = malloc(sizeof(spiral_matrix_t));
    if (!spiral) {
        return NULL;
    }
    
    spiral->size = size;
    
    if (size == 0) {
        spiral->matrix = NULL;
        return spiral;
    }
    
    // Allocate array of row pointers
    spiral->matrix = malloc(size * sizeof(int*));
    if (!spiral->matrix) {
        free(spiral);
        return NULL;
    }
    
    // Allocate each row
    for (int i = 0; i < size; i++) {
        spiral->matrix[i] = malloc(size * sizeof(int));
        if (!spiral->matrix[i]) {
            // Cleanup previously allocated rows
            for (int j = 0; j < i; j++) {
                free(spiral->matrix[j]);
            }
            free(spiral->matrix);
            free(spiral);
            return NULL;
        }
    }
    
    // Fill the matrix in spiral order
    int top = 0, bottom = size - 1;
    int left = 0, right = size - 1;
    int num = 1;
    
    while (top <= bottom && left <= right) {
        // Fill top row from left to right
        for (int col = left; col <= right; col++) {
            spiral->matrix[top][col] = num++;
        }
        top++;
        
        // Fill right column from top to bottom
        for (int row = top; row <= bottom; row++) {
            spiral->matrix[row][right] = num++;
        }
        right--;
        
        // Fill bottom row from right to left (if we still have rows)
        if (top <= bottom) {
            for (int col = right; col >= left; col--) {
                spiral->matrix[bottom][col] = num++;
            }
            bottom--;
        }
        
        // Fill left column from bottom to top (if we still have columns)
        if (left <= right) {
            for (int row = bottom; row >= top; row--) {
                spiral->matrix[row][left] = num++;
            }
            left++;
        }
    }
    
    return spiral;
}

void spiral_matrix_destroy(spiral_matrix_t *spiral_matrix)
{
    if (!spiral_matrix) {
        return;
    }
    
    if (spiral_matrix->matrix) {
        for (int i = 0; i < spiral_matrix->size; i++) {
            free(spiral_matrix->matrix[i]);
        }
        free(spiral_matrix->matrix);
    }
    
    free(spiral_matrix);
}
