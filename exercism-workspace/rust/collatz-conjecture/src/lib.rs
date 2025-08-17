pub fn collatz(n: u64) -> Option<u64> {
    if n == 0 {
        return None;
    }
    
    let mut current = n;
    let mut steps = 0;
    
    while current != 1 {
        if current % 2 == 0 {
            current /= 2;
        } else {
            current = current * 3 + 1;
        }
        steps += 1;
    }
    
    Some(steps)
}
