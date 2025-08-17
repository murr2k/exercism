pub fn abbreviate(phrase: &str) -> String {
    phrase
        .split(|c: char| c.is_whitespace() || c == '-' || c == '_')
        .filter(|word| !word.is_empty())
        .map(|word| {
            // Check if word is all uppercase
            if word.chars().all(|c| c.is_uppercase()) {
                // For all-caps words, just take the first character
                word.chars().next().unwrap().to_string()
            } else {
                // For mixed case, take first char and internal uppercase chars
                let first_char = word.chars().next().unwrap().to_uppercase().collect::<String>();
                let rest: String = word.chars()
                    .skip(1)
                    .filter(|c| c.is_uppercase())
                    .map(|c| c.to_string())
                    .collect();
                first_char + &rest
            }
        })
        .collect()
}