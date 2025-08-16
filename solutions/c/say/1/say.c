#include "say.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Word arrays for number conversion
static const char *ones[] = {
    "", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
    "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", 
    "seventeen", "eighteen", "nineteen"
};

static const char *tens[] = {
    "", "", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"
};

static const char *scales[] = {
    "", "thousand", "million", "billion"
};

static int say_three_digits(int num, char *buffer) {
    int pos = 0;
    
    if (num >= 100) {
        int hundreds = num / 100;
        pos += sprintf(buffer + pos, "%s hundred", ones[hundreds]);
        num %= 100;
        if (num > 0) {
            buffer[pos++] = ' ';
        }
    }
    
    if (num >= 20) {
        int ten_digit = num / 10;
        pos += sprintf(buffer + pos, "%s", tens[ten_digit]);
        num %= 10;
        if (num > 0) {
            buffer[pos++] = '-';
            pos += sprintf(buffer + pos, "%s", ones[num]);
        }
    } else if (num > 0) {
        pos += sprintf(buffer + pos, "%s", ones[num]);
    }
    
    return pos;
}

int say(int64_t input, char **ans) {
    if (input < 0 || input > 999999999999LL) {
        return -1;
    }
    
    if (input == 0) {
        *ans = malloc(5);
        if (!*ans) return -1;
        strcpy(*ans, "zero");
        return 0;
    }
    
    // Buffer large enough for longest possible result
    char buffer[1000] = {0};
    int pos = 0;
    
    // Process number in groups of three digits
    int64_t remaining = input;
    char groups[4][100] = {0}; // billions, millions, thousands, ones
    
    // Break number into groups of three digits
    for (int i = 0; i < 4 && remaining > 0; i++) {
        int group = remaining % 1000;
        if (group > 0) {
            say_three_digits(group, groups[i]);
        }
        remaining /= 1000;
    }
    
    // Build the final string from groups
    bool first = true;
    for (int i = 3; i >= 0; i--) {
        if (strlen(groups[i]) > 0) {
            if (!first) {
                buffer[pos++] = ' ';
            }
            strcpy(buffer + pos, groups[i]);
            pos += strlen(groups[i]);
            
            if (i > 0) {
                buffer[pos++] = ' ';
                strcpy(buffer + pos, scales[i]);
                pos += strlen(scales[i]);
            }
            first = false;
        }
    }
    
    *ans = malloc(pos + 1);
    if (!*ans) return -1;
    strcpy(*ans, buffer);
    
    return 0;
}
