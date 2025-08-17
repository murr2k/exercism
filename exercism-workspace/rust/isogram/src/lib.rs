pub fn check(candidate: &str) -> bool {
    use std::collections::HashSet;
    
    let mut seen = HashSet::new();
    
    for ch in candidate.chars() {
        // Only consider alphabetic characters, ignore case
        if ch.is_alphabetic() {
            let lowercase = ch.to_ascii_lowercase();
            if seen.contains(&lowercase) {
                return false;
            }
            seen.insert(lowercase);
        }
    }
    
    true
}
