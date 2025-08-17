pub fn plants(diagram: &str, student: &str) -> Vec<&'static str> {
    let students = vec![
        "Alice", "Bob", "Charlie", "David", "Eve", "Fred",
        "Ginny", "Harriet", "Ileana", "Joseph", "Kincaid", "Larry"
    ];
    
    let student_index = students.iter().position(|&s| s == student).unwrap();
    let plant_index = student_index * 2;
    
    let lines: Vec<&str> = diagram.lines().collect();
    let first_row = lines[0];
    let second_row = lines[1];
    
    let mut result = Vec::new();
    
    // Get plants from first row
    if let Some(plant1) = first_row.chars().nth(plant_index) {
        result.push(plant_name(plant1));
    }
    if let Some(plant2) = first_row.chars().nth(plant_index + 1) {
        result.push(plant_name(plant2));
    }
    
    // Get plants from second row
    if let Some(plant3) = second_row.chars().nth(plant_index) {
        result.push(plant_name(plant3));
    }
    if let Some(plant4) = second_row.chars().nth(plant_index + 1) {
        result.push(plant_name(plant4));
    }
    
    result
}

fn plant_name(plant_char: char) -> &'static str {
    match plant_char {
        'G' => "grass",
        'C' => "clover",
        'R' => "radishes",
        'V' => "violets",
        _ => "unknown",
    }
}

