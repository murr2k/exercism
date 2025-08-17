#!/usr/bin/env node

const RustSolutionGenerator = require('./src/RustSolutionGenerator');
const chalk = require('chalk');
const fs = require('fs').promises;
const path = require('path');
const { exec } = require('child_process');
const { promisify } = require('util');
require('dotenv').config();

const execAsync = promisify(exec);

async function solveOne(exerciseName) {
  console.log(chalk.blue(`Solving ${exerciseName}...`));
  
  const generator = new RustSolutionGenerator();
  const workspaceDir = '/home/murr2k/projects/exercism/exercism-workspace/rust';
  const exercisePath = path.join(workspaceDir, exerciseName);
  
  // Check if exercise exists
  try {
    await fs.access(exercisePath);
  } catch {
    console.log(chalk.red(`Exercise ${exerciseName} not found in workspace`));
    return;
  }
  
  try {
    // Generate solution
    const result = await generator.generateSolution(exerciseName);
    
    if (result.success) {
      console.log(chalk.green(`✓ All tests passed for ${exerciseName}`));
      
      // Submit the solution
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
          return;
        }
      }
      
      try {
        const { stdout } = await execAsync(`exercism submit ${sourcePath}`, {
          timeout: 10000
        });
        console.log(chalk.gray(stdout.split('\\n')[0])); // Show submission URL
        console.log(chalk.green(`✓ ${exerciseName} submitted successfully`));
      } catch (submitError) {
        console.log(chalk.yellow(`Warning: Could not submit ${exerciseName}: ${submitError.message}`));
      }
    } else {
      console.log(chalk.red(`✗ ${exerciseName} failed: ${result.message}`));
      
      // Show first few lines of error
      if (result.testsOutput) {
        const lines = result.testsOutput.split('\\n').slice(0, 10);
        console.log(chalk.gray(lines.join('\\n')));
      }
    }
  } catch (error) {
    console.log(chalk.red(`✗ Error with ${exerciseName}: ${error.message}`));
  }
}

// Get exercise name from command line
const exerciseName = process.argv[2];
if (!exerciseName) {
  console.log(chalk.red('Please provide an exercise name'));
  console.log(chalk.gray('Usage: node solve-one-rust.js <exercise-name>'));
  process.exit(1);
}

solveOne(exerciseName).catch(console.error);