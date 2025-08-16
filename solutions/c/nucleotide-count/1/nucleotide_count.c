#include "nucleotide_count.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *count(const char *dna_strand)
{
    int a_count = 0, c_count = 0, g_count = 0, t_count = 0;
    
    if (dna_strand == NULL) {
        char *result = malloc(50);
        sprintf(result, "A:0 C:0 G:0 T:0");
        return result;
    }
    
    // Count each nucleotide
    for (int i = 0; dna_strand[i] != '\0'; i++) {
        switch (dna_strand[i]) {
            case 'A':
                a_count++;
                break;
            case 'C':
                c_count++;
                break;
            case 'G':
                g_count++;
                break;
            case 'T':
                t_count++;
                break;
            default: {
                // Invalid nucleotide
                char *error = malloc(50);
                strcpy(error, "");
                return error;
            }
        }
    }
    
    // Format the result
    char *result = malloc(50);
    sprintf(result, "A:%d C:%d G:%d T:%d", a_count, c_count, g_count, t_count);
    
    return result;
}