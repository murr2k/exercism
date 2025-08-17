pub fn answer(command: &str) -> Option<i32> {
    // Remove "What is " and "?"
    let command = command.strip_prefix("What is ")?.strip_suffix("?")?;
    
    if command.is_empty() {
        return None;
    }
    
    // Split by spaces and filter out empty strings
    let tokens: Vec<&str> = command.split_whitespace().collect();
    
    if tokens.is_empty() {
        return None;
    }
    
    // Try to parse the first token as a number
    let mut result = tokens[0].parse::<i32>().ok()?;
    
    // If there's only one token and it's a valid number, return it
    if tokens.len() == 1 {
        return Some(result);
    }
    
    let mut i = 1;
    while i < tokens.len() {
        // Expect an operation
        let operation = match tokens.get(i) {
            Some(&"plus") => "+",
            Some(&"minus") => "-",
            Some(&"multiplied") => {
                // Check for "multiplied by"
                if tokens.get(i + 1) == Some(&"by") {
                    i += 1; // Skip "by"
                    "*"
                } else {
                    return None;
                }
            }
            Some(&"divided") => {
                // Check for "divided by"
                if tokens.get(i + 1) == Some(&"by") {
                    i += 1; // Skip "by"
                    "/"
                } else {
                    return None;
                }
            }
            _ => return None,
        };
        
        i += 1;
        
        // Expect a number
        let operand = tokens.get(i)?.parse::<i32>().ok()?;
        
        // Apply the operation
        match operation {
            "+" => result += operand,
            "-" => result -= operand,
            "*" => result *= operand,
            "/" => result /= operand,
            _ => return None,
        }
        
        i += 1;
    }
    
    Some(result)
}

