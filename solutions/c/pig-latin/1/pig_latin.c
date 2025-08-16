#define _GNU_SOURCE
#include "pig_latin.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

static bool is_vowel(char c) {
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

static char *translate_word(const char *word) {
    int len = strlen(word);
    if (len == 0) return strdup("");
    
    char *result = malloc(len + 3); // +3 for "ay" + null terminator
    if (!result) return NULL;
    
    // Rule 1: word begins with vowel, "xr", or "yt"
    if (is_vowel(word[0]) || 
        (len >= 2 && word[0] == 'x' && word[1] == 'r') ||
        (len >= 2 && word[0] == 'y' && word[1] == 't')) {
        strcpy(result, word);
        strcat(result, "ay");
        return result;
    }
    
    // Find consonant cluster and check for special cases
    int consonant_end = 0;
    
    // Look for consonant cluster, handling special cases
    while (consonant_end < len) {
        char c = word[consonant_end];
        
        // Rule 3: Handle "qu" combinations
        if (c == 'q' && consonant_end + 1 < len && word[consonant_end + 1] == 'u') {
            consonant_end += 2; // Include both 'q' and 'u'
            break;
        }
        
        // Rule 4: Handle 'y' as vowel when it comes after consonants
        if (c == 'y' && consonant_end > 0) {
            // 'y' acts as a vowel here, so stop before it
            break;
        }
        
        // If we hit a vowel, stop
        if (is_vowel(c)) {
            break;
        }
        
        // If we hit 'y' as the first character, treat it as consonant
        if (c == 'y' && consonant_end == 0) {
            consonant_end++;
            continue;
        }
        
        consonant_end++;
    }
    
    // Rule 2 and others: Move consonant cluster to end
    if (consonant_end == 0) {
        // No consonants found, treat as vowel
        strcpy(result, word);
        strcat(result, "ay");
    } else {
        // Move consonants to end
        strcpy(result, word + consonant_end);
        strncat(result, word, consonant_end);
        strcat(result, "ay");
    }
    
    return result;
}

char *translate(const char *phrase) {
    if (!phrase) return NULL;
    
    int phrase_len = strlen(phrase);
    if (phrase_len == 0) return strdup("");
    
    // Estimate result size (generous allocation)
    char *result = malloc(phrase_len * 2 + 10);
    if (!result) return NULL;
    
    result[0] = '\0';
    
    // Parse phrase word by word
    char *phrase_copy = strdup(phrase);
    if (!phrase_copy) {
        free(result);
        return NULL;
    }
    
    char *word = strtok(phrase_copy, " ");
    bool first_word = true;
    
    while (word != NULL) {
        char *translated_word = translate_word(word);
        if (!translated_word) {
            free(result);
            free(phrase_copy);
            return NULL;
        }
        
        if (!first_word) {
            strcat(result, " ");
        }
        strcat(result, translated_word);
        
        free(translated_word);
        first_word = false;
        word = strtok(NULL, " ");
    }
    
    free(phrase_copy);
    return result;
}
