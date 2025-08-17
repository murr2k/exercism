pub fn encrypt(input: &str) -> String {
    // Normalize input: remove non-alphanumeric characters and convert to lowercase
    let normalized: String = input
        .chars()
        .filter(|c| c.is_alphanumeric())
        .map(|c| c.to_ascii_lowercase())
        .collect();
    
    if normalized.is_empty() {
        return String::new();
    }
    
    let len = normalized.len();
    let cols = (len as f64).sqrt().ceil() as usize;
    let rows = if cols * (cols - 1) >= len { cols - 1 } else { cols };
    
    // Create the rectangle
    let mut rectangle: Vec<Vec<char>> = vec![vec![' '; cols]; rows];
    
    // Fill the rectangle row by row
    for (i, ch) in normalized.chars().enumerate() {
        let row = i / cols;
        let col = i % cols;
        if row < rows {
            rectangle[row][col] = ch;
        }
    }
    
    // Read column by column to create the cipher
    let mut result = Vec::new();
    for col in 0..cols {
        let mut column = String::new();
        for row in 0..rows {
            column.push(rectangle[row][col]);
        }
        result.push(column);
    }
    
    result.join(" ")
}

