// The code below is a stub. Just enough to satisfy the compiler.
// In order to pass the tests you can add-to or change any of this code.

#[derive(Debug, PartialEq, Eq)]
pub enum Error {
    InvalidRowCount(usize),
    InvalidColumnCount(usize),
}

pub fn convert(input: &str) -> Result<String, Error> {
    let lines: Vec<&str> = input.lines().collect();
    
    // Check if number of lines is a multiple of 4
    if lines.len() % 4 != 0 {
        return Err(Error::InvalidRowCount(lines.len()));
    }
    
    // Check if each line's length is a multiple of 3
    if !lines.is_empty() {
        let line_length = lines[0].len();
        if line_length % 3 != 0 {
            return Err(Error::InvalidColumnCount(line_length));
        }
        
        // All lines should have the same length
        for line in &lines {
            if line.len() != line_length {
                return Err(Error::InvalidColumnCount(line.len()));
            }
        }
    }
    
    if lines.is_empty() {
        return Ok(String::new());
    }
    
    let num_rows = lines.len() / 4;
    let num_cols = lines[0].len() / 3;
    let mut result = String::new();
    
    // Process each row of digits
    for row in 0..num_rows {
        let row_start = row * 4;
        
        // Process each digit in this row
        for col in 0..num_cols {
            let col_start = col * 3;
            
            // Extract the 3x4 pattern for this digit
            let mut pattern = Vec::new();
            for r in 0..4 {
                if row_start + r < lines.len() {
                    let line = lines[row_start + r];
                    let segment = if col_start + 3 <= line.len() {
                        &line[col_start..col_start + 3]
                    } else {
                        ""
                    };
                    pattern.push(segment);
                } else {
                    pattern.push("");
                }
            }
            
            // Recognize the digit
            let digit = recognize_digit(&pattern);
            result.push_str(&digit);
        }
        
        // Add comma between rows if there are multiple rows
        if row < num_rows - 1 {
            result.push(',');
        }
    }
    
    Ok(result)
}

fn recognize_digit(pattern: &[&str]) -> String {
    match pattern {
        [" _ ", "| |", "|_|", "   "] => "0".to_string(),
        ["   ", "  |", "  |", "   "] => "1".to_string(),
        [" _ ", " _|", "|_ ", "   "] => "2".to_string(),
        [" _ ", " _|", " _|", "   "] => "3".to_string(),
        ["   ", "|_|", "  |", "   "] => "4".to_string(),
        [" _ ", "|_ ", " _|", "   "] => "5".to_string(),
        [" _ ", "|_ ", "|_|", "   "] => "6".to_string(),
        [" _ ", "  |", "  |", "   "] => "7".to_string(),
        [" _ ", "|_|", "|_|", "   "] => "8".to_string(),
        [" _ ", "|_|", " _|", "   "] => "9".to_string(),
        _ => "?".to_string(),
    }
}