pub fn get_diamond(c: char) -> Vec<String> {
    if c < 'A' || c > 'Z' {
        return vec![];
    }
    
    let size = (c as u8 - b'A') as usize;
    let width = size * 2 + 1;
    let mut diamond = Vec::new();
    
    // Top half + middle
    for i in 0..=size {
        let letter = (b'A' + i as u8) as char;
        let line = create_line(letter, i, width);
        diamond.push(line);
    }
    
    // Bottom half (excluding middle)
    for i in (0..size).rev() {
        let letter = (b'A' + i as u8) as char;
        let line = create_line(letter, i, width);
        diamond.push(line);
    }
    
    diamond
}

fn create_line(letter: char, position: usize, width: usize) -> String {
    if position == 0 {
        // First row: just 'A' centered
        let padding = width / 2;
        format!("{:padding$}{}{:padding$}", "", letter, "", padding = padding)
    } else {
        // Other rows: letter at start and end with spaces in between
        let outer_padding = width / 2 - position;
        let inner_padding = position * 2 - 1;
        format!(
            "{:outer_padding$}{}{:inner_padding$}{}{:outer_padding$}",
            "", letter, "", letter, "",
            outer_padding = outer_padding,
            inner_padding = inner_padding
        )
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_diamond_a() {
        let result = get_diamond('A');
        assert_eq!(result, vec!["A"]);
    }

    #[test]
    fn test_diamond_c() {
        let result = get_diamond('C');
        println!("Diamond C:");
        for line in &result {
            println!("'{}'", line);
        }
        // Expected:
        // "  A  "
        // " B B "
        // "C   C"
        // " B B "
        // "  A  "
        assert_eq!(result.len(), 5);
    }
}