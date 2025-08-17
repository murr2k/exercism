/// A munger which XORs a key with some data
#[derive(Clone)]
pub struct Xorcism<'a> {
    key: &'a [u8],
    position: usize,
}

impl<'a> Xorcism<'a> {
    /// Create a new Xorcism munger from a key
    ///
    /// Should accept anything which has a cheap conversion to a byte slice.
    pub fn new<Key>(key: &'a Key) -> Xorcism<'a> 
    where
        Key: AsRef<[u8]> + ?Sized,
    {
        Xorcism {
            key: key.as_ref(),
            position: 0,
        }
    }

    /// XOR each byte of the input buffer with a byte from the key.
    ///
    /// Note that this is stateful: repeated calls are likely to produce different results,
    /// even with identical inputs.
    pub fn munge_in_place(&mut self, data: &mut [u8]) {
        if self.key.is_empty() {
            return;
        }
        
        for byte in data.iter_mut() {
            *byte ^= self.key[self.position % self.key.len()];
            self.position += 1;
        }
    }

    /// XOR each byte of the data with a byte from the key.
    ///
    /// Note that this is stateful: repeated calls are likely to produce different results,
    /// even with identical inputs.
    ///
    /// Should accept anything which has a cheap conversion to a byte iterator.
    /// Shouldn't matter whether the byte iterator's values are owned or borrowed.
    pub fn munge<Data>(&mut self, data: Data) -> impl Iterator<Item = u8> 
    where
        Data: IntoIterator,
        Data::Item: AsByteRef,
    {
        let key = self.key;
        let mut position = self.position;
        
        let result: Vec<u8> = data.into_iter().map(|byte| {
            let byte = byte.as_byte();
            if key.is_empty() {
                byte
            } else {
                let result = byte ^ key[position % key.len()];
                position += 1;
                result
            }
        }).collect();
        
        self.position = position;
        result.into_iter()
    }
}

pub trait AsByteRef {
    fn as_byte(self) -> u8;
}

impl AsByteRef for u8 {
    fn as_byte(self) -> u8 {
        self
    }
}

impl AsByteRef for &u8 {
    fn as_byte(self) -> u8 {
        *self
    }
}