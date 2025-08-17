/// Check a Luhn checksum.
pub fn is_valid(code: &str) -> bool {
    let digits: Result<Vec<u32>, _> = code
        .chars()
        .filter(|c| !c.is_whitespace())
        .map(|c| c.to_digit(10).ok_or(()))
        .collect();
    
    let digits = match digits {
        Ok(d) => d,
        Err(_) => return false, // Contains non-digit characters
    };
    
    if digits.len() < 2 {
        return false;
    }
    
    let sum: u32 = digits
        .iter()
        .rev()
        .enumerate()
        .map(|(i, &digit)| {
            if i % 2 == 1 {
                // Every second digit from the right, double it
                let doubled = digit * 2;
                if doubled > 9 {
                    doubled - 9
                } else {
                    doubled
                }
            } else {
                digit
            }
        })
        .sum();
    
    sum % 10 == 0
}
