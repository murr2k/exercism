use rand::Rng;

pub fn encode(key: &str, s: &str) -> Option<String> {
    if key.is_empty() || !key.chars().all(|c| c.is_ascii_lowercase()) {
        return None;
    }
    
    let s_filtered: String = s.to_lowercase()
        .chars()
        .filter(|c| c.is_ascii_lowercase())
        .collect();
    
    let encoded: String = s_filtered
        .chars()
        .zip(key.chars().cycle())
        .map(|(plain, key_char)| {
            let plain_val = (plain as u8) - b'a';
            let key_val = (key_char as u8) - b'a';
            let encoded_val = (plain_val + key_val) % 26;
            (encoded_val + b'a') as char
        })
        .collect();
    
    Some(encoded)
}

pub fn decode(key: &str, s: &str) -> Option<String> {
    if key.is_empty() || !key.chars().all(|c| c.is_ascii_lowercase()) {
        return None;
    }
    
    let decoded: String = s
        .chars()
        .zip(key.chars().cycle())
        .map(|(cipher, key_char)| {
            let cipher_val = (cipher as u8) - b'a';
            let key_val = (key_char as u8) - b'a';
            let decoded_val = (cipher_val + 26 - key_val) % 26;
            (decoded_val + b'a') as char
        })
        .collect();
    
    Some(decoded)
}

pub fn encode_random(s: &str) -> (String, String) {
    let mut rng = rand::thread_rng();
    let key_length = s.chars().filter(|c| c.is_ascii_alphabetic()).count().max(100);
    
    let key: String = (0..key_length)
        .map(|_| {
            let n = rng.gen_range(0..26);
            (b'a' + n) as char
        })
        .collect();
    
    let encoded = encode(&key, s).unwrap();
    (key, encoded)
}

#[cfg(test)]
mod tests {
    use super::*;

    const PLAIN_TEXT: &str = "thisismysecret";
    const KEY: &str = "abcdefghij";

    #[test]
    fn test_encode_with_key() {
        let key = "abcdefghij";
        let plaintext = "thisismysecret";
        let ciphertext = encode(key, plaintext);
        // Expected: t+a=t, h+b=i, i+c=k, s+d=v...
        assert_eq!(ciphertext, Some("tikvmxsfancsgw".to_string()));
    }

    #[test]
    fn test_decode_with_key() {
        let key = "abcdefghij";
        let ciphertext = "tikvmxsfancsgw";
        let plaintext = decode(key, ciphertext);
        assert_eq!(plaintext, Some("thisismysecret".to_string()));
    }

    #[test]
    fn test_encode_decode_roundtrip() {
        let key = "abcdefghij";
        let plaintext = "thisismysecret";
        let ciphertext = encode(key, plaintext).unwrap();
        let decoded = decode(key, &ciphertext).unwrap();
        assert_eq!(decoded, plaintext);
    }

    #[test]
    fn test_key_is_as_long_as_plaintext() {
        let plaintext = "thisismysecret";
        let (key, _) = encode_random(plaintext);
        assert!(key.len() >= plaintext.len());
    }

    #[test]
    fn test_encode_random() {
        let plaintext = "thisismysecret";
        let (key, encoded) = encode_random(plaintext);
        assert!(key.chars().all(|c| c.is_ascii_lowercase()));
        assert_eq!(decode(&key, &encoded).unwrap(), plaintext);
    }

    #[test]
    fn test_empty_key_returns_none() {
        assert_eq!(encode("", "sometext"), None);
        assert_eq!(decode("", "sometext"), None);
    }

    #[test]
    fn test_invalid_key_returns_none() {
        assert_eq!(encode("UPPERCASE", "sometext"), None);
        assert_eq!(encode("with spaces", "sometext"), None);
        assert_eq!(encode("with123numbers", "sometext"), None);
    }

    #[test]
    fn test_encode_filters_non_letters() {
        let key = "abc";
        let plaintext = "This, is a test!";
        let ciphertext = encode(key, plaintext);
        // this is a test -> thisisatest, then encode with "abc" cycling
        assert_eq!(ciphertext, Some("tiksjuaugsu".to_string()));
    }

    #[test]
    fn test_key_cycles() {
        let key = "abc";
        let plaintext = "iamapandabear";
        let ciphertext = encode(key, plaintext);
        assert_eq!(ciphertext, Some("iboaqcnecbfcr".to_string()));
    }

    #[test]
    fn test_wraps_around_alphabet() {
        let key = "dddd";
        let plaintext = "zzzzz";
        let ciphertext = encode(key, plaintext);
        assert_eq!(ciphertext, Some("ccccc".to_string()));
    }
}