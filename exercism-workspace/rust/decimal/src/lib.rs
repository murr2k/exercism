use std::cmp::Ordering;
use std::ops::{Add, Sub, Mul};

/// Type implementing arbitrary-precision decimal arithmetic
#[derive(Debug, Clone)]
pub struct Decimal {
    // Store as integer with scale (number of decimal places)
    // e.g., 12.34 = mantissa: 1234, scale: 2
    mantissa: Vec<i8>, // Store digits, least significant first
    scale: usize,       // Number of decimal places
    negative: bool,     // Sign
}

impl Decimal {
    pub fn try_from(input: &str) -> Option<Decimal> {
        let input = input.trim();
        if input.is_empty() {
            return None;
        }

        let negative = input.starts_with('-');
        let input = if negative || input.starts_with('+') {
            &input[1..]
        } else {
            input
        };

        let parts: Vec<&str> = input.split('.').collect();
        if parts.len() > 2 {
            return None;
        }

        let integer_part = parts[0];
        let decimal_part = if parts.len() == 2 { parts[1] } else { "" };
        
        let scale = decimal_part.len();
        
        // Build mantissa from both parts
        let mut mantissa = Vec::new();
        
        // Add decimal part digits (reversed)
        for ch in decimal_part.chars().rev() {
            if !ch.is_ascii_digit() {
                return None;
            }
            mantissa.push((ch as i8) - b'0' as i8);
        }
        
        // Add integer part digits (reversed)
        for ch in integer_part.chars().rev() {
            if !ch.is_ascii_digit() {
                return None;
            }
            mantissa.push((ch as i8) - b'0' as i8);
        }
        
        // Handle empty or all zeros
        if mantissa.is_empty() || mantissa.iter().all(|&d| d == 0) {
            return Some(Decimal {
                mantissa: vec![0],
                scale: 0,
                negative: false,
            });
        }
        
        // Remove leading zeros (which are at the end of our reversed array)
        while mantissa.len() > 1 && mantissa.last() == Some(&0) {
            mantissa.pop();
        }
        
        Some(Decimal {
            mantissa,
            scale,
            negative,
        })
    }
    
    fn normalize(&mut self) {
        // Remove trailing zeros from decimal part
        while self.scale > 0 && !self.mantissa.is_empty() && self.mantissa[0] == 0 {
            self.mantissa.remove(0);
            self.scale -= 1;
        }
        
        // Remove leading zeros
        while self.mantissa.len() > 1 && self.mantissa.last() == Some(&0) {
            self.mantissa.pop();
        }
        
        // Handle zero
        if self.mantissa.is_empty() || self.mantissa.iter().all(|&d| d == 0) {
            self.mantissa = vec![0];
            self.scale = 0;
            self.negative = false;
        }
    }
    
    fn align_scales(&self, other: &Self) -> (Vec<i8>, Vec<i8>, usize) {
        let max_scale = self.scale.max(other.scale);
        
        let mut a = self.mantissa.clone();
        let mut b = other.mantissa.clone();
        
        // Pad with zeros to align decimal points
        for _ in self.scale..max_scale {
            a.insert(0, 0);
        }
        for _ in other.scale..max_scale {
            b.insert(0, 0);
        }
        
        // Make same length for easier comparison
        let max_len = a.len().max(b.len());
        while a.len() < max_len {
            a.push(0);
        }
        while b.len() < max_len {
            b.push(0);
        }
        
        (a, b, max_scale)
    }
    
    fn compare_magnitude(&self, other: &Self) -> Ordering {
        let (a, b, _) = self.align_scales(other);
        
        // Compare from most significant digit
        for i in (0..a.len()).rev() {
            match a[i].cmp(&b[i]) {
                Ordering::Equal => continue,
                other => return other,
            }
        }
        Ordering::Equal
    }
    
    fn add_magnitudes(a: &[i8], b: &[i8]) -> Vec<i8> {
        let mut result = Vec::new();
        let mut carry = 0;
        let max_len = a.len().max(b.len());
        
        for i in 0..max_len {
            let digit_a = if i < a.len() { a[i] } else { 0 };
            let digit_b = if i < b.len() { b[i] } else { 0 };
            let sum = digit_a + digit_b + carry;
            result.push(sum % 10);
            carry = sum / 10;
        }
        
        if carry > 0 {
            result.push(carry);
        }
        
        result
    }
    
