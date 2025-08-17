pub fn find_saddle_points(input: &[Vec<u64>]) -> Vec<(usize, usize)> {
    if input.is_empty() || input[0].is_empty() {
        return vec![];
    }
    
    let rows = input.len();
    let cols = input[0].len();
    let mut saddle_points = Vec::new();
    
    for row in 0..rows {
        for col in 0..cols {
            let value = input[row][col];
            
            // Check if it's maximum in its row
            let is_row_max = input[row].iter().all(|&x| value >= x);
            
            // Check if it's minimum in its column
            let is_col_min = (0..rows).all(|r| value <= input[r][col]);
            
            if is_row_max && is_col_min {
                saddle_points.push((row, col));
            }
        }
    }
    
    saddle_points
}
