#include "wordy.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

bool answer(const char *question, int *result) {
    if (!question || !result) return false;
    
    // Check if it starts with "What is " and ends with "?"
    if (strncmp(question, "What is ", 8) != 0) return false;
    
    const char *p = question + 8; // Skip "What is "
    
    // Find the question mark
    const char *end = strrchr(question, '?');
    if (!end) return false;
    
    // Copy the expression part (without "What is " and "?")
    size_t expr_len = end - p;
    char *expr = malloc(expr_len + 1);
    if (!expr) return false;
    strncpy(expr, p, expr_len);
    expr[expr_len] = '\0';
    
    // Remove leading/trailing whitespace
    while (*p == ' ') p++;
    char *expr_end = expr + expr_len - 1;
    while (expr_end > expr && *expr_end == ' ') {
        *expr_end = '\0';
        expr_end--;
    }
    
    // If empty expression, invalid
    if (strlen(expr) == 0) {
        free(expr);
        return false;
    }
    
    // Parse the expression
    p = expr;
    
    // Parse first number
    char *endptr;
    long num = strtol(p, &endptr, 10);
    if (endptr == p) {
        free(expr);
        return false; // No number found
    }
    
    *result = (int)num;
    p = endptr;
    
    // Process operations
    while (*p) {
        // Skip whitespace
        while (*p == ' ') p++;
        if (*p == '\0') break;
        
        // Parse operation
        if (strncmp(p, "plus", 4) == 0) {
            p += 4;
            // Skip whitespace and parse next number
            while (*p == ' ') p++;
            num = strtol(p, &endptr, 10);
            if (endptr == p) {
                free(expr);
                return false;
            }
            *result += (int)num;
            p = endptr;
        } else if (strncmp(p, "minus", 5) == 0) {
            p += 5;
            while (*p == ' ') p++;
            num = strtol(p, &endptr, 10);
            if (endptr == p) {
                free(expr);
                return false;
            }
            *result -= (int)num;
            p = endptr;
        } else if (strncmp(p, "multiplied by", 13) == 0) {
            p += 13;
            while (*p == ' ') p++;
            num = strtol(p, &endptr, 10);
            if (endptr == p) {
                free(expr);
                return false;
            }
            *result *= (int)num;
            p = endptr;
        } else if (strncmp(p, "divided by", 10) == 0) {
            p += 10;
            while (*p == ' ') p++;
            num = strtol(p, &endptr, 10);
            if (endptr == p || num == 0) {
                free(expr);
                return false;
            }
            *result /= (int)num;
            p = endptr;
        } else {
            // Unknown operation
            free(expr);
            return false;
        }
    }
    
    free(expr);
    return true;
}
