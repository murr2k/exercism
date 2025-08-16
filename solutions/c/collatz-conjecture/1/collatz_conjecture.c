#include "collatz_conjecture.h"

int steps(int start)
{
    if (start <= 0) {
        return ERROR_VALUE;
    }
    
    int count = 0;
    int n = start;
    
    while (n != 1) {
        if (n % 2 == 0) {
            n = n / 2;
        } else {
            n = 3 * n + 1;
        }
        count++;
    }
    
    return count;
}