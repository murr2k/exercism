#include "all_your_base.h"

size_t rebase(int8_t digits[], int16_t input_base, int16_t output_base, size_t input_length) {
    // Validate bases
    if (input_base < 2 || output_base < 2) {
        return 0;
    }
    
    // Handle empty input
    if (input_length == 0) {
        return 0;
    }
    
    // Validate digits and find the first non-zero digit
    size_t start = 0;
    for (size_t i = 0; i < input_length; i++) {
        if (digits[i] < 0 || digits[i] >= input_base) {
            return 0;  // Invalid digit
        }
        if (start == i && digits[i] == 0 && i < input_length - 1) {
            start++;  // Skip leading zeros, but keep at least one digit
        }
    }
    
    // Handle case where all digits are zero
    if (start == input_length || (start == input_length - 1 && digits[start] == 0)) {
        digits[0] = 0;
        return 1;
    }
    
    // Convert from input base to decimal
    uint64_t decimal_value = 0;
    uint64_t base_power = 1;
    
    // Process digits from right to left (least significant first)
    for (size_t i = input_length; i > start; i--) {
        decimal_value += digits[i - 1] * base_power;
        base_power *= input_base;
    }
    
    // Convert from decimal to output base
    if (decimal_value == 0) {
        digits[0] = 0;
        return 1;
    }
    
    size_t result_length = 0;
    int8_t temp_digits[DIGITS_ARRAY_SIZE];
    
    // Extract digits in reverse order (least significant first)
    while (decimal_value > 0 && result_length < DIGITS_ARRAY_SIZE) {
        temp_digits[result_length] = decimal_value % output_base;
        decimal_value /= output_base;
        result_length++;
    }
    
    // Reverse the digits to get most significant first
    for (size_t i = 0; i < result_length; i++) {
        digits[i] = temp_digits[result_length - 1 - i];
    }
    
    return result_length;
}
