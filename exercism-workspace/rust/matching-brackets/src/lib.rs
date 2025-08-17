pub fn brackets_are_balanced(string: &str) -> bool {
    let mut stack = Vec::new();
    
    for ch in string.chars() {
        match ch {
            '(' | '[' | '{' => stack.push(ch),
            ')' => {
                if stack.pop() != Some('(') {
                    return false;
                }
            }
            ']' => {
                if stack.pop() != Some('[') {
                    return false;
                }
            }
            '}' => {
                if stack.pop() != Some('{') {
                    return false;
                }
            }
            _ => {} // Ignore non-bracket characters
        }
    }
    
    stack.is_empty()
}