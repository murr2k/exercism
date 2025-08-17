pub fn translate(input: &str) -> String {
    input
        .split_whitespace()
        .map(translate_word)
        .collect::<Vec<String>>()
        .join(" ")
}

fn translate_word(word: &str) -> String {
    let vowels = ['a', 'e', 'i', 'o', 'u'];
    let chars: Vec<char> = word.chars().collect();
    
    if chars.is_empty() {
        return String::new();
    }
    
    // Rule 1: Words that start with vowels
    if vowels.contains(&chars[0]) {
        return format!("{}ay", word);
    }
    
    // Rule 2: Words that start with "xr" or "yt" are treated as if they start with vowels
    if word.starts_with("xr") || word.starts_with("yt") {
        return format!("{}ay", word);
    }
    
    // Rule 3: Words that start with consonants
    let mut consonant_cluster_end = 0;
    let mut i = 0;
    
    while i < chars.len() {
        let ch = chars[i];
        
        // Check for "qu" as a special unit
        if ch == 'q' && i + 1 < chars.len() && chars[i + 1] == 'u' {
            consonant_cluster_end = i + 2;
            i += 2;
            continue;
        }
        
        // 'y' is a vowel if it's not at the beginning of the word
        if ch == 'y' && i > 0 {
            break;
        }
        
        // Regular vowels stop the consonant cluster
        if vowels.contains(&ch) {
            break;
        }
        
        // It's a consonant
        consonant_cluster_end = i + 1;
        i += 1;
    }
    
    let consonants = &word[..consonant_cluster_end];
    let rest = &word[consonant_cluster_end..];
    
    format!("{}{}ay", rest, consonants)
}

