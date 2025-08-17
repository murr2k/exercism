/// Determine whether a sentence is a pangram.
pub fn is_pangram(sentence: &str) -> bool {
    let mut seen = [false; 26];
    
    for ch in sentence.chars() {
        if ch.is_ascii_alphabetic() {
            let letter = ch.to_ascii_lowercase() as u8 - b'a';
            seen[letter as usize] = true;
        }
    }
    
    seen.iter().all(|&x| x)
}