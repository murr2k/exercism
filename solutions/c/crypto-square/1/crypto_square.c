#include "crypto_square.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

char *ciphertext(const char *input) {
    if (!input) return NULL;
    
    // First pass: count alphanumeric characters
    int normalized_len = 0;
    for (const char *p = input; *p; p++) {
        if (isalnum(*p)) {
            normalized_len++;
        }
    }
    
    // Handle empty normalized text
    if (normalized_len == 0) {
        char *result = malloc(1);
        if (!result) return NULL;
        result[0] = '\0';
        return result;
    }
    
    // Second pass: normalize text (lowercase alphanumeric only)
    char *normalized = malloc(normalized_len + 1);
    if (!normalized) return NULL;
    
    int idx = 0;
    for (const char *p = input; *p; p++) {
        if (isalnum(*p)) {
            normalized[idx++] = tolower(*p);
        }
    }
    normalized[normalized_len] = '\0';
    
    // Calculate rectangle dimensions
    // We need c >= r and c - r <= 1 and r * c >= normalized_len
    // This means c = ceil(sqrt(normalized_len))
    int c = (int)ceil(sqrt(normalized_len));
    int r = (normalized_len + c - 1) / c;  // This is ceil(normalized_len / c)
    
    // Ensure c >= r
    while (c < r) {
        c++;
        r = (normalized_len + c - 1) / c;
    }
    
    // Create the cipher by reading columns
    // Result will be c chunks of r characters each, separated by spaces
    // Total length: c * r + (c - 1) spaces + null terminator
    int result_len = c * r + (c - 1) + 1;
    char *result = malloc(result_len);
    if (!result) {
        free(normalized);
        return NULL;
    }
    
    int result_idx = 0;
    
    // Read each column
    for (int col = 0; col < c; col++) {
        if (col > 0) {
            result[result_idx++] = ' '; // Space between chunks
        }
        
        // Read down this column
        for (int row = 0; row < r; row++) {
            int pos = row * c + col;
            if (pos < normalized_len) {
                result[result_idx++] = normalized[pos];
            } else {
                result[result_idx++] = ' '; // Padding
            }
        }
    }
    
    result[result_idx] = '\0';
    
    free(normalized);
    return result;
}
