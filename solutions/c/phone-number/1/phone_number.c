#include "phone_number.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *phone_number_clean(const char *input)
{
    if (!input) {
        char *result = malloc(11);
        if (result) {
            strcpy(result, "0000000000");
        }
        return result;
    }
    
    // Extract only digits from input
    char digits[20] = {0}; // Temporary buffer for digits
    int digit_count = 0;
    
    for (int i = 0; input[i] && digit_count < 19; i++) {
        if (isdigit(input[i])) {
            digits[digit_count++] = input[i];
        } else if (!isspace(input[i]) && input[i] != '(' && input[i] != ')' && 
                   input[i] != '-' && input[i] != '.' && input[i] != '+') {
            // Invalid character (not digit, space, or valid punctuation)
            char *result = malloc(11);
            if (result) {
                strcpy(result, "0000000000");
            }
            return result;
        }
    }
    
    // Allocate result
    char *result = malloc(11);
    if (!result) return NULL;
    
    // Check digit count and validate
    if (digit_count == 10) {
        // Check area code (first 3 digits)
        if (digits[0] == '0' || digits[0] == '1') {
            strcpy(result, "0000000000");
            return result;
        }
        // Check exchange code (next 3 digits)
        if (digits[3] == '0' || digits[3] == '1') {
            strcpy(result, "0000000000");
            return result;
        }
        // Valid 10-digit number
        strncpy(result, digits, 10);
        result[10] = '\0';
        return result;
    } else if (digit_count == 11) {
        // Must start with 1
        if (digits[0] != '1') {
            strcpy(result, "0000000000");
            return result;
        }
        // Check area code (digits 1-3)
        if (digits[1] == '0' || digits[1] == '1') {
            strcpy(result, "0000000000");
            return result;
        }
        // Check exchange code (digits 4-6)
        if (digits[4] == '0' || digits[4] == '1') {
            strcpy(result, "0000000000");
            return result;
        }
        // Valid 11-digit number starting with 1, return last 10 digits
        strncpy(result, digits + 1, 10);
        result[10] = '\0';
        return result;
    } else {
        // Invalid length
        strcpy(result, "0000000000");
        return result;
    }
}
