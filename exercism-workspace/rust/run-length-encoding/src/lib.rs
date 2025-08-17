pub fn encode(source: &str) -> String {
    let mut result = String::new();
    let mut chars = source.chars().peekable();
    
    while let Some(ch) = chars.next() {
        let mut count = 1;
        
        while chars.peek() == Some(&ch) {
            count += 1;
            chars.next();
        }
        
        if count == 1 {
            result.push(ch);
        } else {
            result.push_str(&format!("{}{}", count, ch));
        }
    }
    
    result
}

pub fn decode(source: &str) -> String {
    let mut result = String::new();
    let mut count_str = String::new();
    
    for ch in source.chars() {
        if ch.is_ascii_digit() {
            count_str.push(ch);
        } else {
            let count = if count_str.is_empty() {
                1
            } else {
                count_str.parse().unwrap_or(1)
            };
            
            result.push_str(&ch.to_string().repeat(count));
            count_str.clear();
        }
    }
    
    result
}