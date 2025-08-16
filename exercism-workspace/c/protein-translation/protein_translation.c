#include "protein_translation.h"
#include <string.h>

static bool is_stop_codon(const char *codon) {
    return strcmp(codon, "UAA") == 0 || 
           strcmp(codon, "UAG") == 0 || 
           strcmp(codon, "UGA") == 0;
}

static bool translate_codon(const char *codon, amino_acid_t *amino_acid) {
    if (strcmp(codon, "AUG") == 0) {
        *amino_acid = Methionine;
    } else if (strcmp(codon, "UUU") == 0 || strcmp(codon, "UUC") == 0) {
        *amino_acid = Phenylalanine;
    } else if (strcmp(codon, "UUA") == 0 || strcmp(codon, "UUG") == 0) {
        *amino_acid = Leucine;
    } else if (strcmp(codon, "UCU") == 0 || strcmp(codon, "UCC") == 0 || 
               strcmp(codon, "UCA") == 0 || strcmp(codon, "UCG") == 0) {
        *amino_acid = Serine;
    } else if (strcmp(codon, "UAU") == 0 || strcmp(codon, "UAC") == 0) {
        *amino_acid = Tyrosine;
    } else if (strcmp(codon, "UGU") == 0 || strcmp(codon, "UGC") == 0) {
        *amino_acid = Cysteine;
    } else if (strcmp(codon, "UGG") == 0) {
        *amino_acid = Tryptophan;
    } else {
        return false; // Invalid codon
    }
    return true;
}

protein_t protein(const char *const rna) {
    protein_t result = { .valid = true, .count = 0 };
    
    if (!rna) {
        result.valid = false;
        return result;
    }
    
    size_t rna_len = strlen(rna);
    bool found_stop = false;
    
    // Process each codon (3 nucleotides)
    for (size_t i = 0; i + 2 < rna_len; i += 3) {
        char codon[4];
        strncpy(codon, rna + i, 3);
        codon[3] = '\0';
        
        // Check for stop codon
        if (is_stop_codon(codon)) {
            found_stop = true;
            break; // Stop translation
        }
        
        // Translate codon to amino acid
        amino_acid_t amino_acid;
        if (!translate_codon(codon, &amino_acid)) {
            result.valid = false;
            return result;
        }
        
        // Check if we have room for more amino acids
        if (result.count >= MAX_AMINO_ACIDS) {
            result.valid = false;
            return result;
        }
        
        result.amino_acids[result.count] = amino_acid;
        result.count++;
    }
    
    // If we didn't find a stop codon and the sequence length is not a multiple of 3, it's invalid
    if (!found_stop && rna_len % 3 != 0) {
        result.valid = false;
        return result;
    }
    
    return result;
}
