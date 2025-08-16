#include "variable_length_quantity.h"

int encode(const uint32_t *integers, size_t integers_len, uint8_t *output)
{
    int output_idx = 0;
    
    for (size_t i = 0; i < integers_len; i++) {
        uint32_t value = integers[i];
        
        // Handle zero as special case
        if (value == 0) {
            output[output_idx++] = 0;
            continue;
        }
        
        // Count how many 7-bit groups we need
        uint32_t temp = value;
        int num_bytes = 0;
        while (temp > 0) {
            temp >>= 7;
            num_bytes++;
        }
        
        // Encode from most significant to least significant byte
        for (int j = num_bytes - 1; j >= 0; j--) {
            uint8_t byte = (value >> (j * 7)) & 0x7F;
            
            // Set continuation bit for all bytes except the last one
            if (j > 0) {
                byte |= 0x80;
            }
            
            output[output_idx++] = byte;
        }
    }
    
    return output_idx;
}

int decode(const uint8_t *bytes, size_t buffer_len, uint32_t *output)
{
    int output_idx = 0;
    size_t byte_idx = 0;
    
    while (byte_idx < buffer_len) {
        uint32_t value = 0;
        
        // Read bytes until we find one without the continuation bit
        while (byte_idx < buffer_len) {
            uint8_t byte = bytes[byte_idx++];
            value = (value << 7) | (byte & 0x7F);
            
            // If continuation bit is not set, this is the last byte
            if ((byte & 0x80) == 0) {
                break;
            }
            
            // Check if we've reached the end without finding a terminating byte
            if (byte_idx >= buffer_len) {
                return -1; // Incomplete sequence
            }
        }
        
        output[output_idx++] = value;
    }
    
    return output_idx;
}
