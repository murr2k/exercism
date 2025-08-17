use std::collections::HashMap;

pub type Value = i32;
pub type Result = std::result::Result<(), Error>;

pub struct Forth {
    stack: Vec<Value>,
    definitions: HashMap<String, Vec<String>>,
    definition_snapshots: HashMap<String, HashMap<String, Vec<String>>>,
}

#[derive(Debug, PartialEq, Eq)]
pub enum Error {
    DivisionByZero,
    StackUnderflow,
    UnknownWord,
    InvalidWord,
}

impl Forth {
    pub fn new() -> Forth {
        Forth {
            stack: Vec::new(),
            definitions: HashMap::new(),
            definition_snapshots: HashMap::new(),
        }
    }

    pub fn stack(&self) -> &[Value] {
        &self.stack
    }

    pub fn eval(&mut self, input: &str) -> Result {
        let tokens: Vec<&str> = input.split_whitespace().collect();
        self.process_tokens(&tokens)
    }

    fn process_tokens(&mut self, tokens: &[&str]) -> Result {
        let mut i = 0;
        while i < tokens.len() {
            let token = tokens[i].to_lowercase();
            
            if token == ":" {
                // Handle word definition
                i += 1;
                if i >= tokens.len() {
                    return Err(Error::InvalidWord);
                }
                
                let word_name = tokens[i].to_lowercase();
                
                // Check if trying to redefine a number
                if word_name.parse::<Value>().is_ok() {
                    return Err(Error::InvalidWord);
                }
                
                i += 1;
                let mut definition = Vec::new();
                
                // Collect definition until we find ";"
                while i < tokens.len() && tokens[i] != ";" {
                    definition.push(tokens[i].to_lowercase());
                    i += 1;
                }
                
                if i >= tokens.len() {
                    return Err(Error::InvalidWord);
                }
                
                // Handle special case where the word references itself
                let mut resolved_definition = Vec::new();
                for token in &definition {
                    if token == &word_name {
                        // Self-reference: use the previous definition
                        if let Some(old_def) = self.definitions.get(&word_name) {
                            resolved_definition.extend(old_def.clone());
                        } else {
                            // No previous definition, keep the token
                            resolved_definition.push(token.clone());
                        }
                    } else {
                        // Keep other tokens as-is for late binding
                        resolved_definition.push(token.clone());
                    }
                }
                
                // Take a snapshot of current definitions for this word
                let snapshot = self.definitions.clone();
                self.definition_snapshots.insert(word_name.clone(), snapshot);
                
                self.definitions.insert(word_name, resolved_definition);
                i += 1; // Skip the ";"
            } else {
                self.process_single_token(&token)?;
                i += 1;
            }
        }
        Ok(())
    }

    fn process_single_token(&mut self, token: &str) -> Result {
        // Check user-defined words first
        if let Some(definition) = self.definitions.get(token).cloned() {
            // Execute the user-defined word using the snapshot from when it was defined
            if let Some(snapshot) = self.definition_snapshots.get(token).cloned() {
                self.execute_with_snapshot(&definition, &snapshot)
            } else {
                // Fallback for words defined before we had snapshots
                for def_token in definition {
                    self.process_single_token(&def_token)?;
                }
                Ok(())
            }
        } else {
            self.execute_builtin_token(token)
        }
    }
    
    fn execute_with_snapshot(&mut self, definition: &[String], snapshot: &HashMap<String, Vec<String>>) -> Result {
        for token in definition {
            if let Some(def) = snapshot.get(token) {
                // Use the definition from the snapshot
                self.execute_with_snapshot(def, snapshot)?;
            } else {
                // Built-in word or number
                self.execute_builtin_token(token)?;
            }
        }
        Ok(())
    }
    
    fn execute_builtin_token(&mut self, token: &str) -> Result {
        match token {
            // Numbers
            _ if token.parse::<Value>().is_ok() => {
                let value = token.parse::<Value>().unwrap();
                self.stack.push(value);
                Ok(())
            },
            // Arithmetic operations
            "+" => self.binary_op(|a, b| a + b),
            "-" => self.binary_op(|a, b| a - b),
            "*" => self.binary_op(|a, b| a * b),
            "/" => {
                if self.stack.len() < 2 {
                    return Err(Error::StackUnderflow);
                }
                let b = self.stack.pop().unwrap();
                let a = self.stack.pop().unwrap();
                if b == 0 {
                    return Err(Error::DivisionByZero);
                }
                self.stack.push(a / b);
                Ok(())
            },
            // Stack operations
            "dup" => {
                if self.stack.is_empty() {
                    return Err(Error::StackUnderflow);
                }
                let top = *self.stack.last().unwrap();
                self.stack.push(top);
                Ok(())
            },
            "drop" => {
                if self.stack.is_empty() {
                    return Err(Error::StackUnderflow);
                }
                self.stack.pop();
                Ok(())
            },
            "swap" => {
                if self.stack.len() < 2 {
                    return Err(Error::StackUnderflow);
                }
                let len = self.stack.len();
                self.stack.swap(len - 1, len - 2);
                Ok(())
            },
            "over" => {
                if self.stack.len() < 2 {
                    return Err(Error::StackUnderflow);
                }
                let len = self.stack.len();
                let value = self.stack[len - 2];
                self.stack.push(value);
                Ok(())
            },
            // Unknown word
            _ => Err(Error::UnknownWord)
        }
    }

    fn binary_op<F>(&mut self, op: F) -> Result
    where
        F: Fn(Value, Value) -> Value,
    {
        if self.stack.len() < 2 {
            return Err(Error::StackUnderflow);
        }
        let b = self.stack.pop().unwrap();
        let a = self.stack.pop().unwrap();
        self.stack.push(op(a, b));
        Ok(())
    }

}