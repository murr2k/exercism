#[derive(Debug, PartialEq, Eq)]
pub enum Error {
    NotEnoughPinsLeft,
    GameComplete,
}

pub struct BowlingGame {
    rolls: Vec<u16>,
}

impl BowlingGame {
    pub fn new() -> Self {
        BowlingGame { rolls: Vec::new() }
    }

    pub fn roll(&mut self, pins: u16) -> Result<(), Error> {
        if pins > 10 {
            return Err(Error::NotEnoughPinsLeft);
        }

        if self.is_game_complete() {
            return Err(Error::GameComplete);
        }

        if !self.is_valid_roll(pins) {
            return Err(Error::NotEnoughPinsLeft);
        }

        self.rolls.push(pins);
        Ok(())
    }

    fn is_valid_roll(&self, pins: u16) -> bool {
        let mut frame = 0;
        let mut roll_idx = 0;

        // Navigate to current position
        while frame < 9 && roll_idx < self.rolls.len() {
            if self.rolls[roll_idx] == 10 {
                frame += 1;
                roll_idx += 1;
            } else if roll_idx + 1 < self.rolls.len() {
                frame += 1;
                roll_idx += 2;
            } else {
                // Mid-frame, check if pins are valid
                return pins + self.rolls[roll_idx] <= 10;
            }
        }

        // In frame 10
        if frame == 9 {
            let frame_10_rolls = self.rolls.len() - roll_idx;
            
            match frame_10_rolls {
                0 => true, // First roll of frame 10
                1 => {
                    // Second roll of frame 10
                    let first = self.rolls[roll_idx];
                    if first == 10 {
                        true // After strike, any roll is valid
                    } else {
                        pins + first <= 10
                    }
                }
                2 => {
                    // Third roll of frame 10 (only if strike or spare)
                    let first = self.rolls[roll_idx];
                    let second = self.rolls[roll_idx + 1];
                    
                    if first == 10 {
                        // First was strike
                        if second == 10 {
                            true // Two strikes, any third roll valid
                        } else {
                            pins + second <= 10
                        }
                    } else if first + second == 10 {
                        // Spare
                        true
                    } else {
                        false // No third roll allowed
                    }
                }
                _ => false,
            }
        } else {
            true // First roll of a new frame
        }
    }

    pub fn score(&self) -> Option<u16> {
        if !self.is_game_complete() {
            return None;
        }

        let mut total = 0;
        let mut roll_idx = 0;

        for _ in 0..10 {
            if roll_idx >= self.rolls.len() {
                break;
            }

            if self.rolls[roll_idx] == 10 {
                // Strike
                total += 10;
                if roll_idx + 1 < self.rolls.len() {
                    total += self.rolls[roll_idx + 1];
                }
                if roll_idx + 2 < self.rolls.len() {
                    total += self.rolls[roll_idx + 2];
                }
                roll_idx += 1;
            } else if roll_idx + 1 < self.rolls.len() {
                let frame_score = self.rolls[roll_idx] + self.rolls[roll_idx + 1];
                total += frame_score;
                
                if frame_score == 10 && roll_idx + 2 < self.rolls.len() {
                    // Spare
                    total += self.rolls[roll_idx + 2];
                }
                roll_idx += 2;
            }
        }

        Some(total)
    }

    fn is_game_complete(&self) -> bool {
        let mut frame = 0;
        let mut roll_idx = 0;

        // Process frames 1-9
        while frame < 9 && roll_idx < self.rolls.len() {
            if self.rolls[roll_idx] == 10 {
                frame += 1;
                roll_idx += 1;
            } else if roll_idx + 1 < self.rolls.len() {
                frame += 1;
                roll_idx += 2;
            } else {
                return false; // Mid-frame
            }
        }

        if frame < 9 {
            return false;
        }

        // Check frame 10
        let remaining_rolls = self.rolls.len() - roll_idx;
        
        if remaining_rolls < 2 {
            return false;
        }

        if remaining_rolls == 2 {
            // Check if we need a third roll
            let first = self.rolls[roll_idx];
            let second = self.rolls[roll_idx + 1];
            first != 10 && first + second != 10
        } else if remaining_rolls == 3 {
            // Three rolls - valid if first was strike or if first two make spare
            let first = self.rolls[roll_idx];
            let second = self.rolls[roll_idx + 1];
            first == 10 || first + second == 10
        } else {
            false
        }
    }
}