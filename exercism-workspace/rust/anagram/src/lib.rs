use std::collections::HashSet;

pub fn anagrams_for<'a>(word: &str, possible_anagrams: &'a [&str]) -> HashSet<&'a str> {
    let word_lower = word.to_lowercase();
    let mut word_chars: Vec<char> = word_lower.chars().collect();
    word_chars.sort();
    
    possible_anagrams
        .iter()
        .filter(|&candidate| {
            let candidate_lower = candidate.to_lowercase();
            
            // Skip if it's the same word (case insensitive)
            if word_lower == candidate_lower {
                return false;
            }
            
            // Check if they have the same length
            if word.len() != candidate.len() {
                return false;
            }
            
            // Sort characters and compare
            let mut candidate_chars: Vec<char> = candidate_lower.chars().collect();
            candidate_chars.sort();
            
            word_chars == candidate_chars
        })
        .cloned()
        .collect()
}
