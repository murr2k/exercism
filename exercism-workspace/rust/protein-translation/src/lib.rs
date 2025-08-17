pub fn translate(rna: &str) -> Option<Vec<&'static str>> {
    let mut proteins = Vec::new();
    let chars: Vec<char> = rna.chars().collect();
    
    // Process RNA in chunks of 3 (codons)
    for chunk in chars.chunks(3) {
        if chunk.len() != 3 {
            // If we hit an incomplete codon and haven't found any proteins yet,
            // or no stop codon was encountered, this is invalid
            return None;
        }
        
        let codon: String = chunk.iter().collect();
        
        match translate_codon(&codon) {
            Some("STOP") => break, // Stop translation at stop codon - this is valid
            Some(protein) => proteins.push(protein),
            None => return None, // Invalid codon
        }
    }
    
    Some(proteins)
}

fn translate_codon(codon: &str) -> Option<&'static str> {
    match codon {
        "AUG" => Some("Methionine"),
        "UUU" | "UUC" => Some("Phenylalanine"),
        "UUA" | "UUG" => Some("Leucine"),
        "UCU" | "UCC" | "UCA" | "UCG" => Some("Serine"),
        "UAU" | "UAC" => Some("Tyrosine"),
        "UGU" | "UGC" => Some("Cysteine"),
        "UGG" => Some("Tryptophan"),
        "UAA" | "UAG" | "UGA" => Some("STOP"),
        _ => None,
    }
}

