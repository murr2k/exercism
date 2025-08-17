use std::collections::HashSet;

pub fn sum_of_multiples(limit: u32, factors: &[u32]) -> u32 {
    let mut multiples = HashSet::new();
    
    for &factor in factors {
        // Skip 0 as a factor since it doesn't generate valid multiples
        if factor == 0 {
            continue;
        }
        
        // Generate all multiples of this factor that are less than limit
        let mut multiple = factor;
        while multiple < limit {
            multiples.insert(multiple);
            multiple += factor;
        }
    }
    
    multiples.iter().sum()
}