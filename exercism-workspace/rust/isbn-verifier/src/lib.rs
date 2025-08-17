/// Determines whether the supplied string is a valid ISBN number
pub fn is_valid_isbn(isbn: &str) -> bool {
    // Remove dashes and get the clean ISBN
    let clean_isbn: String = isbn.chars().filter(|&c| c != '-').collect();
    
    // Must be exactly 10 characters
    if clean_isbn.len() != 10 {
        return false;
    }
    
    // Check characters and calculate checksum
    let mut sum = 0;
    let chars: Vec<char> = clean_isbn.chars().collect();
    
    // First 9 characters must be digits
    for i in 0..9 {
        if let Some(digit) = chars[i].to_digit(10) {
            sum += (digit as i32) * (10 - i as i32);
        } else {
            return false;
        }
    }
    
    // Last character can be digit or 'X'
    match chars[9] {
        'X' => sum += 10,
        c if c.is_ascii_digit() => {
            if let Some(digit) = c.to_digit(10) {
                sum += digit as i32;
            } else {
                return false;
            }
        }
        _ => return false,
    }
    
    // Valid if sum is divisible by 11
    sum % 11 == 0
}
