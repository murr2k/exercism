#include "isogram.h"
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

bool is_isogram(const char phrase[])
{
    if (phrase == NULL) return false;
    
    int seen[26] = {0};
    
    for (int i = 0; phrase[i] != '\0'; i++) {
        char c = tolower(phrase[i]);
        if (c >= 'a' && c <= 'z') {
            if (seen[c - 'a']) {
                return false;
            }
            seen[c - 'a'] = 1;
        }
    }
    
    return true;
}