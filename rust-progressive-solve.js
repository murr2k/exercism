#!/usr/bin/env node

const ExercismCLI = require('./src/ExercismCLI');
const RustSolutionGenerator = require('./src/RustSolutionGenerator');
const { program } = require('commander');
const chalk = require('chalk');
const fs = require('fs').promises;
const path = require('path');
const { exec } = require('child_process');
const { promisify } = require('util');
require('dotenv').config();

const execAsync = promisify(exec);

// Known Rust exercise sequence based on Exercism's progression
const RUST_EXERCISE_SEQUENCE = [
  'hello-world',
  'reverse-string',
  'gigasecond',
  'armstrong-numbers',
  'raindrops',
  'nth-prime',
  'leap',
  'grains',
  'prime-factors',
  'collatz-conjecture',
  'difference-of-squares',
  'sum-of-multiples',
  'hamming',
  'space-age',
  'nucleotide-count',
  'rna-transcription',
  'protein-translation',
  'resistor-color',
  'beer-song',
  'proverb',
  'two-fer',
  'isogram',
  'scrabble-score',
  'luhn',
  'pangram',
  'bob',
  'acronym',
  'high-scores',
  'matching-brackets',
  'word-count',
  'sublist',
  'etl',
  'series',
  'run-length-encoding',
  'simple-cipher',
  'rotational-cipher',
  'atbash-cipher',
  'affine-cipher',
  'rail-fence-cipher',
  'crypto-square',
  'diffie-hellman',
  'pig-latin',
  'all-your-base',
  'largest-series-product',
  'spiral-matrix',
  'matrix',
  'saddle-points',
  'queen-attack',
  'kindergarten-garden',
  'grade-school',
  'tournament',
  'binary-search',
  'binary-search-tree',
  'ledger',
  'poker',
  'variable-length-quantity',
  'wordy',
  'alphametics',
  'yacht',
  'zebra-puzzle',
  'go-counting',
  'minesweeper',
  'connect',
  'dominoes',
  'bowling',
  'say',
  'phone-number',
  'ocr-numbers',
  'allergies',
  'anagram',
  'roman-numerals',
  'bracket-push',
  'triangle',
  'secret-handshake',
  'perfect-numbers',
  'custom-set',
  'simple-linked-list',
  'list-ops',
  'sieve',
  'paasio',
  'clock',
  'parallel-letter-frequency',
  'rectangles',
  'meetup',
  'isbn-verifier',
  'robot-simulator',
  'robot-name',
  'house',
  'two-bucket',
  'accumulate',
  'macros',
  'forth',
  'circular-buffer',
  'grep',
  'react',
  'decimal'
];

class RustProgressiveSolver extends ExercismCLI {
  constructor(options = {}) {
    super(options);
    this.solutionGenerator = new RustSolutionGenerator();
    this.solvedCount = 0;
    this.failedExercises = [];
  }

  async runTests(exercisePath) {
    const exerciseDir = path.dirname(exercisePath);
    
    try {
      const { stdout, stderr } = await execAsync('cargo test', { cwd: exerciseDir });
      return this.parseRustTestOutput(stdout + stderr);
    } catch (error) {
      return this.parseRustTestOutput(error.stdout + error.stderr);
    }
  }

  parseRustTestOutput(output) {
    const lines = output.split('\n');
    let passed = 0;
    let failed = 0;
    const tests = [];

    for (const line of lines) {
      if (line.includes('test result: ok')) {
        const match = line.match(/(\d+) passed; (\d+) failed/);
        if (match) {
          passed = parseInt(match[1]);
          failed = parseInt(match[2]);
        }
      } else if (line.includes('... ok')) {
        tests.push({ name: line.trim(), passed: true });
      } else if (line.includes('... FAILED')) {
        tests.push({ name: line.trim(), passed: false, error: line });
      }
    }

    return {
      allPassed: failed === 0 && passed > 0,
      passedCount: passed,
      totalCount: passed + failed,
      tests,
      output
    };
  }

  async tryDownloadExercise(exercise) {
    try {
      const exercisePath = await this.downloadExercise('rust', exercise);
      return { success: true, path: exercisePath };
    } catch (error) {
      if (error.message.includes('not unlocked')) {
        return { success: false, reason: 'locked' };
      } else if (error.message.includes('not found')) {
        return { success: false, reason: 'not_found' };
      } else if (error.message.includes('429')) {
        return { success: false, reason: 'rate_limit' };
      }
      return { success: false, reason: 'other', error: error.message };
    }
  }

