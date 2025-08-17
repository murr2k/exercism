use std::collections::HashMap;

pub fn count(nucleotide: char, dna: &str) -> Result<usize, char> {
    // First check if the nucleotide is valid
    if !matches!(nucleotide, 'A' | 'C' | 'G' | 'T') {
        return Err(nucleotide);
    }
    
    // Check if all characters in dna are valid nucleotides
    for ch in dna.chars() {
        if !matches!(ch, 'A' | 'C' | 'G' | 'T') {
            return Err(ch);
        }
    }
    
    // Count occurrences of the nucleotide
    Ok(dna.chars().filter(|&ch| ch == nucleotide).count())
}

pub fn nucleotide_counts(dna: &str) -> Result<HashMap<char, usize>, char> {
    // Check if all characters in dna are valid nucleotides
    for ch in dna.chars() {
        if !matches!(ch, 'A' | 'C' | 'G' | 'T') {
            return Err(ch);
        }
    }
    
    let mut counts = HashMap::new();
    counts.insert('A', 0);
    counts.insert('C', 0);
    counts.insert('G', 0);
    counts.insert('T', 0);
    
    for ch in dna.chars() {
        *counts.get_mut(&ch).unwrap() += 1;
    }
    
    Ok(counts)
}