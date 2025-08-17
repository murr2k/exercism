use std::collections::HashMap;
use std::sync::{Arc, Mutex};
use std::thread;

pub fn frequency(input: &[&str], worker_count: usize) -> HashMap<char, usize> {
    if input.is_empty() || worker_count == 0 {
        return HashMap::new();
    }

    let result = Arc::new(Mutex::new(HashMap::new()));
    let input_vec: Vec<String> = input.iter().map(|s| s.to_string()).collect();
    let chunk_size = (input_vec.len() + worker_count - 1) / worker_count;
    
    let mut handles = Vec::new();
    
    for chunk in input_vec.chunks(chunk_size) {
        let chunk_owned = chunk.to_vec();
        let result_clone = Arc::clone(&result);
        
        let handle = thread::spawn(move || {
            let mut local_frequencies = HashMap::new();
            
            for text in chunk_owned {
                for ch in text.chars() {
                    if ch.is_alphabetic() {
                        let lowercase = ch.to_lowercase().next().unwrap();
                        *local_frequencies.entry(lowercase).or_insert(0) += 1;
                    }
                }
            }
            
            let mut global_frequencies = result_clone.lock().unwrap();
            for (ch, count) in local_frequencies {
                *global_frequencies.entry(ch).or_insert(0) += count;
            }
        });
        
        handles.push(handle);
    }
    
    for handle in handles {
        handle.join().unwrap();
    }
    
    Arc::try_unwrap(result).unwrap().into_inner().unwrap()
}