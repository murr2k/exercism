#[derive(Debug, PartialEq, Eq)]
pub enum Error {
    SpanTooLong,
    InvalidDigit(char),
}

pub fn lsp(string_digits: &str, span: usize) -> Result<u64, Error> {
    // Check if span is longer than string length
    if span > string_digits.len() {
        return Err(Error::SpanTooLong);
    }
    
    // Handle span 0 case - product of 0 elements is 1
    if span == 0 {
        return Ok(1);
    }
    
    // Parse string to ensure all characters are digits
    let digits: Result<Vec<u64>, Error> = string_digits
        .chars()
        .map(|c| {
            c.to_digit(10)
                .map(|d| d as u64)
                .ok_or(Error::InvalidDigit(c))
        })
        .collect();
        
    let digits = digits?;
    
    // Calculate the product for each consecutive span and find the maximum
    let mut max_product = 0;
    
    for window in digits.windows(span) {
        let product = window.iter().product::<u64>();
        max_product = max_product.max(product);
    }
    
    Ok(max_product)
}