use std::collections::HashMap;

pub fn lowest_price(books: &[u32]) -> u32 {
    if books.is_empty() {
        return 0;
    }
    
    // Count how many of each book we have
    let mut counts = HashMap::new();
    for &book in books {
        *counts.entry(book).or_insert(0) += 1;
    }
    
    // Convert to a sorted vector of counts
    let mut book_counts: Vec<u32> = counts.values().cloned().collect();
    book_counts.sort_unstable_by(|a, b| b.cmp(a)); // Sort in descending order
    
    calculate_min_cost(&book_counts)
}

fn calculate_min_cost(counts: &[u32]) -> u32 {
    if counts.is_empty() || counts.iter().all(|&c| c == 0) {
        return 0;
    }
    
    let mut min_cost = u32::MAX;
    
    // Try grouping with different sizes (1 to 5)
    for group_size in 1..=5.min(counts.len()) {
        // Check if we can form a group of this size
        if counts.len() >= group_size && counts[group_size - 1] > 0 {
            // Calculate cost for this group
            let group_cost = group_price(group_size);
            
            // Create new counts after removing one of each book in this group
            let mut new_counts = counts.to_vec();
            for i in 0..group_size {
                new_counts[i] -= 1;
            }
            
            // Remove zeros and sort again
            new_counts.retain(|&c| c > 0);
            new_counts.sort_unstable_by(|a, b| b.cmp(a));
            
            // Recursively calculate the cost for remaining books
            let remaining_cost = calculate_min_cost(&new_counts);
            
            min_cost = min_cost.min(group_cost + remaining_cost);
        }
    }
    
    min_cost
}

fn group_price(size: usize) -> u32 {
    let base_price = 800;
    let total_base = base_price * size as u32;
    
    match size {
        1 => total_base,                           // 0% discount
        2 => (total_base as f64 * 0.95) as u32,   // 5% discount
        3 => (total_base as f64 * 0.90) as u32,   // 10% discount
        4 => (total_base as f64 * 0.80) as u32,   // 20% discount
        5 => (total_base as f64 * 0.75) as u32,   // 25% discount
        _ => total_base,
    }
}