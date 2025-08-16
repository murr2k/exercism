#include "roman_numerals.h"
#include <stdlib.h>
#include <string.h>

char *to_roman_numeral(unsigned int number)
{
    // Roman numeral values in descending order
    struct {
        unsigned int value;
        const char *numeral;
    } conversions[] = {
        {1000, "M"},
        {900, "CM"},
        {500, "D"},
        {400, "CD"},
        {100, "C"},
        {90, "XC"},
        {50, "L"},
        {40, "XL"},
        {10, "X"},
        {9, "IX"},
        {5, "V"},
        {4, "IV"},
        {1, "I"}
    };
    
    // Allocate enough space for the result
    // Max length would be for 3888 = MMMDCCCLXXXVIII (15 chars) + null terminator
    char *result = malloc(20);
    if (!result) {
        return NULL;
    }
    result[0] = '\0';
    
    // Convert number to Roman numerals
    for (int i = 0; i < 13 && number > 0; i++) {
        while (number >= conversions[i].value) {
            strcat(result, conversions[i].numeral);
            number -= conversions[i].value;
        }
    }
    
    return result;
}