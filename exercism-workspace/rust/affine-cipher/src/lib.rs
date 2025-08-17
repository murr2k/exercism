/// While the problem description indicates a return status of 1 should be returned on errors,
/// it is much more common to return a `Result`, so we provide an error type for the result here.
#[derive(Debug, Eq, PartialEq)]
pub enum AffineCipherError {
    NotCoprime(i32),
}

/// Encodes the plaintext using the affine cipher with key (`a`, `b`). Note that, rather than
/// returning a return code, the more common convention in Rust is to return a `Result`.
pub fn encode(plaintext: &str, a: i32, b: i32) -> Result<String, AffineCipherError> {
    // Check if a and 26 are coprime
    if gcd(a, 26) != 1 {
        return Err(AffineCipherError::NotCoprime(a));
    }
    
    let mut result = String::new();
    let mut char_count = 0;
    
    for ch in plaintext.chars() {
        if ch.is_ascii_alphabetic() {
            let ch_lower = ch.to_ascii_lowercase();
            let x = (ch_lower as u8 - b'a') as i32;
            let encoded = ((a * x + b) % 26) as u8 + b'a';
            result.push(encoded as char);
            char_count += 1;
            
            if char_count % 5 == 0 {
                result.push(' ');
            }
        } else if ch.is_ascii_digit() {
            result.push(ch);
            char_count += 1;
            
            if char_count % 5 == 0 {
                result.push(' ');
            }
        }
    }
    
    // Remove trailing space if present
    if result.ends_with(' ') {
        result.pop();
    }
    
    Ok(result)
}

/// Decodes the ciphertext using the affine cipher with key (`a`, `b`). Note that, rather than
/// returning a return code, the more common convention in Rust is to return a `Result`.
pub fn decode(ciphertext: &str, a: i32, b: i32) -> Result<String, AffineCipherError> {
    // Check if a and 26 are coprime
    if gcd(a, 26) != 1 {
        return Err(AffineCipherError::NotCoprime(a));
    }
    
    let a_inv = mod_inverse(a, 26).unwrap();
    let mut result = String::new();
    
    for ch in ciphertext.chars() {
        if ch.is_ascii_alphabetic() {
            let ch_lower = ch.to_ascii_lowercase();
            let y = (ch_lower as u8 - b'a') as i32;
            let decoded = (a_inv * (y - b)).rem_euclid(26) as u8 + b'a';
            result.push(decoded as char);
        } else if ch.is_ascii_digit() {
            result.push(ch);
        }
        // Skip spaces and other characters
    }
    
    Ok(result)
}

fn gcd(mut a: i32, mut b: i32) -> i32 {
    while b != 0 {
        let temp = b;
        b = a % b;
        a = temp;
    }
    a.abs()
}

fn mod_inverse(a: i32, m: i32) -> Option<i32> {
    let (gcd, x, _) = extended_gcd(a, m);
    if gcd == 1 {
        Some(x.rem_euclid(m))
    } else {
        None
    }
}

fn extended_gcd(a: i32, b: i32) -> (i32, i32, i32) {
    if a == 0 {
        (b, 0, 1)
    } else {
        let (gcd, x1, y1) = extended_gcd(b % a, a);
        let x = y1 - (b / a) * x1;
        let y = x1;
        (gcd, x, y)
    }
}
