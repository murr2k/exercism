#include "atbash_cipher.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char atbash_transform(char c) {
    if (c >= 'a' && c <= 'z') {
        return 'z' - (c - 'a');
    }
    if (c >= 'A' && c <= 'Z') {
        return 'z' - (tolower(c) - 'a');
    }
    return c;  // For numbers and other characters
}

char *atbash_encode(const char *input) {
    if (!input) return NULL;
    
    size_t len = strlen(input);
    // Worst case: every character becomes a letter + spaces for grouping
    char *result = malloc(len * 2 + 1);
    if (!result) return NULL;
    
    size_t result_pos = 0;
    size_t group_count = 0;
    
    for (size_t i = 0; i < len; i++) {
        char c = input[i];
        
        if (isalnum(c)) {
            // Add space every 5 characters (except at the beginning)
            if (group_count > 0 && group_count % 5 == 0) {
                result[result_pos++] = ' ';
            }
            
            if (isalpha(c)) {
                result[result_pos++] = atbash_transform(c);
            } else {
                result[result_pos++] = c;  // Keep numbers as is
            }
            group_count++;
        }
        // Skip punctuation and spaces
    }
    
    result[result_pos] = '\0';
    
    // Resize to actual needed size
    char *final_result = realloc(result, result_pos + 1);
    return final_result ? final_result : result;
}

char *atbash_decode(const char *input) {
    if (!input) return NULL;
    
    size_t len = strlen(input);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    
    size_t result_pos = 0;
    
    for (size_t i = 0; i < len; i++) {
        char c = input[i];
        
        if (c != ' ') {  // Skip spaces
            if (isalpha(c)) {
                result[result_pos++] = atbash_transform(c);
            } else {
                result[result_pos++] = c;  // Keep numbers as is
            }
        }
    }
    
    result[result_pos] = '\0';
    
    // Resize to actual needed size
    char *final_result = realloc(result, result_pos + 1);
    return final_result ? final_result : result;
}
