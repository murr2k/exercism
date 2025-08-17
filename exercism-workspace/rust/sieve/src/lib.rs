pub fn primes_up_to(upper_bound: u64) -> Vec<u64> {
    if upper_bound < 2 {
        return vec![];
    }
    
    let mut is_prime = vec![true; (upper_bound + 1) as usize];
    is_prime[0] = false;
    if upper_bound >= 1 {
        is_prime[1] = false;
    }
    
    let mut p = 2;
    while p * p <= upper_bound {
        if is_prime[p as usize] {
            let mut multiple = p * p;
            while multiple <= upper_bound {
                is_prime[multiple as usize] = false;
                multiple += p;
            }
        }
        p += 1;
    }
    
    (2..=upper_bound)
        .filter(|&i| is_prime[i as usize])
        .collect()
}
