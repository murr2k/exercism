use std::fmt::Display;

pub trait Luhn {
    fn valid_luhn(&self) -> bool;
}

fn validate_luhn_string(input: &str) -> bool {
    let digits: Vec<char> = input.chars().filter(|c| !c.is_whitespace()).collect();
    
    // Must have at least 2 digits
    if digits.len() < 2 {
        return false;
    }
    
    // All characters must be digits
    if !digits.iter().all(|c| c.is_ascii_digit()) {
        return false;
    }
    
    // Apply Luhn algorithm
    let sum: u32 = digits
        .iter()
        .rev()
        .enumerate()
        .map(|(i, &c)| {
            let mut digit = c.to_digit(10).unwrap();
            if i % 2 == 1 {
                digit *= 2;
                if digit > 9 {
                    digit -= 9;
                }
            }
            digit
        })
        .sum();
    
    sum % 10 == 0
}

impl<T> Luhn for T
where
    T: Display,
{
    fn valid_luhn(&self) -> bool {
        validate_luhn_string(&self.to_string())
    }
}