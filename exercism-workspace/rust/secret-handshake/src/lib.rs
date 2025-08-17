pub fn actions(n: u8) -> Vec<&'static str> {
    let mut result = Vec::new();
    
    // Check each bit for the corresponding action
    if n & 1 != 0 {
        result.push("wink");
    }
    if n & 2 != 0 {
        result.push("double blink");
    }
    if n & 4 != 0 {
        result.push("close your eyes");
    }
    if n & 8 != 0 {
        result.push("jump");
    }
    
    // If bit 5 is set (value 16), reverse the order
    if n & 16 != 0 {
        result.reverse();
    }
    
    result
}

