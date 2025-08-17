pub struct Luhn {
    number: String,
}

impl Luhn {
    pub fn is_valid(&self) -> bool {
        let digits: Vec<char> = self.number.chars().filter(|c| !c.is_whitespace()).collect();
        
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
}

impl From<&str> for Luhn {
    fn from(input: &str) -> Self {
        Luhn {
            number: input.to_string(),
        }
    }
}

impl From<String> for Luhn {
    fn from(input: String) -> Self {
        Luhn { number: input }
    }
}

impl From<u8> for Luhn {
    fn from(input: u8) -> Self {
        Luhn {
            number: input.to_string(),
        }
    }
}

impl From<u16> for Luhn {
    fn from(input: u16) -> Self {
        Luhn {
            number: input.to_string(),
        }
    }
}

impl From<u32> for Luhn {
    fn from(input: u32) -> Self {
        Luhn {
            number: input.to_string(),
        }
    }
}

impl From<u64> for Luhn {
    fn from(input: u64) -> Self {
        Luhn {
            number: input.to_string(),
        }
    }
}

impl From<usize> for Luhn {
    fn from(input: usize) -> Self {
        Luhn {
            number: input.to_string(),
        }
    }
}