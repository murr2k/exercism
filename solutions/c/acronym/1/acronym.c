#include "acronym.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *abbreviate(const char *phrase)
{
    if (!phrase || strlen(phrase) == 0) {
        return NULL;
    }
    
    // Allocate enough space for worst case (every char could be an acronym letter)
    char *result = malloc(strlen(phrase) + 1);
    if (!result) return NULL;
    
    int result_index = 0;
    int prev_was_separator = 1; // Start as true to catch first character
    
    for (int i = 0; phrase[i] != '\0'; i++) {
        char c = phrase[i];
        
        if (isalpha(c)) {
            if (prev_was_separator) {
                result[result_index++] = toupper(c);
                prev_was_separator = 0;
            }
        } else if (c == ' ' || c == '-' || c == '_') {
            prev_was_separator = 1;
        }
    }
    
    result[result_index] = '\0';
    
    // Resize to actual size
    char *final = realloc(result, result_index + 1);
    return final ? final : result;
}