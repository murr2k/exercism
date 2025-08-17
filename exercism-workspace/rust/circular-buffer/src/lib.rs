pub struct CircularBuffer<T> {
    buffer: Vec<Option<T>>,
    capacity: usize,
    read_index: usize,
    write_index: usize,
    size: usize,
}

#[derive(Debug, PartialEq, Eq)]
pub enum Error {
    EmptyBuffer,
    FullBuffer,
}

impl<T> CircularBuffer<T> {
    pub fn new(capacity: usize) -> Self {
        let mut buffer = Vec::with_capacity(capacity);
        for _ in 0..capacity {
            buffer.push(None);
        }
        CircularBuffer {
            buffer,
            capacity,
            read_index: 0,
            write_index: 0,
            size: 0,
        }
    }

    pub fn write(&mut self, element: T) -> Result<(), Error> {
        if self.size >= self.capacity {
            return Err(Error::FullBuffer);
        }
        
        self.buffer[self.write_index] = Some(element);
        self.write_index = (self.write_index + 1) % self.capacity;
        self.size += 1;
        Ok(())
    }

    pub fn read(&mut self) -> Result<T, Error> {
        if self.size == 0 {
            return Err(Error::EmptyBuffer);
        }
        
        let element = self.buffer[self.read_index].take().unwrap();
        self.read_index = (self.read_index + 1) % self.capacity;
        self.size -= 1;
        Ok(element)
    }

    pub fn clear(&mut self) {
        for i in 0..self.capacity {
            self.buffer[i] = None;
        }
        self.read_index = 0;
        self.write_index = 0;
        self.size = 0;
    }

    pub fn overwrite(&mut self, element: T) {
        if self.size < self.capacity {
            // Buffer not full, just write normally
            self.write(element).unwrap();
        } else {
            // Buffer full, overwrite the oldest element
            self.buffer[self.write_index] = Some(element);
            self.write_index = (self.write_index + 1) % self.capacity;
            self.read_index = (self.read_index + 1) % self.capacity;
            // Size stays the same since we're overwriting
        }
    }
}