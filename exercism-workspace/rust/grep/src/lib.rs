use anyhow::Error;
use std::fs;

/// While using `&[&str]` to handle flags is convenient for exercise purposes,
/// and resembles the output of [`std::env::args`], in real-world projects it is
/// both more convenient and more idiomatic to contain runtime configuration in
/// a dedicated struct. Therefore, we suggest that you do so in this exercise.
///
/// [`std::env::args`]: https://doc.rust-lang.org/std/env/fn.args.html
#[derive(Debug)]
pub struct Flags {
    pub print_line_numbers: bool,  // -n
    pub print_file_names_only: bool,  // -l  
    pub case_insensitive: bool,    // -i
    pub invert_match: bool,        // -v
    pub match_entire_line: bool,   // -x
}

impl Flags {
    pub fn new(flags: &[&str]) -> Self {
        let mut flags_struct = Flags {
            print_line_numbers: false,
            print_file_names_only: false,
            case_insensitive: false,
            invert_match: false,
            match_entire_line: false,
        };
        
        for flag in flags {
            match *flag {
                "-n" => flags_struct.print_line_numbers = true,
                "-l" => flags_struct.print_file_names_only = true,
                "-i" => flags_struct.case_insensitive = true,
                "-v" => flags_struct.invert_match = true,
                "-x" => flags_struct.match_entire_line = true,
                _ => {}  // Unknown flags are ignored
            }
        }
        
        flags_struct
    }
}

pub fn grep(pattern: &str, flags: &Flags, files: &[&str]) -> Result<Vec<String>, Error> {
    let mut results = Vec::new();
    let multiple_files = files.len() > 1;
    
    let search_pattern = if flags.case_insensitive {
        pattern.to_lowercase()
    } else {
        pattern.to_string()
    };
    
    for &filename in files {
        let contents = fs::read_to_string(filename)?;
        let lines: Vec<&str> = contents.lines().collect();
        
        let mut file_has_match = false;
        
        for (line_num, line) in lines.iter().enumerate() {
            let search_line = if flags.case_insensitive {
                line.to_lowercase()
            } else {
                line.to_string()
            };
            
            let is_match = if flags.match_entire_line {
                search_line == search_pattern
            } else {
                search_line.contains(&search_pattern)
            };
            
            let should_include = if flags.invert_match {
                !is_match
            } else {
                is_match
            };
            
            if should_include {
                file_has_match = true;
                
                if flags.print_file_names_only {
                    // For -l flag, we just need to record that this file has a match
                    // We'll add the filename later and break
                    break;
                } else {
                    let mut result_line = String::new();
                    
                    // Add filename if multiple files
                    if multiple_files {
                        result_line.push_str(filename);
                        result_line.push(':');
                    }
                    
                    // Add line number if requested
                    if flags.print_line_numbers {
                        result_line.push_str(&(line_num + 1).to_string());
                        result_line.push(':');
                    }
                    
                    // Add the actual line content
                    result_line.push_str(line);
                    
                    results.push(result_line);
                }
            }
        }
        
        // If -l flag is set and this file had a match, add just the filename
        if flags.print_file_names_only && file_has_match {
            results.push(filename.to_string());
        }
    }
    
    Ok(results)
}