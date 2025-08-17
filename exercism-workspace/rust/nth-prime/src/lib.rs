pub fn nth(n: u32) -> u32 {
    let mut primes = Vec::new();
    let mut candidate = 2;
    
    while primes.len() <= n as usize {
        if is_prime(candidate) {
            primes.push(candidate);
        }
        candidate += 1;
    }
    
    primes[n as usize]
}

fn is_prime(num: u32) -> bool {
    if num < 2 {
        return false;
    }
    if num == 2 {
        return true;
    }
    if num % 2 == 0 {
        return false;
    }
    
    let sqrt_num = (num as f64).sqrt() as u32;
    for i in (3..=sqrt_num).step_by(2) {
        if num % i == 0 {
            return false;
        }
    }
    
    true
}