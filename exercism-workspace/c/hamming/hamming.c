#include "hamming.h"
#include <string.h>

int compute(const char *lhs, const char *rhs)
{
    if (!lhs || !rhs) return -1;
    
    int len1 = strlen(lhs);
    int len2 = strlen(rhs);
    
    if (len1 != len2) return -1;
    
    int distance = 0;
    for (int i = 0; i < len1; i++) {
        if (lhs[i] != rhs[i]) {
            distance++;
        }
    }
    
    return distance;
}