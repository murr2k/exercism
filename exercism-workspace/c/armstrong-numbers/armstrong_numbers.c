#include "armstrong_numbers.h"
#include <math.h>
#include <stdbool.h>

bool is_armstrong_number(int num)
{
    if (num < 0) return false;
    
    int original = num;
    int digits = 0;
    int temp = num;
    
    // Count digits
    while (temp > 0) {
        digits++;
        temp /= 10;
    }
    
    // Calculate sum of powers
    temp = num;
    int sum = 0;
    while (temp > 0) {
        int digit = temp % 10;
        sum += pow(digit, digits);
        temp /= 10;
    }
    
    return sum == original;
}