  async solveExercise(track, exercise, maxAttempts = 3) {
    console.log(`Attempting to download ${exercise}...`);
    
    const downloadResult = await this.tryDownloadExercise(exercise);
    
    if (!downloadResult.success) {
      if (downloadResult.reason === 'rate_limit') {
        console.log(chalk.yellow(`Rate limited, waiting 30 seconds...`));
        await new Promise(resolve => setTimeout(resolve, 30000));
        return await this.solveExercise(track, exercise, maxAttempts);
      } else if (downloadResult.reason === 'locked') {
        console.log(chalk.yellow(`Exercise ${exercise} is locked`));
        return { success: false, reason: 'locked' };
      } else {
        console.log(chalk.red(`Failed to download ${exercise}: ${downloadResult.reason}`));
        return { success: false, reason: downloadResult.reason };
      }
    }

    const exerciseDir = downloadResult.path;
    console.log(`Exercise located at: ${exerciseDir}`);
    
    // Use the RustSolutionGenerator to create solution
    const result = await this.solutionGenerator.generateSolution(exercise);
    
    if (result.success) {
      console.log(chalk.green('All tests passed! Submitting solution...'));
      
      // Find the main source file to submit
      const libPath = path.join(exerciseDir, 'src', 'lib.rs');
      const mainPath = path.join(exerciseDir, 'src', 'main.rs');
      
      let sourcePath;
      try {
        await fs.access(libPath);
        sourcePath = libPath;
      } catch {
        try {
          await fs.access(mainPath);
          sourcePath = mainPath;
        } catch {
          throw new Error(`Cannot find source file for ${exercise}`);
        }
      }
      
      const submission = await this.submitSolution(sourcePath);
      console.log(submission);
      this.solvedCount++;
      return { success: true, attempts: 1 };
    } else {
      console.log(chalk.red(`Failed to solve ${exercise}: ${result.message}`));
      this.failedExercises.push({ exercise, reason: result.message });
      return { success: false, attempts: 1 };
    }
  }

  async solveAllAvailable() {
    console.log(chalk.cyan('Starting progressive Rust exercise solving...'));
    
    for (const exercise of RUST_EXERCISE_SEQUENCE) {
      console.log(chalk.blue(`\n[${this.solvedCount + this.failedExercises.length + 1}] Attempting: ${exercise}`));
      
      try {
        const result = await this.solveExercise('rust', exercise);
        
        if (result.success) {
          console.log(chalk.green(`✓ Successfully solved ${exercise}`));
        } else if (result.reason === 'locked') {
          console.log(chalk.yellow(`⏸ ${exercise} is locked, continuing to find unlocked exercises...`));
        } else {
          console.log(chalk.red(`✗ Failed to solve ${exercise}`));
        }
        
        // Wait between exercises to avoid rate limiting
        if (result.success || result.reason !== 'locked') {
          await new Promise(resolve => setTimeout(resolve, 5000));
        }
        
        // Break if we've hit too many rate limits in a row
        if (this.failedExercises.filter(f => f.reason.includes('429')).length > 5) {
          console.log(chalk.red('Too many rate limits, pausing for 2 minutes...'));
          await new Promise(resolve => setTimeout(resolve, 120000));
          this.failedExercises = this.failedExercises.filter(f => !f.reason.includes('429'));
        }
        
      } catch (error) {
        console.log(chalk.red(`✗ Error with ${exercise}: ${error.message}`));
        this.failedExercises.push({ exercise, reason: error.message });
      }
    }
    
    // Summary
    console.log(chalk.cyan('\n' + '='.repeat(60)));
    console.log(chalk.cyan('FINAL SUMMARY:'));
    console.log(chalk.green(`✓ Successfully solved: ${this.solvedCount} exercises`));
    console.log(chalk.yellow(`⚠ Failed exercises: ${this.failedExercises.length}`));
    
    if (this.failedExercises.length > 0) {
      console.log(chalk.cyan('\nFailed exercises breakdown:'));
      const lockCount = this.failedExercises.filter(f => f.reason === 'locked').length;
      const otherCount = this.failedExercises.length - lockCount;
      console.log(chalk.yellow(`  Locked: ${lockCount}`));
      console.log(chalk.red(`  Other failures: ${otherCount}`));
    }
    
    console.log(chalk.cyan(`\nEstimated progress: ${this.solvedCount}/97 Rust exercises`));
    console.log(chalk.cyan(`Success rate: ${((this.solvedCount / (this.solvedCount + this.failedExercises.length)) * 100).toFixed(1)}%`));
  }
}

async function main() {
  const token = process.env.EXERCISM_TOKEN;
  
  if (!token) {
    console.error(chalk.red('Please set EXERCISM_TOKEN environment variable'));
    process.exit(1);
  }

  const solver = new RustProgressiveSolver({
    apiToken: token,
  });

  try {
    console.log(chalk.blue('Configuring Exercism CLI...'));
    await solver.configure();
    
    await solver.solveAllAvailable();
    
  } catch (error) {
    console.error(chalk.red('Error:'), error.message);
    if (process.env.DEBUG) {
      console.error(error.stack);
    }
  }
}

main().catch(console.error);