use std::collections::HashSet;
use std::cell::RefCell;

thread_local! {
    static USED_NAMES: RefCell<HashSet<String>> = RefCell::new(HashSet::new());
}

pub struct Robot {
    name: String,
}

impl Robot {
    pub fn new() -> Self {
        let mut robot = Robot {
            name: String::new(),
        };
        robot.reset_name();
        robot
    }

    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn reset_name(&mut self) {
        USED_NAMES.with(|used_names| {
            let mut used_names = used_names.borrow_mut();
            
            // Remove current name from the set if it exists
            if !self.name.is_empty() {
                used_names.remove(&self.name);
            }
            
            // Generate a new unique name
            loop {
                let new_name = generate_random_name();
                if !used_names.contains(&new_name) {
                    used_names.insert(new_name.clone());
                    self.name = new_name;
                    break;
                }
            }
        });
    }
}

fn generate_random_name() -> String {
    use std::collections::hash_map::DefaultHasher;
    use std::hash::{Hash, Hasher};
    use std::time::{SystemTime, UNIX_EPOCH};
    
    // Create a simple pseudo-random number based on current time and memory address
    let now = SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_nanos();
    let mut hasher = DefaultHasher::new();
    now.hash(&mut hasher);
    
    // Add some additional entropy from heap allocation
    let box_addr = Box::into_raw(Box::new(0)) as usize;
    box_addr.hash(&mut hasher);
    
    // Clean up the memory
    unsafe { 
        let _ = Box::from_raw(box_addr as *mut u8); 
    }
    
    let hash = hasher.finish();
    
    let mut name = String::new();
    
    // Generate two letters
    for i in 0..2 {
        let letter_index = ((hash >> (i * 8)) % 26) as u8;
        name.push((b'A' + letter_index) as char);
    }
    
    // Generate three digits
    for i in 2..5 {
        let digit = ((hash >> (i * 8)) % 10) as u8;
        name.push((b'0' + digit) as char);
    }
    
    name
}