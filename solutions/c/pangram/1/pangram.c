#include "pangram.h"
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

bool is_pangram(const char *sentence)
{
    if (!sentence) return false;
    
    int seen[26] = {0};
    
    for (int i = 0; sentence[i] != '\0'; i++) {
        char c = tolower(sentence[i]);
        if (c >= 'a' && c <= 'z') {
            seen[c - 'a'] = 1;
        }
    }
    
    // Check if all letters are seen
    for (int i = 0; i < 26; i++) {
        if (!seen[i]) {
            return false;
        }
    }
    
    return true;
}