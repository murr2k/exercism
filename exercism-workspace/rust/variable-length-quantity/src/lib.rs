#[derive(Debug, PartialEq, Eq)]
pub enum Error {
    IncompleteNumber,
}

/// Convert a list of numbers to a stream of bytes encoded with variable length encoding.
pub fn to_bytes(values: &[u32]) -> Vec<u8> {
    let mut result = Vec::new();
    
    for &value in values {
        if value == 0 {
            result.push(0);
            continue;
        }
        
        let mut temp = Vec::new();
        let mut n = value;
        
        // Extract groups of 7 bits from right to left
        temp.push((n & 0x7F) as u8); // First byte doesn't have continuation bit
        n >>= 7;
        
        while n > 0 {
            temp.push(((n & 0x7F) | 0x80) as u8); // Set continuation bit
            n >>= 7;
        }
        
        // Reverse to get correct order (most significant first)
        temp.reverse();
        result.extend(temp);
    }
    
    result
}

/// Given a stream of bytes, extract all numbers which are encoded in there.
pub fn from_bytes(bytes: &[u8]) -> Result<Vec<u32>, Error> {
    let mut result = Vec::new();
    let mut i = 0;
    
    while i < bytes.len() {
        let mut value = 0u32;
        let mut found_end = false;
        
        // Read bytes for this number
        while i < bytes.len() {
            let byte = bytes[i];
            value = (value << 7) | (byte & 0x7F) as u32;
            i += 1;
            
            if byte & 0x80 == 0 {
                // No continuation bit, this is the end
                found_end = true;
                break;
            }
        }
        
        if !found_end {
            return Err(Error::IncompleteNumber);
        }
        
        result.push(value);
    }
    
    Ok(result)
}