    fn sub_magnitudes(a: &[i8], b: &[i8]) -> (Vec<i8>, bool) {
        // Assumes a >= b in magnitude
        let mut result = Vec::new();
        let mut borrow = 0;
        
        for i in 0..a.len() {
            let digit_b = if i < b.len() { b[i] } else { 0 };
            let mut diff = a[i] - digit_b - borrow;
            if diff < 0 {
                diff += 10;
                borrow = 1;
            } else {
                borrow = 0;
            }
            result.push(diff);
        }
        
        // Remove leading zeros
        while result.len() > 1 && result.last() == Some(&0) {
            result.pop();
        }
        
        (result, false)
    }
    
    fn mul_magnitudes(a: &[i8], b: &[i8]) -> Vec<i8> {
        let mut result = vec![0; a.len() + b.len()];
        
        for (i, &digit_a) in a.iter().enumerate() {
            let mut carry = 0;
            for (j, &digit_b) in b.iter().enumerate() {
                let prod = digit_a * digit_b + result[i + j] + carry;
                result[i + j] = prod % 10;
                carry = prod / 10;
            }
            if carry > 0 {
                result[i + b.len()] += carry;
            }
        }
        
        // Remove leading zeros
        while result.len() > 1 && result.last() == Some(&0) {
            result.pop();
        }
        
        result
    }
}

impl PartialEq for Decimal {
    fn eq(&self, other: &Self) -> bool {
        // Normalize and compare
        let mut a = self.clone();
        let mut b = other.clone();
        a.normalize();
        b.normalize();
        
        a.negative == b.negative && a.scale == b.scale && a.mantissa == b.mantissa
    }
}

impl Eq for Decimal {}

impl PartialOrd for Decimal {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl Ord for Decimal {
    fn cmp(&self, other: &Self) -> Ordering {
        // Handle signs
        match (self.negative, other.negative) {
            (true, false) => return Ordering::Less,
            (false, true) => return Ordering::Greater,
            (false, false) => self.compare_magnitude(other),
            (true, true) => other.compare_magnitude(self),
        }
    }
}

impl Add for Decimal {
    type Output = Decimal;
    
    fn add(self, other: Self) -> Self::Output {
        let (a, b, scale) = self.align_scales(&other);
        
        let mantissa = match (self.negative, other.negative) {
            (false, false) | (true, true) => {
                // Same sign - add magnitudes
                Self::add_magnitudes(&a, &b)
            }
            _ => {
                // Different signs - subtract smaller from larger
                match self.compare_magnitude(&other) {
                    Ordering::Greater | Ordering::Equal => {
                        Self::sub_magnitudes(&a, &b).0
                    }
                    Ordering::Less => {
                        Self::sub_magnitudes(&b, &a).0
                    }
                }
            }
        };
        
        let negative = match (self.negative, other.negative) {
            (false, false) => false,
            (true, true) => true,
            _ => {
                match self.compare_magnitude(&other) {
                    Ordering::Greater => self.negative,
                    Ordering::Less => other.negative,
                    Ordering::Equal => false,
                }
            }
        };
        
        let mut result = Decimal { mantissa, scale, negative };
        result.normalize();
        result
    }
}

impl Sub for Decimal {
    type Output = Decimal;
    
    fn sub(self, other: Self) -> Self::Output {
        // a - b = a + (-b)
        let negated = Decimal {
            mantissa: other.mantissa,
            scale: other.scale,
            negative: !other.negative,
        };
        self.add(negated)
    }
}

impl Mul for Decimal {
    type Output = Decimal;
    
    fn mul(self, other: Self) -> Self::Output {
        let mantissa = Self::mul_magnitudes(&self.mantissa, &other.mantissa);
        let scale = self.scale + other.scale;
        let negative = self.negative != other.negative;
        
        let mut result = Decimal { mantissa, scale, negative };
        result.normalize();
        result
    }
}