#include "intergalactic_transmission.h"
#include <stddef.h>

// Helper function to count number of 1 bits in a byte
static int count_ones(uint8_t byte) {
    int count = 0;
    while (byte) {
        count += byte & 1;
        byte >>= 1;
    }
    return count;
}

// Helper function to calculate parity bit (0 for even count of 1s, 1 for odd)
static uint8_t calculate_parity(uint8_t data) {
    return count_ones(data) & 1;
}

int transmit_sequence(uint8_t *buffer, const uint8_t *message, int message_length) {
    if (message_length == 0 || message == NULL) {
        return 0;
    }
    
    int output_length = 0;
    int bit_accumulator = 0;
    int bits_accumulated = 0;
    
    // Process each byte of the message
    for (int i = 0; i < message_length; i++) {
        uint8_t byte = message[i];
        
        // Process each bit of the current byte
        for (int bit_pos = 7; bit_pos >= 0; bit_pos--) {
            // Add this bit to accumulator
            bit_accumulator = (bit_accumulator << 1) | ((byte >> bit_pos) & 1);
            bits_accumulated++;
            
            // When we have 7 data bits, add parity and output
            if (bits_accumulated == 7) {
                uint8_t parity = calculate_parity(bit_accumulator);
                uint8_t transmission = (bit_accumulator << 1) | parity;
                buffer[output_length++] = transmission;
                
                bit_accumulator = 0;
                bits_accumulated = 0;
            }
        }
    }
    
    // Handle remaining bits if any
    if (bits_accumulated > 0) {
        // Pad with zeros to make 7 bits
        bit_accumulator <<= (7 - bits_accumulated);
        uint8_t parity = calculate_parity(bit_accumulator);
        uint8_t transmission = (bit_accumulator << 1) | parity;
        buffer[output_length++] = transmission;
    }
    
    return output_length;
}

int decode_message(uint8_t *buffer, const uint8_t *message, int message_length) {
    if (message_length == 0 || message == NULL) {
        return 0;
    }
    
    int output_length = 0;
    int bit_accumulator = 0;
    int bits_accumulated = 0;
    
    // Process each transmitted byte
    for (int i = 0; i < message_length; i++) {
        uint8_t transmission = message[i];
        
        // Extract data bits (all bits except parity bit)
        uint8_t data_bits = transmission >> 1;
        uint8_t received_parity = transmission & 1;
        
        // Check parity
        uint8_t expected_parity = calculate_parity(data_bits);
        if (received_parity != expected_parity) {
            return WRONG_PARITY;
        }
        
        // Add the 7 data bits to our accumulator
        for (int bit_pos = 6; bit_pos >= 0; bit_pos--) {
            bit_accumulator = (bit_accumulator << 1) | ((data_bits >> bit_pos) & 1);
            bits_accumulated++;
            
            // When we have 8 bits, output a byte
            if (bits_accumulated == 8) {
                buffer[output_length++] = bit_accumulator;
                bit_accumulator = 0;
                bits_accumulated = 0;
            }
        }
    }
    
    return output_length;
}
