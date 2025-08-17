use std::collections::{HashSet, VecDeque};

#[derive(PartialEq, Eq, Debug, Clone)]
pub enum Bucket {
    One,
    Two,
}

/// A struct to hold your results in.
#[derive(PartialEq, Eq, Debug)]
pub struct BucketStats {
    /// The total number of "moves" it should take to reach the desired number of liters, including
    /// the first fill.
    pub moves: u8,
    /// Which bucket should end up with the desired number of liters? (Either "one" or "two")
    pub goal_bucket: Bucket,
    /// How many liters are left in the other bucket?
    pub other_bucket: u8,
}

#[derive(Clone, Copy, PartialEq, Eq, Hash, Debug)]
struct State {
    bucket1: u8,
    bucket2: u8,
}

/// Solve the bucket problem
pub fn solve(
    capacity_1: u8,
    capacity_2: u8,
    goal: u8,
    start_bucket: &Bucket,
) -> Option<BucketStats> {
    // Check if goal is impossible
    if goal > capacity_1.max(capacity_2) {
        return None;
    }
    
    // Check if goal is achievable using GCD
    fn gcd(a: u8, b: u8) -> u8 {
        if b == 0 { a } else { gcd(b, a % b) }
    }
    
    if goal % gcd(capacity_1, capacity_2) != 0 {
        return None;
    }
    
    // BFS to find shortest path
    let mut queue = VecDeque::new();
    let mut visited = HashSet::new();
    let mut parent = std::collections::HashMap::new();
    
    // Initial state based on starting bucket
    let initial = match start_bucket {
        Bucket::One => State { bucket1: capacity_1, bucket2: 0 },
        Bucket::Two => State { bucket1: 0, bucket2: capacity_2 },
    };
    
    // Check if initial state is the goal
    if initial.bucket1 == goal {
        return Some(BucketStats {
            moves: 1,
            goal_bucket: Bucket::One,
            other_bucket: initial.bucket2,
        });
    }
    if initial.bucket2 == goal {
        return Some(BucketStats {
            moves: 1,
            goal_bucket: Bucket::Two,
            other_bucket: initial.bucket1,
        });
    }
    
    // The forbidden state: can't have the other bucket full and starting bucket empty
    let forbidden_state = match start_bucket {
        Bucket::One => State { bucket1: 0, bucket2: capacity_2 },
        Bucket::Two => State { bucket1: capacity_1, bucket2: 0 },
    };
    
    queue.push_back(initial);
    visited.insert(initial);
    visited.insert(forbidden_state); // Never allow reaching this state
    parent.insert(initial, None);
    
    while let Some(current) = queue.pop_front() {
        // Generate all possible next states
        let mut next_states = Vec::new();
        
        // Fill bucket 1
        if current.bucket1 < capacity_1 {
            next_states.push(State { bucket1: capacity_1, bucket2: current.bucket2 });
        }
        
        // Fill bucket 2
        if current.bucket2 < capacity_2 {
            next_states.push(State { bucket1: current.bucket1, bucket2: capacity_2 });
        }
        
        // Empty bucket 1
        if current.bucket1 > 0 {
            next_states.push(State { bucket1: 0, bucket2: current.bucket2 });
        }
        
        // Empty bucket 2
        if current.bucket2 > 0 {
            next_states.push(State { bucket1: current.bucket1, bucket2: 0 });
        }
        
        // Pour bucket 1 into bucket 2
        if current.bucket1 > 0 && current.bucket2 < capacity_2 {
            let amount = (capacity_2 - current.bucket2).min(current.bucket1);
            next_states.push(State {
                bucket1: current.bucket1 - amount,
                bucket2: current.bucket2 + amount,
            });
        }
        
        // Pour bucket 2 into bucket 1
        if current.bucket2 > 0 && current.bucket1 < capacity_1 {
            let amount = (capacity_1 - current.bucket1).min(current.bucket2);
            next_states.push(State {
                bucket1: current.bucket1 + amount,
                bucket2: current.bucket2 - amount,
            });
        }
        
        for next in next_states {
            if !visited.contains(&next) {
                visited.insert(next);
                parent.insert(next, Some(current));
                queue.push_back(next);
                
                // Check if we've reached the goal
                if next.bucket1 == goal {
                    // Count moves by backtracking
                    let mut moves = 0;
                    let mut state = Some(next);
                    while state.is_some() {
                        moves += 1;
                        state = parent.get(&state.unwrap()).and_then(|&s| s);
                    }
                    
                    return Some(BucketStats {
                        moves,
                        goal_bucket: Bucket::One,
                        other_bucket: next.bucket2,
                    });
                }
                if next.bucket2 == goal {
                    // Count moves by backtracking
                    let mut moves = 0;
                    let mut state = Some(next);
                    while state.is_some() {
                        moves += 1;
                        state = parent.get(&state.unwrap()).and_then(|&s| s);
                    }
                    
                    return Some(BucketStats {
                        moves,
                        goal_bucket: Bucket::Two,
                        other_bucket: next.bucket1,
                    });
                }
            }
        }
    }
    
    None
}