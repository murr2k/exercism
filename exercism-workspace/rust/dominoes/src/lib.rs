pub fn chain(input: &[(u8, u8)]) -> Option<Vec<(u8, u8)>> {
    if input.is_empty() {
        return Some(vec![]);
    }
    
    if input.len() == 1 {
        let (a, b) = input[0];
        // A single domino forms a valid chain if both ends are the same
        if a == b {
            return Some(vec![input[0]]);
        } else {
            return None;
        }
    }
    
    // Try to find a valid chain using backtracking
    let mut used = vec![false; input.len()];
    let mut result = Vec::new();
    
    // Try starting with each domino
    for start_idx in 0..input.len() {
        // Try both orientations
        for &orientation in &[false, true] {
            used[start_idx] = true;
            
            let domino = if orientation {
                (input[start_idx].1, input[start_idx].0)
            } else {
                input[start_idx]
            };
            
            result.push(domino);
            
            if find_chain(domino.1, &mut used, &mut result, input) {
                // Check if it forms a valid chain (ends connect)
                if result.first().unwrap().0 == result.last().unwrap().1 {
                    return Some(result);
                }
            }
            
            result.clear();
            used[start_idx] = false;
        }
    }
    
    None
}

fn find_chain(
    need: u8,
    used: &mut Vec<bool>,
    chain: &mut Vec<(u8, u8)>,
    dominoes: &[(u8, u8)],
) -> bool {
    // If all dominoes are used, we're done
    if chain.len() == dominoes.len() {
        return true;
    }
    
    // Try each unused domino
    for i in 0..dominoes.len() {
        if used[i] {
            continue;
        }
        
        let (a, b) = dominoes[i];
        
        // Try normal orientation
        if a == need {
            used[i] = true;
            chain.push((a, b));
            
            if find_chain(b, used, chain, dominoes) {
                return true;
            }
            
            chain.pop();
            used[i] = false;
        }
        
        // Try flipped orientation
        if b == need && a != b {
            used[i] = true;
            chain.push((b, a));
            
            if find_chain(a, used, chain, dominoes) {
                return true;
            }
            
            chain.pop();
            used[i] = false;
        }
    }
    
    false
}