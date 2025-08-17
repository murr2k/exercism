use std::collections::HashSet;
use std::collections::BTreeMap;

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Palindrome {
    value: u64,
    factors: HashSet<(u64, u64)>,
}

impl Palindrome {
    pub fn value(&self) -> u64 {
        self.value
    }

    pub fn into_factors(self) -> HashSet<(u64, u64)> {
        self.factors
    }
}

pub fn palindrome_products(min: u64, max: u64) -> Option<(Palindrome, Palindrome)> {
    if min > max {
        return None;
    }
    
    let mut palindromes: BTreeMap<u64, HashSet<(u64, u64)>> = BTreeMap::new();
    
    for i in min..=max {
        for j in i..=max {  // Start from i to avoid duplicates
            let product = i * j;
            
            if is_palindrome(product) {
                palindromes.entry(product)
                    .or_insert_with(HashSet::new)
                    .insert((i, j));
            }
        }
    }
    
    if palindromes.is_empty() {
        return None;
    }
    
    // Get minimum palindrome
    let (min_value, min_factors) = palindromes.iter().next().unwrap();
    let min_palindrome = Palindrome {
        value: *min_value,
        factors: min_factors.clone(),
    };
    
    // Get maximum palindrome
    let (max_value, max_factors) = palindromes.iter().last().unwrap();
    let max_palindrome = Palindrome {
        value: *max_value,
        factors: max_factors.clone(),
    };
    
    Some((min_palindrome, max_palindrome))
}

fn is_palindrome(n: u64) -> bool {
    let s = n.to_string();
    let reversed = s.chars().rev().collect::<String>();
    s == reversed
}