#include "bob.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

static bool is_shouting(const char *greeting) {
    bool has_letter = false;
    
    for (int i = 0; greeting[i] != '\0'; i++) {
        if (isalpha(greeting[i])) {
            has_letter = true;
            if (islower(greeting[i])) {
                return false;
            }
        }
    }
    
    return has_letter;
}

static bool is_question(const char *greeting) {
    int len = strlen(greeting);
    
    // Trim trailing whitespace to find last non-space character
    while (len > 0 && isspace(greeting[len - 1])) {
        len--;
    }
    
    return len > 0 && greeting[len - 1] == '?';
}

static bool is_silence(const char *greeting) {
    for (int i = 0; greeting[i] != '\0'; i++) {
        if (!isspace(greeting[i])) {
            return false;
        }
    }
    return true;
}

char *hey_bob(char *greeting) {
    if (is_silence(greeting)) {
        return "Fine. Be that way!";
    }
    
    bool shouting = is_shouting(greeting);
    bool question = is_question(greeting);
    
    if (shouting && question) {
        return "Calm down, I know what I'm doing!";
    } else if (shouting) {
        return "Whoa, chill out!";
    } else if (question) {
        return "Sure.";
    } else {
        return "Whatever.";
    }
}