pub struct PascalsTriangle {
    rows: Vec<Vec<u32>>,
}

impl PascalsTriangle {
    pub fn new(row_count: u32) -> Self {
        let mut rows: Vec<Vec<u32>> = Vec::new();
        
        for n in 0..row_count {
            let mut row = Vec::new();
            for k in 0..=n {
                if k == 0 || k == n {
                    row.push(1);
                } else {
                    let prev_row: &Vec<u32> = &rows[(n - 1) as usize];
                    let val = prev_row[(k - 1) as usize] + prev_row[k as usize];
                    row.push(val);
                }
            }
            rows.push(row);
        }
        
        PascalsTriangle { rows }
    }

    pub fn rows(&self) -> Vec<Vec<u32>> {
        self.rows.clone()
    }
}