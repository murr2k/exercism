const fs = require('fs').promises;
const path = require('path');
const { exec } = require('child_process');
const util = require('util');
const execPromise = util.promisify(exec);

class RustSolutionGenerator {
  constructor() {
    this.exerciseWorkspace = '/home/murr2k/projects/exercism/exercism-workspace/rust';
  }

  async generateSolution(exerciseSlug) {
    const exercisePath = path.join(this.exerciseWorkspace, exerciseSlug);
    
    try {
      // Read the main lib.rs or main.rs file to understand the structure
      const libPath = path.join(exercisePath, 'src', 'lib.rs');
      const mainPath = path.join(exercisePath, 'src', 'main.rs');
      
      let exerciseFile;
      let isLibrary = true;
      
      try {
        await fs.access(libPath);
        exerciseFile = libPath;
      } catch {
        try {
          await fs.access(mainPath);
          exerciseFile = mainPath;
          isLibrary = false;
        } catch {
          throw new Error(`Cannot find lib.rs or main.rs for ${exerciseSlug}`);
        }
      }
      
      // Read the test file to understand requirements
      const testsPath = path.join(exercisePath, 'tests', `${exerciseSlug.replace(/-/g, '_')}.rs`);
      let testContent = '';
      
      try {
        testContent = await fs.readFile(testsPath, 'utf8');
      } catch {
        // Sometimes tests are in src/lib.rs itself
        try {
          const srcContent = await fs.readFile(exerciseFile, 'utf8');
          if (srcContent.includes('#[test]') || srcContent.includes('#[cfg(test)]')) {
            testContent = srcContent;
          }
        } catch {
          // If no tests found, proceed with known solution if available
          console.log(`Warning: No tests found for ${exerciseSlug}, using known solution`);
          testContent = '';
        }
      }
      
      // Read the current implementation
      const currentImpl = await fs.readFile(exerciseFile, 'utf8');
      
      // Generate solution based on exercise
      const solution = await this.generateSpecificSolution(exerciseSlug, currentImpl, testContent);
      
      // Write the solution
      await fs.writeFile(exerciseFile, solution);
      
      // Run tests to verify (including ignored tests)
      const { stdout, stderr } = await execPromise('cargo test -- --include-ignored', {
        cwd: exercisePath
      });
      
      if (stdout.includes('test result: ok') || stdout.includes('passed')) {
        return {
          success: true,
          message: 'All tests passed',
          testsOutput: stdout
        };
      } else {
        // If tests fail, try to fix based on error
        const fixedSolution = await this.attemptFix(exerciseSlug, solution, stderr);
        if (fixedSolution) {
          await fs.writeFile(exerciseFile, fixedSolution);
          const { stdout: stdout2 } = await execPromise('cargo test -- --include-ignored', {
            cwd: exercisePath
          });
          
          if (stdout2.includes('test result: ok') || stdout2.includes('passed')) {
            return {
              success: true,
              message: 'All tests passed after fix',
              testsOutput: stdout2
            };
          }
        }
        
        return {
          success: false,
          message: 'Tests failed',
          testsOutput: stderr
        };
      }
    } catch (error) {
      console.error(`Error generating solution for ${exerciseSlug}:`, error);
      return {
        success: false,
        message: error.message,
        testsOutput: error.toString()
      };
    }
  }

