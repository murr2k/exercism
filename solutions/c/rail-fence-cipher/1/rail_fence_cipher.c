#include "rail_fence_cipher.h"
#include <stdlib.h>
#include <string.h>

char *encode(char *text, size_t rails) {
    if (rails == 1) {
        char *result = malloc(strlen(text) + 1);
        strcpy(result, text);
        return result;
    }
    
    size_t len = strlen(text);
    
    // Create a 2D array to represent the rails
    char **fence = malloc(rails * sizeof(char*));
    for (size_t i = 0; i < rails; i++) {
        fence[i] = malloc((len + 1) * sizeof(char));
        memset(fence[i], '\0', len + 1);
    }
    
    // Variables to track current position and direction
    size_t rail = 0;
    int direction = 1;  // 1 for down, -1 for up
    
    // Place characters on the fence
    for (size_t pos = 0; pos < len; pos++) {
        fence[rail][pos] = text[pos];
        
        // Change direction when we hit the top or bottom rail
        if (rail == 0) {
            direction = 1;
        } else if (rail == rails - 1) {
            direction = -1;
        }
        
        rail += direction;
    }
    
    // Read off the rails to create the encoded string
    char *result = malloc(len + 1);
    size_t result_pos = 0;
    
    for (size_t r = 0; r < rails; r++) {
        for (size_t pos = 0; pos < len; pos++) {
            if (fence[r][pos] != '\0') {
                result[result_pos++] = fence[r][pos];
            }
        }
    }
    result[result_pos] = '\0';
    
    // Clean up
    for (size_t i = 0; i < rails; i++) {
        free(fence[i]);
    }
    free(fence);
    
    return result;
}

char *decode(char *ciphertext, size_t rails) {
    if (rails == 1) {
        char *result = malloc(strlen(ciphertext) + 1);
        strcpy(result, ciphertext);
        return result;
    }
    
    size_t len = strlen(ciphertext);
    
    // Create a 2D array to represent the rails
    char **fence = malloc(rails * sizeof(char*));
    for (size_t i = 0; i < rails; i++) {
        fence[i] = malloc((len + 1) * sizeof(char));
        memset(fence[i], '\0', len + 1);
    }
    
    // First, mark the positions that should be filled
    size_t rail = 0;
    int direction = 1;
    
    for (size_t pos = 0; pos < len; pos++) {
        fence[rail][pos] = '*';  // Mark position to be filled
        
        if (rail == 0) {
            direction = 1;
        } else if (rail == rails - 1) {
            direction = -1;
        }
        
        rail += direction;
    }
    
    // Now fill the marked positions with characters from ciphertext
    size_t cipher_pos = 0;
    
    for (size_t r = 0; r < rails; r++) {
        for (size_t pos = 0; pos < len; pos++) {
            if (fence[r][pos] == '*' && cipher_pos < len) {
                fence[r][pos] = ciphertext[cipher_pos++];
            }
        }
    }
    
    // Read the message in zigzag order
    char *result = malloc(len + 1);
    size_t result_pos = 0;
    rail = 0;
    direction = 1;
    
    for (size_t pos = 0; pos < len; pos++) {
        result[result_pos++] = fence[rail][pos];
        
        if (rail == 0) {
            direction = 1;
        } else if (rail == rails - 1) {
            direction = -1;
        }
        
        rail += direction;
    }
    result[result_pos] = '\0';
    
    // Clean up
    for (size_t i = 0; i < rails; i++) {
        free(fence[i]);
    }
    free(fence);
    
    return result;
}
