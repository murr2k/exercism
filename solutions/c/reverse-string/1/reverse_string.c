#include "reverse_string.h"
#include <string.h>
#include <stdlib.h>

char *reverse(const char *value)
{
    if (!value) return NULL;
    
    int len = strlen(value);
    char *result = malloc(len + 1);
    
    if (!result) return NULL;
    
    for (int i = 0; i < len; i++) {
        result[i] = value[len - 1 - i];
    }
    result[len] = '\0';
    
    return result;
}