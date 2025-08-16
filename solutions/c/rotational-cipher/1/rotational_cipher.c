#include "rotational_cipher.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *rotate(const char *text, int shift_key)
{
    if (text == NULL) {
        return NULL;
    }
    
    // Normalize shift_key to 0-25 range
    shift_key = shift_key % 26;
    if (shift_key < 0) {
        shift_key += 26;
    }
    
    size_t len = strlen(text);
    char *result = malloc(len + 1);
    if (result == NULL) {
        return NULL;
    }
    
    for (size_t i = 0; i < len; i++) {
        char c = text[i];
        
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            result[i] = (c - base + shift_key) % 26 + base;
        } else {
            // Non-alphabetic characters remain unchanged
            result[i] = c;
        }
    }
    
    result[len] = '\0';
    return result;
}
