use std::collections::HashMap;

/// Count occurrences of words.
pub fn word_count(words: &str) -> HashMap<String, u32> {
    let mut counts = HashMap::new();
    
    // Use regex-like behavior to split on non-alphanumeric characters except apostrophes
    let mut current_word = String::new();
    
    for ch in words.chars() {
        if ch.is_alphanumeric() || ch == '\'' {
            current_word.push(ch.to_ascii_lowercase());
        } else {
            if !current_word.is_empty() {
                let word = normalize_word(&current_word);
                if !word.is_empty() {
                    *counts.entry(word).or_insert(0) += 1;
                }
                current_word.clear();
            }
        }
    }
    
    // Handle the last word
    if !current_word.is_empty() {
        let word = normalize_word(&current_word);
        if !word.is_empty() {
            *counts.entry(word).or_insert(0) += 1;
        }
    }
    
    counts
}

fn normalize_word(word: &str) -> String {
    // Remove leading and trailing quotes/apostrophes
    let result = word.trim_matches('\'');
    
    // If the word is only apostrophes, return empty
    if result.is_empty() {
        return String::new();
    }
    
    result.to_string()
}