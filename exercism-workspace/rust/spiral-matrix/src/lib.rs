pub fn spiral_matrix(size: u32) -> Vec<Vec<u32>> {
    if size == 0 {
        return vec![];
    }
    
    let size = size as usize;
    let mut matrix = vec![vec![0; size]; size];
    
    let mut num = 1;
    let mut top = 0;
    let mut bottom = size - 1;
    let mut left = 0;
    let mut right = size - 1;
    
    while top <= bottom && left <= right {
        // Fill top row
        for col in left..=right {
            matrix[top][col] = num;
            num += 1;
        }
        top += 1;
        
        // Fill right column
        for row in top..=bottom {
            matrix[row][right] = num;
            num += 1;
        }
        if right > 0 {
            right -= 1;
        }
        
        // Fill bottom row (if there's still a row to fill)
        if top <= bottom {
            for col in (left..=right).rev() {
                matrix[bottom][col] = num;
                num += 1;
            }
            if bottom > 0 {
                bottom -= 1;
            }
        }
        
        // Fill left column (if there's still a column to fill)
        if left <= right {
            for row in (top..=bottom).rev() {
                matrix[row][left] = num;
                num += 1;
            }
            left += 1;
        }
    }
    
    matrix
}
