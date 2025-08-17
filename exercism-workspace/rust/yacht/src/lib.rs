#[derive(Debug)]
pub enum Category {
    Ones,
    Twos,
    Threes,
    Fours,
    Fives,
    Sixes,
    FullHouse,
    FourOfAKind,
    LittleStraight,
    BigStraight,
    Choice,
    Yacht,
}

type Dice = [u8; 5];

pub fn score(dice: Dice, category: Category) -> u8 {
    use Category::*;
    
    // Count occurrences of each die value
    let mut counts = [0; 7]; // index 0 unused, 1-6 for die values
    for &die in &dice {
        if die >= 1 && die <= 6 {
            counts[die as usize] += 1;
        }
    }
    
    match category {
        Ones => counts[1] * 1,
        Twos => counts[2] * 2,
        Threes => counts[3] * 3,
        Fours => counts[4] * 4,
        Fives => counts[5] * 5,
        Sixes => counts[6] * 6,
        
        FullHouse => {
            // Need exactly 3 of one kind and 2 of another
            let mut has_three = false;
            let mut has_two = false;
            
            for &count in &counts[1..] {
                if count == 3 {
                    has_three = true;
                } else if count == 2 {
                    has_two = true;
                }
            }
            
            if has_three && has_two {
                dice.iter().sum()
            } else {
                0
            }
        }
        
        FourOfAKind => {
            // Need at least 4 of the same kind
            for (value, &count) in counts[1..].iter().enumerate() {
                if count >= 4 {
                    return (value as u8 + 1) * 4;
                }
            }
            0
        }
        
        LittleStraight => {
            // Need 1-2-3-4-5
            if counts[1] >= 1 && counts[2] >= 1 && counts[3] >= 1 && counts[4] >= 1 && counts[5] >= 1 {
                30
            } else {
                0
            }
        }
        
        BigStraight => {
            // Need 2-3-4-5-6
            if counts[2] >= 1 && counts[3] >= 1 && counts[4] >= 1 && counts[5] >= 1 && counts[6] >= 1 {
                30
            } else {
                0
            }
        }
        
        Choice => {
            // Sum of all dice
            dice.iter().sum()
        }
        
        Yacht => {
            // All 5 dice must be the same
            for &count in &counts[1..] {
                if count == 5 {
                    return 50;
                }
            }
            0
        }
    }
}