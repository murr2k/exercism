#include "run_length_encoding.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char *encode(const char *text) {
    if (!text || !*text) {
        char *result = malloc(1);
        if (result) result[0] = '\0';
        return result;
    }
    
    size_t len = strlen(text);
    // Worst case: each character becomes "9c" (2 chars), but we could have larger counts
    char *result = malloc(len * 10 + 1);  // Conservative allocation
    if (!result) return NULL;
    
    size_t result_pos = 0;
    size_t i = 0;
    
    while (i < len) {
        char current_char = text[i];
        size_t count = 1;
        
        // Count consecutive occurrences
        while (i + count < len && text[i + count] == current_char) {
            count++;
        }
        
        // Add to result
        if (count == 1) {
            result[result_pos++] = current_char;
        } else {
            result_pos += sprintf(result + result_pos, "%zu%c", count, current_char);
        }
        
        i += count;
    }
    
    result[result_pos] = '\0';
    
    // Resize to actual needed size
    char *final_result = realloc(result, result_pos + 1);
    return final_result ? final_result : result;
}

char *decode(const char *data) {
    if (!data || !*data) {
        char *result = malloc(1);
        if (result) result[0] = '\0';
        return result;
    }
    
    size_t len = strlen(data);
    // Worst case: every character could be preceded by a large number
    char *result = malloc(len * 100 + 1);  // Conservative allocation
    if (!result) return NULL;
    
    size_t result_pos = 0;
    size_t i = 0;
    
    while (i < len) {
        size_t count = 0;
        
        // Read the count (if any)
        while (i < len && data[i] >= '0' && data[i] <= '9') {
            count = count * 10 + (data[i] - '0');
            i++;
        }
        
        // If no count was read, default to 1
        if (count == 0) count = 1;
        
        // Get the character to repeat
        if (i < len) {
            char char_to_repeat = data[i];
            
            // Add repeated characters to result
            for (size_t j = 0; j < count; j++) {
                result[result_pos++] = char_to_repeat;
            }
            
            i++;
        }
    }
    
    result[result_pos] = '\0';
    
    // Resize to actual needed size
    char *final_result = realloc(result, result_pos + 1);
    return final_result ? final_result : result;
}
