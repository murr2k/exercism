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

program
  .version('1.0.0')
  .description('Autonomous Exercism Rust Track Solver (CLI Version)')
  .option('-e, --exercise <slug>', 'Specific exercise to solve (e.g., hello-world)')
  .option('-a, --all', 'Solve all available exercises')
  .option('--token <token>', 'Exercism API token')
  .option('--workspace <path>', 'Workspace directory')
  .parse(process.argv);

const options = program.opts();

class RustExercismCLI extends ExercismCLI {
  constructor(options = {}) {
    super(options);
    this.solutionGenerator = new RustSolutionGenerator();
  }

  async runTests(exercisePath) {
    const exerciseDir = path.dirname(exercisePath);
    
    try {
      // Run cargo test for Rust exercises
      const { stdout, stderr } = await execAsync('cargo test', { cwd: exerciseDir });
      return this.parseRustTestOutput(stdout + stderr);
    } catch (error) {
      // Parse error output for test failures
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
        // Extract passed and total from "test result: ok. 5 passed; 0 failed"
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

  async solveExercise(track, exercise, maxAttempts = 3) {
    console.log(`Downloading ${exercise} from ${track} track...`);
    const exerciseDir = await this.downloadExercise(track, exercise);
    
    if (!exerciseDir) {
      throw new Error('Failed to download exercise');
    }

    console.log(`Exercise located at: ${exerciseDir}`);
    
    // Use the RustSolutionGenerator to create solution
    const result = await this.solutionGenerator.generateSolution(exercise);
    
    if (result.success) {
      console.log('All tests passed! Submitting solution...');
      
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
      return { success: true, attempts: 1, solution: result };
    } else {
      console.log(`Failed to solve ${exercise}: ${result.message}`);
      return { success: false, attempts: 1, solution: result };
    }
  }

  async listExercises(track) {
    if (!this.configured) await this.configure();
    
    // Get list of available Rust exercises
    try {
      const { stdout } = await execAsync(`exercism list --track=${track}`);
      const exercises = [];
      const lines = stdout.split('\n');
      
      for (const line of lines) {
        const match = line.match(/(\w+(?:-\w+)*)/);
        if (match && !line.includes('Track:') && !line.includes('exercises')) {
          exercises.push(match[1]);
        }
      }
      
      return exercises;
    } catch (error) {
      console.log('Could not list exercises from CLI, using predefined list...');
      // Fallback to known Rust exercises
      return [
        'hello-world', 'leap', 'reverse-string', 'gigasecond', 'bob', 
        'raindrops', 'nth-prime', 'beer-song', 'proverb', 'difference-of-squares',
        'grains', 'armstrong-numbers', 'collatz-conjecture', 'prime-factors',
        'allergies', 'sublist', 'word-count', 'hamming', 'nucleotide-count',
        'run-length-encoding', 'sieve', 'isogram', 'pangram', 'sum-of-multiples',
        'acronym', 'variable-length-quantity', 'phone-number', 'grade-school',
        'robot-simulator', 'wordy', 'triangle', 'scrabble-score', 'etl',
        'binary-search', 'roman-numerals', 'all-your-base', 'ocr-numbers',
        'luhn', 'pig-latin', 'rotational-cipher', 'perfect-numbers',
        'secret-handshake', 'largest-series-product', 'spiral-matrix',
        'anagram', 'bracket-push', 'two-bucket', 'kindergarten-garden',
        'list-ops', 'custom-set', 'simple-linked-list', 'paasio',
        'clock', 'alphametics', 'protein-translation', 'series',
        'house', 'tournament', 'minesweeper', 'connect', 'forth',
        'circular-buffer', 'binary-search-tree', 'grep', 'robot-name',
        'space-age', 'saddle-points', 'rail-fence-cipher', 'rna-transcription',
        'accumulate', 'parallel-letter-frequency', 'macros', 'decimal',
        'allergies', 'simple-cipher', 'crypto-square', 'rectangles',
        'dominoes', 'bowling', 'say', 'poker', 'ledger', 'react',
        'queen-attack', 'meetup', 'isbn-verifier', 'yacht', 'variable-length-quantity',
        'resistor-color', 'resistor-color-duo', 'resistor-color-trio',
        'two-fer', 'matching-brackets', 'lucians-luscious-lasagna',
        'semi-structured-logs', 'assembly-line', 'enum-iterator',
        'role-playing-game', 'reverse-string', 'low-power-embedded-game',
        'short-fibonacci', 'magazine-cutout', 'health-statistics'
      ];
    }
  }
}

async function main() {
  const token = options.token || process.env.EXERCISM_TOKEN;
  
  if (!token) {
    console.error(chalk.red('Please provide Exercism API token via --token or EXERCISM_TOKEN in .env file'));
    process.exit(1);
  }

  const cli = new RustExercismCLI({
    apiToken: token,
    workspace: options.workspace,
  });

  try {
    console.log(chalk.blue('Configuring Exercism CLI...'));
    await cli.configure();
    
    if (options.exercise) {
      // Solve specific exercise
      console.log(chalk.green(`\nSolving exercise: ${options.exercise}`));
      const result = await cli.solveExercise('rust', options.exercise);
      
      if (result.success) {
        console.log(chalk.green(`✓ Successfully solved ${options.exercise} in ${result.attempts} attempt(s)`));
      } else {
        console.log(chalk.yellow(`⚠ Could not fully solve ${options.exercise} after ${result.attempts} attempts`));
      }
    } else if (options.all) {
      // List and solve all exercises
      console.log(chalk.blue('\nFetching available Rust exercises...'));
      const exercises = await cli.listExercises('rust');
      
      console.log(chalk.cyan(`Found ${exercises.length} exercises`));
      
      let solved = 0;
      let failed = 0;
      
      for (const exercise of exercises) {
        console.log(chalk.green(`\n[${solved + failed + 1}/${exercises.length}] Solving: ${exercise}`));
        
        try {
          const result = await cli.solveExercise('rust', exercise);
          
          if (result.success) {
            solved++;
            console.log(chalk.green(`✓ Successfully solved ${exercise}`));
          } else {
            failed++;
            console.log(chalk.yellow(`⚠ Could not fully solve ${exercise}`));
          }
          
          // Wait between exercises to avoid rate limiting
          await new Promise(resolve => setTimeout(resolve, 2000));
        } catch (error) {
          failed++;
          console.log(chalk.red(`✗ Error solving ${exercise}: ${error.message}`));
        }
      }
      
      // Summary
      console.log(chalk.cyan('\n' + '='.repeat(50)));
      console.log(chalk.cyan('Summary:'));
      console.log(chalk.green(`✓ Solved: ${solved} exercises`));
      if (failed > 0) {
        console.log(chalk.yellow(`⚠ Failed: ${failed} exercises`));
      }
      console.log(chalk.cyan(`Success rate: ${((solved / (solved + failed)) * 100).toFixed(1)}%`));
      console.log(chalk.cyan(`Total Rust exercises completed: ${solved}/97`));
    } else {
      // Default: solve hello-world
      console.log(chalk.green('\nSolving hello-world exercise...'));
      const result = await cli.solveExercise('rust', 'hello-world');
      
      if (result.success) {
        console.log(chalk.green('✓ Successfully solved hello-world!'));
        console.log(chalk.cyan('\nTo solve other exercises, use:'));
        console.log(chalk.gray('  node rust-cli-solve.js --exercise <slug>'));
        console.log(chalk.gray('  node rust-cli-solve.js --all'));
      }
    }
  } catch (error) {
    console.error(chalk.red('Error:'), error.message);
    if (process.env.DEBUG) {
      console.error(error.stack);
    }
  }
}

main().catch(console.error);