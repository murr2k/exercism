#[derive(Debug, PartialEq)]
pub struct Dna {
    strand: String,
}

#[derive(Debug, PartialEq)]
pub struct Rna {
    strand: String,
}

impl Dna {
    pub fn new(dna: &str) -> Result<Dna, usize> {
        for (i, c) in dna.chars().enumerate() {
            if !"ACGT".contains(c) {
                return Err(i);
            }
        }
        Ok(Dna { strand: dna.to_string() })
    }

    pub fn into_rna(self) -> Rna {
        let rna_strand = self.strand
            .chars()
            .map(|c| match c {
                'G' => 'C',
                'C' => 'G',
                'T' => 'A',
                'A' => 'U',
                _ => c,
            })
            .collect();
        Rna { strand: rna_strand }
    }
}

impl Rna {
    pub fn new(rna: &str) -> Result<Rna, usize> {
        for (i, c) in rna.chars().enumerate() {
            if !"ACGU".contains(c) {
                return Err(i);
            }
        }
        Ok(Rna { strand: rna.to_string() })
    }
}