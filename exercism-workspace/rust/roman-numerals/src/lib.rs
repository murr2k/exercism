use std::fmt::{Display, Formatter, Result};

pub struct Roman {
    value: u32,
}

impl Display for Roman {
    fn fmt(&self, f: &mut Formatter<'_>) -> Result {
        let mut num = self.value;
        let mut result = String::new();
        
        let values = [
            (1000, "M"),
            (900, "CM"),
            (500, "D"),
            (400, "CD"),
            (100, "C"),
            (90, "XC"),
            (50, "L"),
            (40, "XL"),
            (10, "X"),
            (9, "IX"),
            (5, "V"),
            (4, "IV"),
            (1, "I"),
        ];
        
        for (value, symbol) in &values {
            while num >= *value {
                result.push_str(symbol);
                num -= value;
            }
        }
        
        write!(f, "{}", result)
    }
}

impl From<u32> for Roman {
    fn from(num: u32) -> Self {
        Roman { value: num }
    }
}