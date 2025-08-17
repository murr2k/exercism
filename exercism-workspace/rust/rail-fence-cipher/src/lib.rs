pub struct RailFence {
    rails: usize,
}

impl RailFence {
    pub fn new(rails: u32) -> RailFence {
        RailFence {
            rails: rails as usize,
        }
    }

    pub fn encode(&self, text: &str) -> String {
        if self.rails <= 1 {
            return text.to_string();
        }

        let chars: Vec<char> = text.chars().collect();
        let mut rails: Vec<Vec<char>> = vec![Vec::new(); self.rails];
        
        let mut rail = 0;
        let mut direction = 1; // 1 for down, -1 for up

        for ch in chars {
            rails[rail].push(ch);
            
            // Change direction at the ends
            if rail == 0 {
                direction = 1;
            } else if rail == self.rails - 1 {
                direction = -1;
            }
            
            rail = ((rail as i32) + direction) as usize;
        }

        rails.into_iter().flatten().collect()
    }

    pub fn decode(&self, cipher: &str) -> String {
        if self.rails <= 1 {
            return cipher.to_string();
        }

        let chars: Vec<char> = cipher.chars().collect();
        let len = chars.len();
        
        // Create a pattern to determine which positions belong to which rail
        let mut pattern = vec![0; len];
        let mut rail = 0;
        let mut direction = 1;
        
        for i in 0..len {
            pattern[i] = rail;
            
            if rail == 0 {
                direction = 1;
            } else if rail == self.rails - 1 {
                direction = -1;
            }
            
            rail = ((rail as i32) + direction) as usize;
        }
        
        // Count how many characters belong to each rail
        let mut rail_counts = vec![0; self.rails];
        for &rail_num in &pattern {
            rail_counts[rail_num] += 1;
        }
        
        // Distribute cipher characters to rails
        let mut rails: Vec<Vec<char>> = vec![Vec::new(); self.rails];
        let mut cipher_idx = 0;
        
        for rail_num in 0..self.rails {
            let count = rail_counts[rail_num];
            for _ in 0..count {
                if cipher_idx < chars.len() {
                    rails[rail_num].push(chars[cipher_idx]);
                    cipher_idx += 1;
                }
            }
        }
        
        // Read characters according to the pattern
        let mut result = Vec::new();
        let mut rail_indices = vec![0; self.rails];
        
        for &rail_num in &pattern {
            if rail_indices[rail_num] < rails[rail_num].len() {
                result.push(rails[rail_num][rail_indices[rail_num]]);
                rail_indices[rail_num] += 1;
            }
        }
        
        result.into_iter().collect()
    }
}