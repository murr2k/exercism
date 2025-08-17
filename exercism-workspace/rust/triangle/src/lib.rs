pub struct Triangle {
    sides: [u64; 3],
}

impl Triangle {
    pub fn build(sides: [u64; 3]) -> Option<Triangle> {
        let [a, b, c] = sides;
        
        // Check if all sides are positive
        if a == 0 || b == 0 || c == 0 {
            return None;
        }
        
        // Check triangle inequality: each side must be less than the sum of the other two
        if a + b <= c || a + c <= b || b + c <= a {
            return None;
        }
        
        Some(Triangle { sides })
    }

    pub fn is_equilateral(&self) -> bool {
        let [a, b, c] = self.sides;
        a == b && b == c
    }

    pub fn is_scalene(&self) -> bool {
        let [a, b, c] = self.sides;
        a != b && b != c && a != c
    }

    pub fn is_isosceles(&self) -> bool {
        let [a, b, c] = self.sides;
        a == b || b == c || a == c
    }
}