pub fn count(lines: &[&str]) -> u32 {
    if lines.is_empty() {
        return 0;
    }
    
    let grid: Vec<Vec<char>> = lines.iter()
        .map(|line| line.chars().collect())
        .collect();
    
    let rows = grid.len();
    if rows == 0 {
        return 0;
    }
    
    // Find all possible corners (positions with '+')
    let mut corners = Vec::new();
    for r in 0..rows {
        for c in 0..grid[r].len() {
            if grid[r][c] == '+' {
                corners.push((r, c));
            }
        }
    }
    
    let mut rectangle_count = 0;
    
    // Try every pair of corners as potential top-left and bottom-right corners
    for i in 0..corners.len() {
        for j in i + 1..corners.len() {
            let (r1, c1) = corners[i];
            let (r2, c2) = corners[j];
            
            // Only consider as top-left and bottom-right
            if r1 < r2 && c1 < c2 {
                if is_valid_rectangle(&grid, r1, c1, r2, c2) {
                    rectangle_count += 1;
                }
            }
        }
    }
    
    rectangle_count
}

fn is_valid_rectangle(grid: &[Vec<char>], top: usize, left: usize, bottom: usize, right: usize) -> bool {
    // Check if we have corners at all four positions
    if !is_corner_or_edge(grid, top, left) ||
       !is_corner_or_edge(grid, top, right) ||
       !is_corner_or_edge(grid, bottom, left) ||
       !is_corner_or_edge(grid, bottom, right) {
        return false;
    }
    
    // Check top edge
    for c in left + 1..right {
        if !is_horizontal_edge(grid, top, c) {
            return false;
        }
    }
    
    // Check bottom edge
    for c in left + 1..right {
        if !is_horizontal_edge(grid, bottom, c) {
            return false;
        }
    }
    
    // Check left edge
    for r in top + 1..bottom {
        if !is_vertical_edge(grid, r, left) {
            return false;
        }
    }
    
    // Check right edge
    for r in top + 1..bottom {
        if !is_vertical_edge(grid, r, right) {
            return false;
        }
    }
    
    true
}

fn is_corner_or_edge(grid: &[Vec<char>], r: usize, c: usize) -> bool {
    if r >= grid.len() || c >= grid[r].len() {
        return false;
    }
    grid[r][c] == '+'
}

fn is_horizontal_edge(grid: &[Vec<char>], r: usize, c: usize) -> bool {
    if r >= grid.len() || c >= grid[r].len() {
        return false;
    }
    grid[r][c] == '-' || grid[r][c] == '+'
}

fn is_vertical_edge(grid: &[Vec<char>], r: usize, c: usize) -> bool {
    if r >= grid.len() || c >= grid[r].len() {
        return false;
    }
    grid[r][c] == '|' || grid[r][c] == '+'
}