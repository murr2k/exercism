pub fn recite(start_bottles: u32, take_down: u32) -> String {
    let mut verses = Vec::new();
    
    for i in 0..take_down {
        let current = start_bottles - i;
        verses.push(verse(current));
    }
    
    verses.join("\n\n")
}

fn verse(n: u32) -> String {
    let current = number_word(n);
    let current_plural = if n == 1 { "bottle" } else { "bottles" };
    
    let next = if n > 0 { n - 1 } else { 0 };
    let next_word = if next == 0 { "no".to_string() } else { number_word(next) };
    let next_plural = if next == 1 { "bottle" } else { "bottles" };
    
    format!(
        "{} green {} hanging on the wall,\n\
         {} green {} hanging on the wall,\n\
         And if one green bottle should accidentally fall,\n\
         There'll be {} green {} hanging on the wall.",
        current, current_plural,
        current, current_plural,
        next_word.to_lowercase(), next_plural
    )
}

fn number_word(n: u32) -> String {
    match n {
        0 => "No".to_string(),
        1 => "One".to_string(),
        2 => "Two".to_string(),
        3 => "Three".to_string(),
        4 => "Four".to_string(),
        5 => "Five".to_string(),
        6 => "Six".to_string(),
        7 => "Seven".to_string(),
        8 => "Eight".to_string(),
        9 => "Nine".to_string(),
        10 => "Ten".to_string(),
        _ => n.to_string(),
    }
}