  async generateSpecificSolution(exerciseSlug, currentImpl, testContent) {
    // Convert slug to proper Rust module name
    const moduleName = exerciseSlug.replace(/-/g, '_');
    
    // Analyze the current implementation to understand the function signatures
    const functionSignatures = this.extractFunctionSignatures(currentImpl);
    
    // Generate solution based on exercise type
    switch(exerciseSlug) {
      case 'hello-world':
        return `pub fn hello() -> &'static str {
    "Hello, World!"
}`;
      
      case 'leap':
        return `pub fn is_leap_year(year: u64) -> bool {
    year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)
}`;
      
      case 'reverse-string':
        return `pub fn reverse(input: &str) -> String {
    input.chars().rev().collect()
}`;
      
      case 'gigasecond':
        return `use time::PrimitiveDateTime as DateTime;
use time::Duration;

pub fn after(start: DateTime) -> DateTime {
    start + Duration::seconds(1_000_000_000)
}`;
      
      case 'bob':
        return `pub fn reply(message: &str) -> &str {
    let message = message.trim();
    
    if message.is_empty() {
        return "Fine. Be that way!";
    }
    
    let is_question = message.ends_with('?');
    let is_yelling = message.chars().any(|c| c.is_alphabetic()) && 
                     message.chars().filter(|c| c.is_alphabetic()).all(|c| c.is_uppercase());
    
    match (is_question, is_yelling) {
        (true, true) => "Calm down, I know what I'm doing!",
        (true, false) => "Sure.",
        (false, true) => "Whoa, chill out!",
        _ => "Whatever."
    }
}`;
      
      case 'raindrops':
        return `pub fn raindrops(n: u32) -> String {
    let mut result = String::new();
    
    if n % 3 == 0 {
        result.push_str("Pling");
    }
    if n % 5 == 0 {
        result.push_str("Plang");
    }
    if n % 7 == 0 {
        result.push_str("Plong");
    }
    
    if result.is_empty() {
        result = n.to_string();
    }
    
    result
}`;
      
      case 'nth-prime':
        return `pub fn nth(n: u32) -> u32 {
    let mut primes = vec![2];
    let mut candidate = 3;
    
    while primes.len() <= n as usize {
        if is_prime(candidate) {
            primes.push(candidate);
        }
        candidate += 2;
    }
    
    primes[n as usize]
}

fn is_prime(n: u32) -> bool {
    if n < 2 {
        return false;
    }
    for i in 2..=((n as f64).sqrt() as u32) {
        if n % i == 0 {
            return false;
        }
    }
    true
}`;
      
      case 'beer-song':
        return `pub fn verse(n: u32) -> String {
    match n {
        0 => "No more bottles of beer on the wall, no more bottles of beer.\\nGo to the store and buy some more, 99 bottles of beer on the wall.\\n".to_string(),
        1 => "1 bottle of beer on the wall, 1 bottle of beer.\\nTake it down and pass it around, no more bottles of beer on the wall.\\n".to_string(),
        2 => "2 bottles of beer on the wall, 2 bottles of beer.\\nTake one down and pass it around, 1 bottle of beer on the wall.\\n".to_string(),
        _ => format!("{} bottles of beer on the wall, {} bottles of beer.\\nTake one down and pass it around, {} bottles of beer on the wall.\\n", n, n, n - 1)
    }
}

pub fn sing(start: u32, end: u32) -> String {
    (end..=start)
        .rev()
        .map(verse)
        .collect::<Vec<_>>()
        .join("\\n")
}`;
      
      case 'proverb':
        return `pub fn build_proverb(list: &[&str]) -> String {
    if list.is_empty() {
        return String::new();
    }
    
    let mut result = Vec::new();
    
    for i in 0..list.len() - 1 {
        result.push(format!("For want of a {} the {} was lost.", list[i], list[i + 1]));
    }
    
    result.push(format!("And all for the want of a {}.", list[0]));
    
    result.join("\\n")
}`;
      
      case 'difference-of-squares':
        return `pub fn square_of_sum(n: u32) -> u32 {
    let sum = n * (n + 1) / 2;
    sum * sum
}

pub fn sum_of_squares(n: u32) -> u32 {
    n * (n + 1) * (2 * n + 1) / 6
}

pub fn difference(n: u32) -> u32 {
    square_of_sum(n) - sum_of_squares(n)
}`;
      
      case 'grains':
        return `pub fn square(s: u32) -> u64 {
    if s < 1 || s > 64 {
        panic!("Square must be between 1 and 64");
    }
    1u64 << (s - 1)
}

pub fn total() -> u64 {
    u64::MAX
}`;
      
      case 'armstrong-numbers':
        return `pub fn is_armstrong_number(num: u32) -> bool {
    let digits: Vec<u32> = num.to_string()
        .chars()
        .map(|c| c.to_digit(10).unwrap())
        .collect();
    
    let len = digits.len() as u32;
    let sum: u32 = digits.iter()
        .map(|&d| d.pow(len))
        .sum();
    
    sum == num
}`;
      
      case 'collatz-conjecture':
        return `pub fn collatz(n: u64) -> Option<u64> {
    if n == 0 {
        return None;
    }
    
    let mut current = n;
    let mut steps = 0;
    
    while current != 1 {
        if current % 2 == 0 {
            current /= 2;
        } else {
            current = current * 3 + 1;
        }
        steps += 1;
    }
    
    Some(steps)
}`;
      
      case 'prime-factors':
        return `pub fn factors(n: u64) -> Vec<u64> {
    let mut result = Vec::new();
    let mut num = n;
    let mut divisor = 2;
    
    while num > 1 {
        while num % divisor == 0 {
            result.push(divisor);
            num /= divisor;
        }
        divisor += 1;
    }
    
    result
}`;
      
      case 'sum-of-multiples':
        return `pub fn sum_of_multiples(limit: u32, factors: &[u32]) -> u32 {
    (1..limit)
        .filter(|&n| factors.iter().any(|&f| f != 0 && n % f == 0))
        .sum()
}`;
      
      case 'hamming':
        return `pub fn hamming_distance(s1: &str, s2: &str) -> Option<usize> {
    if s1.len() != s2.len() {
        return None;
    }
    
    Some(
        s1.chars()
            .zip(s2.chars())
            .filter(|(a, b)| a != b)
            .count()
    )
}`;
      
      case 'space-age':
        return `#[derive(Debug, PartialEq)]
pub struct Duration {
    seconds: u64,
}

impl From<u64> for Duration {
    fn from(s: u64) -> Self {
        Duration { seconds: s }
    }
}

pub trait Planet {
    fn years_during(d: &Duration) -> f64;
}

pub struct Mercury;
pub struct Venus;
pub struct Earth;
pub struct Mars;
pub struct Jupiter;
pub struct Saturn;
pub struct Uranus;
pub struct Neptune;

const EARTH_YEAR_SECONDS: f64 = 31_557_600.0;

impl Planet for Mercury {
    fn years_during(d: &Duration) -> f64 {
        d.seconds as f64 / (EARTH_YEAR_SECONDS * 0.2408467)
    }
}

impl Planet for Venus {
    fn years_during(d: &Duration) -> f64 {
        d.seconds as f64 / (EARTH_YEAR_SECONDS * 0.61519726)
    }
}

impl Planet for Earth {
    fn years_during(d: &Duration) -> f64 {
        d.seconds as f64 / EARTH_YEAR_SECONDS
    }
}

impl Planet for Mars {
    fn years_during(d: &Duration) -> f64 {
        d.seconds as f64 / (EARTH_YEAR_SECONDS * 1.8808158)
    }
}

impl Planet for Jupiter {
    fn years_during(d: &Duration) -> f64 {
        d.seconds as f64 / (EARTH_YEAR_SECONDS * 11.862615)
    }
}

impl Planet for Saturn {
    fn years_during(d: &Duration) -> f64 {
        d.seconds as f64 / (EARTH_YEAR_SECONDS * 29.447498)
    }
}

impl Planet for Uranus {
    fn years_during(d: &Duration) -> f64 {
        d.seconds as f64 / (EARTH_YEAR_SECONDS * 84.016846)
    }
}

impl Planet for Neptune {
    fn years_during(d: &Duration) -> f64 {
        d.seconds as f64 / (EARTH_YEAR_SECONDS * 164.79132)
    }
}`;
      
      case 'nucleotide-count':
        return `use std::collections::HashMap;

pub fn count(nucleotide: char, dna: &str) -> Result<usize, char> {
    if !"ACGT".contains(nucleotide) {
        return Err(nucleotide);
    }
    
    for c in dna.chars() {
        if !"ACGT".contains(c) {
            return Err(c);
        }
    }
    
    Ok(dna.chars().filter(|&c| c == nucleotide).count())
}

pub fn nucleotide_counts(dna: &str) -> Result<HashMap<char, usize>, char> {
    let mut counts = HashMap::new();
    counts.insert('A', 0);
    counts.insert('C', 0);
    counts.insert('G', 0);
    counts.insert('T', 0);
    
    for c in dna.chars() {
        if !"ACGT".contains(c) {
            return Err(c);
        }
        *counts.get_mut(&c).unwrap() += 1;
    }
    
    Ok(counts)
}`;
      
      case 'rna-transcription':
        return `#[derive(Debug, PartialEq)]
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
}`;
      
      case 'isogram':
        return `use std::collections::HashSet;

pub fn check(candidate: &str) -> bool {
    let mut seen = HashSet::new();
    
    for c in candidate.chars() {
        let c = c.to_ascii_lowercase();
        if c.is_alphabetic() {
            if seen.contains(&c) {
                return false;
            }
            seen.insert(c);
        }
    }
    
    true
}`;
      
      case 'scrabble-score':
        return `pub fn score(word: &str) -> u64 {
    word.chars()
        .map(|c| {
            match c.to_ascii_uppercase() {
                'A' | 'E' | 'I' | 'O' | 'U' | 'L' | 'N' | 'R' | 'S' | 'T' => 1,
                'D' | 'G' => 2,
                'B' | 'C' | 'M' | 'P' => 3,
                'F' | 'H' | 'V' | 'W' | 'Y' => 4,
                'K' => 5,
                'J' | 'X' => 8,
                'Q' | 'Z' => 10,
                _ => 0,
            }
        })
        .sum()
}`;
      
      case 'luhn':
        return `pub fn is_valid(code: &str) -> bool {
    let code: String = code.chars().filter(|c| !c.is_whitespace()).collect();
    
    if code.len() <= 1 {
        return false;
    }
    
    if !code.chars().all(|c| c.is_ascii_digit()) {
        return false;
    }
    
    let sum: u32 = code
        .chars()
        .rev()
        .enumerate()
        .map(|(i, c)| {
            let mut digit = c.to_digit(10).unwrap();
            if i % 2 == 1 {
                digit *= 2;
                if digit > 9 {
                    digit -= 9;
                }
            }
            digit
        })
        .sum();
    
    sum % 10 == 0
}`;
      
      case 'pangram':
        return `use std::collections::HashSet;

pub fn is_pangram(sentence: &str) -> bool {
    let letters: HashSet<char> = sentence
        .chars()
        .filter(|c| c.is_ascii_alphabetic())
        .map(|c| c.to_ascii_lowercase())
        .collect();
    
    letters.len() == 26
}`;
      
      case 'acronym':
        return `pub fn abbreviate(phrase: &str) -> String {
    phrase
        .split(|c: char| c.is_whitespace() || c == '-' || c == '_')
        .filter(|word| !word.is_empty())
        .map(|word| {
            // Check if word is all uppercase
            if word.chars().all(|c| c.is_uppercase()) {
                // For all-caps words, just take the first character
                word.chars().next().unwrap().to_string()
            } else {
                // For mixed case, take first char and internal uppercase chars
                let first_char = word.chars().next().unwrap().to_uppercase().collect::<String>();
                let rest: String = word.chars()
                    .skip(1)
                    .filter(|c| c.is_uppercase())
                    .map(|c| c.to_string())
                    .collect();
                first_char + &rest
            }
        })
        .collect()
}`;
      
      case 'two-fer':
        return `pub fn twofer(name: &str) -> String {
    if name.is_empty() {
        "One for you, one for me.".to_string()
    } else {
        format!("One for {}, one for me.", name)
    }
}`;
      
      case 'resistor-color':
        return `use enum_iterator::{all, Sequence};
use int_enum::IntEnum;

#[repr(usize)]
#[derive(Debug, PartialEq, Eq, Clone, Copy, IntEnum, Sequence)]
pub enum ResistorColor {
    Black = 0,
    Brown = 1,
    Red = 2,
    Orange = 3,
    Yellow = 4,
    Green = 5,
    Blue = 6,
    Violet = 7,
    Grey = 8,
    White = 9,
}

pub fn color_to_value(color: ResistorColor) -> usize {
    color.int_value()
}

pub fn value_to_color_string(value: usize) -> String {
    match ResistorColor::from_int(value) {
        Ok(color) => format!("{:?}", color),
        Err(_) => "value out of range".to_string(),
    }
}

pub fn colors() -> Vec<ResistorColor> {
    all::<ResistorColor>().collect()
}`;
      
      case 'matching-brackets':
        return `pub fn brackets_are_balanced(string: &str) -> bool {
    let mut stack = Vec::new();
    
    for ch in string.chars() {
        match ch {
            '(' | '[' | '{' => stack.push(ch),
            ')' => {
                if stack.pop() != Some('(') {
                    return false;
                }
            },
            ']' => {
                if stack.pop() != Some('[') {
                    return false;
                }
            },
            '}' => {
                if stack.pop() != Some('{') {
                    return false;
                }
            },
            _ => {},
        }
    }
    
    stack.is_empty()
}`;
      
      case 'word-count':
        return `use std::collections::HashMap;

pub fn word_count(words: &str) -> HashMap<String, u32> {
    let mut counts = HashMap::new();
    
    let cleaned = words
        .chars()
        .map(|c| if c.is_alphanumeric() || c == '\'' { c } else { ' ' })
        .collect::<String>();
    
    for word in cleaned.split_whitespace() {
        let word = word.trim_matches('\'')
            .to_lowercase();
        if !word.is_empty() {
            *counts.entry(word).or_insert(0) += 1;
        }
    }
    
    counts
}`;
      
      case 'sublist':
        return `#[derive(Debug, PartialEq)]
pub enum Comparison {
    Equal,
    Sublist,
    Superlist,
    Unequal,
}

pub fn sublist<T: PartialEq>(first_list: &[T], second_list: &[T]) -> Comparison {
    if first_list == second_list {
        Comparison::Equal
    } else if is_sublist(first_list, second_list) {
        Comparison::Sublist
    } else if is_sublist(second_list, first_list) {
        Comparison::Superlist
    } else {
        Comparison::Unequal
    }
}

fn is_sublist<T: PartialEq>(shorter: &[T], longer: &[T]) -> bool {
    shorter.is_empty() || 
    longer.windows(shorter.len()).any(|window| window == shorter)
}`;
      
      case 'etl':
        return `use std::collections::BTreeMap;

pub fn transform(h: &BTreeMap<i32, Vec<char>>) -> BTreeMap<char, i32> {
    let mut result = BTreeMap::new();
    
    for (&score, letters) in h {
        for &letter in letters {
            result.insert(letter.to_ascii_lowercase(), score);
        }
    }
    
    result
}`;
      
      case 'series':
        return `pub fn series(digits: &str, len: usize) -> Vec<String> {
    if len == 0 {
        return vec![String::new(); digits.len() + 1];
    }
    
    if len > digits.len() {
        return vec![];
    }
    
    digits
        .chars()
        .collect::<Vec<_>>()
        .windows(len)
        .map(|window| window.iter().collect())
        .collect()
}`;
      
      case 'run-length-encoding':
        return `pub fn encode(source: &str) -> String {
    let mut result = String::new();
    let mut chars = source.chars().peekable();
    
    while let Some(ch) = chars.next() {
        let mut count = 1;
        
        while chars.peek() == Some(&ch) {
            count += 1;
            chars.next();
        }
        
        if count == 1 {
            result.push(ch);
        } else {
            result.push_str(&format!("{}{}", count, ch));
        }
    }
    
    result
}

pub fn decode(source: &str) -> String {
    let mut result = String::new();
    let mut count_str = String::new();
    
    for ch in source.chars() {
        if ch.is_ascii_digit() {
            count_str.push(ch);
        } else {
            let count = if count_str.is_empty() {
                1
            } else {
                count_str.parse().unwrap_or(1)
            };
            
            result.push_str(&ch.to_string().repeat(count));
            count_str.clear();
        }
    }
    
    result
}`;
      
      // Default: try to generate based on function signatures
      default:
        return this.generateGenericSolution(functionSignatures, testContent);
    }
  }

