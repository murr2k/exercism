pub fn annotate(garden: &[&str]) -> Vec<String> {
    if garden.is_empty() {
        return vec![];
    }
    
    let rows = garden.len();
    let cols = if rows > 0 { garden[0].len() } else { 0 };
    
    if cols == 0 {
        return vec!["".to_string()];
    }
    
    let mut result = Vec::new();
    
    for row in 0..rows {
        let mut row_str = String::new();
        
        for col in 0..cols {
            let ch = garden[row].chars().nth(col).unwrap();
            
            if ch == '*' {
                row_str.push('*');
            } else {
                // Count flowers around this position
                let count = count_flowers_around(garden, row, col, rows, cols);
                if count == 0 {
                    row_str.push(' ');
                } else {
                    row_str.push((b'0' + count as u8) as char);
                }
            }
        }
        
        result.push(row_str);
    }
    
    result
}

fn count_flowers_around(garden: &[&str], row: usize, col: usize, rows: usize, cols: usize) -> usize {
    let mut count = 0;
    
    // Check all 8 directions
    let directions = [
        (-1, -1), (-1, 0), (-1, 1),
        (0, -1),           (0, 1),
        (1, -1),  (1, 0),  (1, 1),
    ];
    
    for (dr, dc) in directions.iter() {
        let new_row = row as i32 + dr;
        let new_col = col as i32 + dc;
        
        // Check bounds
        if new_row >= 0 && new_row < rows as i32 && new_col >= 0 && new_col < cols as i32 {
            let nr = new_row as usize;
            let nc = new_col as usize;
            
            if garden[nr].chars().nth(nc).unwrap() == '*' {
                count += 1;
            }
        }
    }
    
    count
}