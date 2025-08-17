#[derive(Debug)]
pub struct Item {
    pub weight: u32,
    pub value: u32,
}

pub fn maximum_value(max_weight: u32, items: &[Item]) -> u32 {
    if items.is_empty() || max_weight == 0 {
        return 0;
    }
    
    let n = items.len();
    let capacity = max_weight as usize;
    
    // Create DP table where dp[i][w] represents the maximum value 
    // that can be obtained with first i items and weight limit w
    let mut dp = vec![vec![0; capacity + 1]; n + 1];
    
    // Fill the DP table
    for i in 1..=n {
        let item = &items[i - 1];
        let item_weight = item.weight as usize;
        let item_value = item.value;
        
        for w in 0..=capacity {
            // Option 1: Don't include the current item
            dp[i][w] = dp[i - 1][w];
            
            // Option 2: Include the current item (if it fits)
            if item_weight <= w {
                dp[i][w] = dp[i][w].max(dp[i - 1][w - item_weight] + item_value);
            }
        }
    }
    
    dp[n][capacity]
}