  extractFunctionSignatures(code) {
    const signatures = [];
    const fnRegex = /pub\s+fn\s+(\w+)\s*\((.*?)\)\s*(->.*?)?[\s{]/g;
    let match;
    
    while ((match = fnRegex.exec(code)) !== null) {
      signatures.push({
        name: match[1],
        params: match[2],
        returnType: match[3] || ''
      });
    }
    
    return signatures;
  }

  generateGenericSolution(signatures, testContent) {
    // This is a fallback that tries to create a minimal implementation
    // Real implementation would analyze tests more deeply
    let solution = '';
    
    for (const sig of signatures) {
      const returnType = sig.returnType.replace('->', '').trim();
      
      if (returnType === '') {
        // No return type (void function)
        solution += `pub fn ${sig.name}(${sig.params}) {
    // TODO: Implement
}\n\n`;
      } else if (returnType === 'bool') {
        solution += `pub fn ${sig.name}(${sig.params}) -> ${returnType} {
    false
}\n\n`;
      } else if (returnType.includes('String')) {
        solution += `pub fn ${sig.name}(${sig.params}) -> ${returnType} {
    String::new()
}\n\n`;
      } else if (returnType.includes('Vec')) {
        solution += `pub fn ${sig.name}(${sig.params}) -> ${returnType} {
    vec![]
}\n\n`;
      } else if (returnType.includes('Option')) {
        solution += `pub fn ${sig.name}(${sig.params}) -> ${returnType} {
    None
}\n\n`;
      } else {
        solution += `pub fn ${sig.name}(${sig.params}) -> ${returnType} {
    Default::default()
}\n\n`;
      }
    }
    
    return solution;
  }

  async attemptFix(exerciseSlug, solution, errorOutput) {
    // Try to fix common compilation errors
    if (errorOutput.includes('unresolved import')) {
      // Add missing imports
      const imports = this.extractNeededImports(errorOutput);
      return imports + '\n\n' + solution;
    }
    
    if (errorOutput.includes('type mismatch')) {
      // Try to fix type issues - this would need more sophisticated analysis
      return null;
    }
    
    return null;
  }

  extractNeededImports(errorOutput) {
    let imports = '';
    
    if (errorOutput.includes('DateTime')) {
      imports += 'use time::PrimitiveDateTime as DateTime;\n';
    }
    if (errorOutput.includes('Duration')) {
      imports += 'use time::Duration;\n';
    }
    
    return imports;
  }
}

module.exports = RustSolutionGenerator;