/// "Encipher" with the Atbash cipher.
pub fn encode(plain: &str) -> String {
    let mut encoded_chars = Vec::new();
    
    for ch in plain.chars() {
        if ch.is_ascii_alphabetic() {
            let lower_ch = ch.to_ascii_lowercase();
            // Map a->z, b->y, etc. using ASCII values
            let encoded = (b'z' - (lower_ch as u8 - b'a')) as char;
            encoded_chars.push(encoded);
        } else if ch.is_ascii_digit() {
            encoded_chars.push(ch);
        }
        // Skip spaces and punctuation
    }
    
    // Group into chunks of 5
    let mut result = String::new();
    for (i, ch) in encoded_chars.iter().enumerate() {
        if i > 0 && i % 5 == 0 {
            result.push(' ');
        }
        result.push(*ch);
    }
    
    result
}

/// "Decipher" with the Atbash cipher.
pub fn decode(cipher: &str) -> String {
    let mut result = String::new();
    
    for ch in cipher.chars() {
        if ch.is_ascii_alphabetic() {
            let lower_ch = ch.to_ascii_lowercase();
            // Atbash is its own inverse, so we use the same mapping
            let decoded = (b'z' - (lower_ch as u8 - b'a')) as char;
            result.push(decoded);
        } else if ch.is_ascii_digit() {
            result.push(ch);
        }
        // Skip spaces
    }
    
    result
}
