#include "rna_transcription.h"
#include <stdlib.h>
#include <string.h>

char *to_rna(const char *dna)
{
    if (dna == NULL) {
        return NULL;
    }
    
    size_t length = strlen(dna);
    char *rna = malloc(length + 1);
    
    if (rna == NULL) {
        return NULL;
    }
    
    for (size_t i = 0; i < length; i++) {
        switch (dna[i]) {
            case 'G':
                rna[i] = 'C';
                break;
            case 'C':
                rna[i] = 'G';
                break;
            case 'T':
                rna[i] = 'A';
                break;
            case 'A':
                rna[i] = 'U';
                break;
            default:
                // Invalid nucleotide - should not happen in well-formed input
                free(rna);
                return NULL;
        }
    }
    
    rna[length] = '\0';
    return rna;
}
