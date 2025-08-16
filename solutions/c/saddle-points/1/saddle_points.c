#include "saddle_points.h"
#include <stdlib.h>
#include <stdbool.h>

saddle_points_t *saddle_points(size_t rows, size_t cols, uint8_t matrix[][cols]) {
    saddle_points_t *result = malloc(sizeof(saddle_points_t));
    if (!result) return NULL;
    
    result->count = 0;
    result->points = NULL;
    
    // Handle empty matrix
    if (rows == 0 || cols == 0) {
        return result;
    }
    
    // Temporary array to store potential saddle points
    saddle_point_t *temp_points = malloc(rows * cols * sizeof(saddle_point_t));
    if (!temp_points) {
        free(result);
        return NULL;
    }
    
    size_t count = 0;
    
    // Check each position in the matrix
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < cols; c++) {
            uint8_t value = matrix[r][c];
            bool is_saddle = true;
            
            // Check if it's maximum in its row
            for (size_t col = 0; col < cols; col++) {
                if (matrix[r][col] > value) {
                    is_saddle = false;
                    break;
                }
            }
            
            // Check if it's minimum in its column (only if still candidate)
            if (is_saddle) {
                for (size_t row = 0; row < rows; row++) {
                    if (matrix[row][c] < value) {
                        is_saddle = false;
                        break;
                    }
                }
            }
            
            if (is_saddle) {
                temp_points[count].row = r + 1;    // 1-indexed as shown in tests
                temp_points[count].column = c + 1; // 1-indexed as shown in tests
                count++;
            }
        }
    }
    
    // Allocate final array with correct size
    if (count > 0) {
        result->points = malloc(count * sizeof(saddle_point_t));
        if (!result->points) {
            free(temp_points);
            free(result);
            return NULL;
        }
        
        // Copy points to final array
        for (size_t i = 0; i < count; i++) {
            result->points[i] = temp_points[i];
        }
    }
    
    result->count = count;
    free(temp_points);
    
    return result;
}

void free_saddle_points(saddle_points_t *saddle_points) {
    if (saddle_points) {
        if (saddle_points->points) {
            free(saddle_points->points);
        }
        free(saddle_points);
    }
}
