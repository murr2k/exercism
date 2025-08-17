pub fn number(user_number: &str) -> Option<String> {
    // Extract only digits
    let digits: String = user_number.chars().filter(|c| c.is_ascii_digit()).collect();
    
    // Check for valid length
    match digits.len() {
        10 => {
            // 10 digits - must be valid US number
            validate_nanp_number(&digits)
        }
        11 => {
            // 11 digits - must start with 1 and the remaining 10 digits must be valid
            if digits.starts_with('1') {
                validate_nanp_number(&digits[1..])
            } else {
                None
            }
        }
        _ => None, // Invalid length
    }
}

fn validate_nanp_number(digits: &str) -> Option<String> {
    if digits.len() != 10 {
        return None;
    }
    
    let area_code = &digits[0..3];
    let exchange_code = &digits[3..6];
    
    // Area code cannot start with 0 or 1
    if area_code.starts_with('0') || area_code.starts_with('1') {
        return None;
    }
    
    // Exchange code cannot start with 0 or 1
    if exchange_code.starts_with('0') || exchange_code.starts_with('1') {
        return None;
    }
    
    Some(digits.to_string())
}

