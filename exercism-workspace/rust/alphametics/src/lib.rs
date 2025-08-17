use std::collections::HashMap;

pub fn solve(input: &str) -> Option<HashMap<char, u8>> {
    // Parse the equation to extract terms and result
    let parts: Vec<&str> = input.split(" == ").collect();
    if parts.len() != 2 {
        return None;
    }
    
    let left_side = parts[0];
    let right_side = parts[1];
    
    // Split left side by " + "
    let addends: Vec<&str> = left_side.split(" + ").collect();
    
    // Collect all unique letters
    let mut letters: Vec<char> = Vec::new();
    for addend in &addends {
        for ch in addend.chars() {
            if ch.is_alphabetic() && !letters.contains(&ch) {
                letters.push(ch);
            }
        }
    }
    for ch in right_side.chars() {
        if ch.is_alphabetic() && !letters.contains(&ch) {
            letters.push(ch);
        }
    }
    
    // Check if we have too many letters (max 10 digits)
    if letters.len() > 10 {
        return None;
    }
    
    // Find letters that cannot be zero (first letters of multi-digit numbers)
    let mut cannot_be_zero: Vec<char> = Vec::new();
    for addend in &addends {
        if addend.len() > 1 {
            if let Some(first_char) = addend.chars().next() {
                if first_char.is_alphabetic() && !cannot_be_zero.contains(&first_char) {
                    cannot_be_zero.push(first_char);
                }
            }
        }
    }
    if right_side.len() > 1 {
        if let Some(first_char) = right_side.chars().next() {
            if first_char.is_alphabetic() && !cannot_be_zero.contains(&first_char) {
                cannot_be_zero.push(first_char);
            }
        }
    }
    
    // Try all possible digit assignments
    let mut assignment = HashMap::new();
    let mut used_digits = vec![false; 10];
    
    if backtrack(&letters, 0, &mut assignment, &mut used_digits, &addends, right_side, &cannot_be_zero) {
        Some(assignment)
    } else {
        None
    }
}

fn backtrack(
    letters: &[char],
    index: usize,
    assignment: &mut HashMap<char, u8>,
    used_digits: &mut [bool],
    addends: &[&str],
    result: &str,
    cannot_be_zero: &[char],
) -> bool {
    // Base case: all letters assigned
    if index == letters.len() {
        return evaluate_equation(addends, result, assignment);
    }
    
    let letter = letters[index];
    
    // Try digits 0-9
    for digit in 0..10 {
        // Skip if digit already used
        if used_digits[digit] {
            continue;
        }
        
        // Skip if this letter cannot be zero and digit is zero
        if digit == 0 && cannot_be_zero.contains(&letter) {
            continue;
        }
        
        // Make assignment
        assignment.insert(letter, digit as u8);
        used_digits[digit] = true;
        
        // Recurse
        if backtrack(letters, index + 1, assignment, used_digits, addends, result, cannot_be_zero) {
            return true;
        }
        
        // Backtrack
        assignment.remove(&letter);
        used_digits[digit] = false;
    }
    
    false
}

fn evaluate_equation(addends: &[&str], result: &str, assignment: &HashMap<char, u8>) -> bool {
    // Convert each addend to a number
    let mut sum = 0;
    for addend in addends {
        let num = word_to_number(addend, assignment);
        sum += num;
    }
    
    // Convert result to a number
    let expected = word_to_number(result, assignment);
    
    sum == expected
}

fn word_to_number(word: &str, assignment: &HashMap<char, u8>) -> u64 {
    let mut result = 0;
    for ch in word.chars() {
        if let Some(&digit) = assignment.get(&ch) {
            result = result * 10 + digit as u64;
        }
    }
    result
}