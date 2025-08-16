const fs = require('fs').promises;
const path = require('path');
const { exec } = require('child_process');
const util = require('util');
const execPromise = util.promisify(exec);

class RustSolutionGenerator {
  constructor() {
    this.exerciseWorkspace = path.join(process.cwd(), 'exercism-workspace', 'rust');
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
      let testContent;
      
      try {
        testContent = await fs.readFile(testsPath, 'utf8');
      } catch {
        // Sometimes tests are in src/lib.rs itself
        const srcContent = await fs.readFile(exerciseFile, 'utf8');
        if (srcContent.includes('#[test]') || srcContent.includes('#[cfg(test)]')) {
          testContent = srcContent;
        } else {
          throw new Error(`Cannot find tests for ${exerciseSlug}`);
        }
      }
      
      // Read the current implementation
      const currentImpl = await fs.readFile(exerciseFile, 'utf8');
      
      // Generate solution based on exercise
      const solution = await this.generateSpecificSolution(exerciseSlug, currentImpl, testContent);
      
      // Write the solution
      await fs.writeFile(exerciseFile, solution);
      
      // Run tests to verify
      const { stdout, stderr } = await execPromise('cargo test', {
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
          const { stdout: stdout2 } = await execPromise('cargo test', {
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