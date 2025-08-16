#include "secret_handshake.h"
#include <stdlib.h>
#include <string.h>

const char **commands(size_t number) {
    const char **result = malloc(4 * sizeof(char *));
    if (!result) return NULL;
    
    // Initialize all elements to NULL
    for (int i = 0; i < 4; i++) {
        result[i] = NULL;
    }
    
    const char *actions[4] = {"wink", "double blink", "close your eyes", "jump"};
    int action_count = 0;
    
    // Check each bit (0-3) and add corresponding actions
    for (int i = 0; i < 4; i++) {
        if (number & (1 << i)) {
            result[action_count++] = actions[i];
        }
    }
    
    // Check if bit 4 is set (reverse order)
    if (number & 16) {
        // Reverse the array in place
        for (int i = 0; i < action_count / 2; i++) {
            const char *temp = result[i];
            result[i] = result[action_count - 1 - i];
            result[action_count - 1 - i] = temp;
        }
    }
    
    return result;
}
