#include "diamond.h"
#include <stdlib.h>
#include <string.h>

char **make_diamond(const char letter) {
    int n = letter - 'A';  // 0 for A, 1 for B, etc.
    int size = 2 * n + 1;  // total number of rows
    int width = 2 * n + 1; // width of each row
    
    // Allocate array of string pointers (add 1 for NULL terminator)
    char **diamond = malloc((size + 1) * sizeof(char*));
    if (!diamond) return NULL;
    
    // Create each row
    for (int i = 0; i < size; i++) {
        diamond[i] = malloc((width + 1) * sizeof(char));
        if (!diamond[i]) {
            // Free previously allocated rows on failure
            for (int j = 0; j < i; j++) {
                free(diamond[j]);
            }
            free(diamond);
            return NULL;
        }
        
        // Fill row with spaces and null terminator
        memset(diamond[i], ' ', width);
        diamond[i][width] = '\0';
        
        // Determine which letter for this row
        int row_from_center = (i <= n) ? i : (size - 1 - i);
        char current_letter = 'A' + row_from_center;
        
        if (current_letter == 'A') {
            // Special case: A appears only in center
            diamond[i][n] = 'A';
        } else {
            // Two instances of the letter
            int outer_spaces = n - row_from_center;
            int inner_spaces = 2 * row_from_center - 1;
            
            diamond[i][outer_spaces] = current_letter;
            diamond[i][outer_spaces + 1 + inner_spaces] = current_letter;
        }
    }
    
    // NULL terminator for the array
    diamond[size] = NULL;
    
    return diamond;
}

void free_diamond(char **diamond) {
    if (!diamond) return;
    
    for (int i = 0; diamond[i] != NULL; i++) {
        free(diamond[i]);
    }
    free(diamond);
}
