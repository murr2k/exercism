pub fn build_proverb(list: &[&str]) -> String {
    if list.is_empty() {
        return String::new();
    }
    
    let mut lines = Vec::new();
    
    // Add the "For want of..." lines
    for window in list.windows(2) {
        lines.push(format!("For want of a {} the {} was lost.", window[0], window[1]));
    }
    
    // Add the final line
    lines.push(format!("And all for the want of a {}.", list[0]));
    
    lines.join("\n")
}