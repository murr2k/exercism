#!/usr/bin/env node

const RustSolutionGenerator = require('./src/RustSolutionGenerator');
const chalk = require('chalk');
const fs = require('fs').promises;
const path = require('path');
const { exec } = require('child_process');
const { promisify } = require('util');
require('dotenv').config();

const execAsync = promisify(exec);

class ExistingRustSolver {
  constructor() {
    this.solutionGenerator = new RustSolutionGenerator();
    this.workspaceDir = '/home/murr2k/projects/exercism/exercism-workspace/rust';
    this.solvedCount = 0;
    this.failedExercises = [];
  }

  async getExerciseDirectories() {
    try {
      const entries = await fs.readdir(this.workspaceDir);
      const directories = [];
      
      for (const entry of entries) {
        const fullPath = path.join(this.workspaceDir, entry);
        const stat = await fs.stat(fullPath);
        if (stat.isDirectory()) {
          // Check if it has a Cargo.toml file
          try {
            await fs.access(path.join(fullPath, 'Cargo.toml'));
            directories.push(entry);
          } catch {
            // Skip directories without Cargo.toml
          }
        }
      }
      
      return directories.sort();
    } catch (error) {
      console.error('Error reading workspace directory:', error);
      return [];
    }
  }

  async checkIfSolved(exerciseName) {
    const exercisePath = path.join(this.workspaceDir, exerciseName);
    
    try {
      // Run tests to see if they pass
      const { stdout, stderr } = await execAsync('cargo test -- --include-ignored', {
        cwd: exercisePath,
        timeout: 30000
      });
      
      const output = stdout + stderr;
      
      // Check for test success patterns
      if (output.includes('test result: ok') && !output.includes('0 passed')) {
        // Also check that there are no failures
        const failureMatch = output.match(/(\\d+) failed/);
        if (!failureMatch || failureMatch[1] === '0') {
          return true;
        }
      }
      
      return false;
    } catch (error) {
      return false;
    }
  }

  async solveExercise(exerciseName) {
    console.log(chalk.blue(`Solving ${exerciseName}...`));
    
    // First check if already solved
    const alreadySolved = await this.checkIfSolved(exerciseName);
    if (alreadySolved) {
      console.log(chalk.green(`✓ ${exerciseName} already solved`));
      this.solvedCount++;
      return { success: true, alreadySolved: true };
    }

    try {
      const result = await this.solutionGenerator.generateSolution(exerciseName);
      
      if (result.success) {
        console.log(chalk.green(`✓ Successfully solved ${exerciseName}`));
        this.solvedCount++;
        
        // Submit the solution
        const exercisePath = path.join(this.workspaceDir, exerciseName);
        const libPath = path.join(exercisePath, 'src', 'lib.rs');
        const mainPath = path.join(exercisePath, 'src', 'main.rs');
        
        let sourcePath;
        try {
          await fs.access(libPath);
          sourcePath = libPath;
        } catch {
          try {
            await fs.access(mainPath);
            sourcePath = mainPath;
          } catch {
            console.log(chalk.yellow(`Warning: Could not find source file for ${exerciseName}`));
            return { success: true, submitted: false };
          }
        }
        
        try {
          const { stdout } = await execAsync(`exercism submit ${sourcePath}`, {
            timeout: 10000
          });
          console.log(chalk.gray(stdout.split('\\n')[0])); // Show submission URL
          return { success: true, submitted: true };
        } catch (submitError) {
          console.log(chalk.yellow(`Warning: Could not submit ${exerciseName}: ${submitError.message}`));
          return { success: true, submitted: false };
        }
      } else {
        console.log(chalk.red(`✗ Failed to solve ${exerciseName}: ${result.message}`));
        this.failedExercises.push({ exercise: exerciseName, reason: result.message });
        return { success: false, reason: result.message };
      }
    } catch (error) {
      console.log(chalk.red(`✗ Error solving ${exerciseName}: ${error.message}`));
      this.failedExercises.push({ exercise: exerciseName, reason: error.message });
      return { success: false, reason: error.message };
    }
  }

  async solveAll() {
    console.log(chalk.cyan('🚀 Solving existing Rust exercises...'));
    
    const exercises = await this.getExerciseDirectories();
    console.log(chalk.cyan(`Found ${exercises.length} exercises to process`));
    
    if (exercises.length === 0) {
      console.log(chalk.red('No exercises found in workspace'));
      return;
    }

    for (let i = 0; i < exercises.length; i++) {
      const exercise = exercises[i];
      console.log(chalk.blue(`\\n[${i + 1}/${exercises.length}] Processing: ${exercise}`));
      
      await this.solveExercise(exercise);
      
      // Small delay to avoid overwhelming the system
      await new Promise(resolve => setTimeout(resolve, 1000));
    }
    
    // Summary
    console.log(chalk.cyan('\\n' + '='.repeat(60)));
    console.log(chalk.cyan('FINAL SUMMARY:'));
    console.log(chalk.green(`✓ Successfully solved: ${this.solvedCount} exercises`));
    console.log(chalk.red(`✗ Failed: ${this.failedExercises.length} exercises`));
    
    if (this.failedExercises.length > 0) {
      console.log(chalk.cyan('\\nFailed exercises:'));
      this.failedExercises.forEach(({ exercise, reason }) => {
        console.log(chalk.red(`  • ${exercise}: ${reason.substring(0, 80)}...`));
      });
    }
    
    const total = this.solvedCount + this.failedExercises.length;
    console.log(chalk.cyan(`\\nSuccess rate: ${((this.solvedCount / total) * 100).toFixed(1)}%`));
    console.log(chalk.cyan(`Total processed: ${total} exercises`));
  }
}

async function main() {
  const solver = new ExistingRustSolver();
  await solver.solveAll();
}

main().catch(console.error);