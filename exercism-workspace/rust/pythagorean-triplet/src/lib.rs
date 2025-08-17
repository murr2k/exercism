use std::collections::HashSet;

pub fn find(sum: u32) -> HashSet<[u32; 3]> {
    let mut triplets = HashSet::new();
    
    // For a Pythagorean triplet a + b + c = sum and a² + b² = c²
    // Since a < b < c, we need to find the proper bounds
    
    for a in 1..sum/3 {
        for b in (a + 1)..sum/2 {
            let c = sum - a - b;
            
            // Ensure b < c (which means b < sum - a - b, so 2b < sum - a, so b < (sum - a)/2)
            if b >= c {
                break;
            }
            
            // Check if this forms a Pythagorean triplet
            if a * a + b * b == c * c {
                triplets.insert([a, b, c]);
            }
        }
    }
    
    triplets
}