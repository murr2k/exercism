pub fn factors(n: u64) -> Vec<u64> {
    let mut result = Vec::new();
    let mut num = n;
    let mut divisor = 2;
    
    while num > 1 {
        while num % divisor == 0 {
            result.push(divisor);
            num /= divisor;
        }
        divisor += 1;
    }
    
    result
}