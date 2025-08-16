#include "word_count.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Helper function to check if a character is a word separator
static bool is_word_separator(char c) {
    return !isalnum(c) && c != '\'';
}

// Helper function to convert string to lowercase
static void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Helper function to trim quotes from word boundaries
static void trim_quotes(char *word, int *len) {
    // Remove leading quotes
    while (*len > 0 && word[0] == '\'') {
        memmove(word, word + 1, *len);
        (*len)--;
    }
    
    // Remove trailing quotes
    while (*len > 0 && word[*len - 1] == '\'') {
        word[*len - 1] = '\0';
        (*len)--;
    }
}

int count_words(const char *sentence, word_count_word_t *words) {
    if (!sentence || !words) {
        return 0;
    }
    
    int word_count = 0;
    int sentence_len = strlen(sentence);
    
    for (int i = 0; i < sentence_len; i++) {
        // Skip separators
        if (is_word_separator(sentence[i]) && sentence[i] != '\'') {
            continue;
        }
        
        // Found start of a word
        char current_word[MAX_WORD_LENGTH + 1];
        int word_len = 0;
        
        // Extract the word
        while (i < sentence_len && !is_word_separator(sentence[i])) {
            if (word_len >= MAX_WORD_LENGTH) {
                return EXCESSIVE_LENGTH_WORD;
            }
            current_word[word_len++] = sentence[i++];
        }
        
        // Move back one if we went past the word
        if (i < sentence_len && is_word_separator(sentence[i])) {
            i--;
        }
        
        if (word_len == 0) continue;
        
        current_word[word_len] = '\0';
        
        // Trim surrounding quotes
        trim_quotes(current_word, &word_len);
        
        if (word_len == 0) continue;
        
        // Convert to lowercase
        to_lowercase(current_word);
        
        // Check if word already exists
        bool found = false;
        for (int j = 0; j < word_count; j++) {
            if (strcmp(words[j].text, current_word) == 0) {
                words[j].count++;
                found = true;
                break;
            }
        }
        
        // If not found, add new word
        if (!found) {
            if (word_count >= MAX_WORDS) {
                return EXCESSIVE_NUMBER_OF_WORDS;
            }
            
            strncpy(words[word_count].text, current_word, MAX_WORD_LENGTH);
            words[word_count].text[MAX_WORD_LENGTH] = '\0';
            words[word_count].count = 1;
            word_count++;
        }
    }
    
    return word_count;
}
