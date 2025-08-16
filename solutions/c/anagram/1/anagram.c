#include "anagram.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Helper function to compare characters for qsort
static int char_compare(const void *a, const void *b) {
    return *(char*)a - *(char*)b;
}

// Helper function to convert string to lowercase and sort it
static void normalize_word(const char *word, char *normalized) {
    int len = strlen(word);
    
    // Convert to lowercase
    for (int i = 0; i < len; i++) {
        normalized[i] = tolower(word[i]);
    }
    normalized[len] = '\0';
    
    // Sort the characters
    qsort(normalized, len, sizeof(char), char_compare);
}

// Helper function to check if two words are the same (case-insensitive)
static int words_are_same(const char *word1, const char *word2) {
    if (strlen(word1) != strlen(word2)) {
        return 0;
    }
    
    for (size_t i = 0; i < strlen(word1); i++) {
        if (tolower(word1[i]) != tolower(word2[i])) {
            return 0;
        }
    }
    return 1;
}

void find_anagrams(const char *subject, struct candidates *candidates) {
    char subject_normalized[MAX_STR_LEN];
    normalize_word(subject, subject_normalized);
    
    for (size_t i = 0; i < candidates->count; i++) {
        // Check if the candidate is the same word as the subject (case-insensitive)
        if (words_are_same(subject, candidates->candidate[i].word)) {
            candidates->candidate[i].is_anagram = NOT_ANAGRAM;
            continue;
        }
        
        // Normalize the candidate word
        char candidate_normalized[MAX_STR_LEN];
        normalize_word(candidates->candidate[i].word, candidate_normalized);
        
        // Compare normalized strings
        if (strcmp(subject_normalized, candidate_normalized) == 0) {
            candidates->candidate[i].is_anagram = IS_ANAGRAM;
        } else {
            candidates->candidate[i].is_anagram = NOT_ANAGRAM;
        }
    }
}
