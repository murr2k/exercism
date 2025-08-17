#[derive(Debug, PartialEq, Eq)]
pub enum Comparison {
    Equal,
    Sublist,
    Superlist,
    Unequal,
}

pub fn sublist(first_list: &[i32], second_list: &[i32]) -> Comparison {
    if first_list == second_list {
        return Comparison::Equal;
    }
    
    if is_sublist(first_list, second_list) {
        return Comparison::Sublist;
    }
    
    if is_sublist(second_list, first_list) {
        return Comparison::Superlist;
    }
    
    Comparison::Unequal
}

fn is_sublist(needle: &[i32], haystack: &[i32]) -> bool {
    if needle.is_empty() {
        return true;
    }
    
    if needle.len() > haystack.len() {
        return false;
    }
    
    haystack.windows(needle.len()).any(|window| window == needle)
}
