pub struct Matrix {
    data: Vec<Vec<u32>>,
}

impl Matrix {
    pub fn new(input: &str) -> Self {
        let data = input
            .lines()
            .map(|line| {
                line.split_whitespace()
                    .map(|num| num.parse::<u32>().unwrap())
                    .collect()
            })
            .collect();
        
        Matrix { data }
    }

    pub fn row(&self, row_no: usize) -> Option<Vec<u32>> {
        if row_no == 0 || row_no > self.data.len() {
            None
        } else {
            Some(self.data[row_no - 1].clone())
        }
    }

    pub fn column(&self, col_no: usize) -> Option<Vec<u32>> {
        if col_no == 0 || self.data.is_empty() || col_no > self.data[0].len() {
            None
        } else {
            Some(self.data.iter().map(|row| row[col_no - 1]).collect())
        }
    }
}