#!/usr/bin/env node

const fs = require('fs').promises;
const path = require('path');
const { exec } = require('child_process');
const util = require('util');
const execPromise = util.promisify(exec);
const RustSolutionGenerator = require('./src/RustSolutionGenerator');
const ExercismIterationChecker = require('./src/ExercismIterationChecker');
const chalk = require('chalk');
require('dotenv').config();

const EXERCISM_TOKEN = process.env.EXERCISM_TOKEN || 'd3143676-eb0c-49fd-9fbb-8af1f28647a0';
const WORKSPACE = path.join(__dirname, 'exercism-workspace', 'rust');

async function downloadExercise(exerciseSlug) {
  try {
    const exercisePath = path.join(WORKSPACE, exerciseSlug);
    
    // Check if already exists
    try {
      await fs.access(exercisePath);
      console.log(chalk.gray(`Exercise ${exerciseSlug} already downloaded`));
      return exercisePath;
    } catch {
      // Download it
      console.log(chalk.blue(`Downloading ${exerciseSlug}...`));
      await execPromise(`exercism download --track=rust --exercise=${exerciseSlug}`);
      return exercisePath;
    }
  } catch (error) {
    console.error(chalk.red(`Error downloading ${exerciseSlug}: ${error.message}`));
    throw error;
  }
}

async function solveExercise(exerciseSlug, generator, checker) {
  console.log(chalk.cyan(`\n🔧 Solving ${exerciseSlug}...`));
  
  try {
    // Download exercise if needed
    const exercisePath = await downloadExercise(exerciseSlug);
    
    // Generate solution
    console.log(chalk.blue('Generating solution...'));
    const result = await generator.generateSolution(exerciseSlug);
    
    if (!result.success) {
      console.log(chalk.yellow(`Initial solution failed, attempting to fix...`));
      // Try to fix common issues
      const libPath = path.join(exercisePath, 'src', 'lib.rs');
      const currentCode = await fs.readFile(libPath, 'utf8');
      
      // Add common imports if missing
      let fixedCode = currentCode;
      if (exerciseSlug === 'simple-cipher' && !currentCode.includes('use rand')) {
        fixedCode = 'use rand::Rng;\n\n' + fixedCode;
        await fs.writeFile(libPath, fixedCode);
        
        // Re-run tests
        const { stdout } = await execPromise('cargo test', { cwd: exercisePath });
        if (stdout.includes('test result: ok')) {
          result.success = true;
        }
      }
    }
    
    if (result.success) {
      console.log(chalk.green('✓ Local tests passed'));
      
      // Submit solution
      console.log(chalk.blue('Submitting to Exercism...'));
      const libPath = path.join(exercisePath, 'src', 'lib.rs');
      const { stdout } = await execPromise(`exercism submit ${libPath}`, { cwd: exercisePath });
      
      console.log(chalk.green('✓ Solution submitted'));
      
      // Check iteration status
      const iterationResult = await checker.waitForIterationCompletion('rust', exerciseSlug);
      
      if (iterationResult.passed) {
        console.log(chalk.green(`✅ ${exerciseSlug} completed successfully!`));
        return { success: true, exercise: exerciseSlug };
      } else {
        console.log(chalk.yellow(`⚠️ ${exerciseSlug} submitted but tests failed on Exercism`));
        return { success: false, exercise: exerciseSlug, reason: iterationResult.message };
      }
    } else {
      console.log(chalk.red(`❌ ${exerciseSlug} local tests failed`));
      return { success: false, exercise: exerciseSlug, reason: 'Local tests failed' };
    }
  } catch (error) {
    console.error(chalk.red(`Error solving ${exerciseSlug}: ${error.message}`));
    return { success: false, exercise: exerciseSlug, reason: error.message };
  }
}

async function getAvailableExercises() {
  try {
    // Get list of all Rust exercises from Exercism
    const { stdout } = await execPromise('exercism list --track=rust --all');
    
    // Parse the output to get exercise slugs
    const lines = stdout.split('\n');
    const exercises = [];
    
    for (const line of lines) {
      // Look for lines that contain exercise info
      if (line.includes('[') && line.includes(']')) {
        const match = line.match(/\[([^\]]+)\]/);
        if (match) {
          const status = match[1].trim();
          const slugMatch = line.match(/([a-z-]+)$/);
          if (slugMatch) {
            exercises.push({
              slug: slugMatch[1],
              status: status
            });
          }
        }
      }
    }
    
    return exercises;
  } catch (error) {
    console.error(chalk.red(`Error getting exercises: ${error.message}`));
    
    // Fallback to a known list of exercises
    return [
      { slug: 'hello-world', status: 'Available' },
      { slug: 'leap', status: 'Available' },
      { slug: 'reverse-string', status: 'Available' },
      { slug: 'gigasecond', status: 'Available' },
      { slug: 'bob', status: 'Available' },
      { slug: 'raindrops', status: 'Available' },
      { slug: 'nth-prime', status: 'Available' },
      { slug: 'beer-song', status: 'Available' },
      { slug: 'proverb', status: 'Available' },
      { slug: 'difference-of-squares', status: 'Available' },
      { slug: 'grains', status: 'Available' },
      { slug: 'armstrong-numbers', status: 'Available' },
      { slug: 'collatz-conjecture', status: 'Available' },
      { slug: 'prime-factors', status: 'Available' },
      { slug: 'simple-cipher', status: 'Available' }
    ];
  }
}

async function main() {
  console.log(chalk.cyan('🦀 Rust Exercise Solver'));
  console.log(chalk.gray('─'.repeat(50)));
  
  const generator = new RustSolutionGenerator();
  const checker = new ExercismIterationChecker(EXERCISM_TOKEN);
  
  // Get available exercises
  console.log(chalk.blue('Fetching available exercises...'));
  const exercises = await getAvailableExercises();
  
  console.log(chalk.cyan(`Found ${exercises.length} exercises`));
  
  const results = {
    successful: [],
    failed: []
  };
  
  // Solve exercises
  for (let i = 0; i < Math.min(exercises.length, 97); i++) {
    const exercise = exercises[i];
    
    console.log(chalk.cyan(`\n[${i + 1}/97] Processing ${exercise.slug}...`));
    
    const result = await solveExercise(exercise.slug, generator, checker);
    
    if (result.success) {
      results.successful.push(result.exercise);
    } else {
      results.failed.push({ exercise: result.exercise, reason: result.reason });
    }
    
    // Brief pause between exercises
    await new Promise(resolve => setTimeout(resolve, 2000));
    
    // Show progress
    console.log(chalk.gray(`Progress: ${results.successful.length} completed, ${results.failed.length} failed`));
  }
  
  // Final report
  console.log(chalk.cyan('\n' + '='.repeat(50)));
  console.log(chalk.cyan('FINAL REPORT'));
  console.log(chalk.cyan('='.repeat(50)));
  console.log(chalk.green(`✅ Successful: ${results.successful.length}`));
  
  if (results.successful.length > 0) {
    console.log(chalk.gray('Completed exercises:'));
    results.successful.forEach(ex => console.log(chalk.gray(`  • ${ex}`)));
  }
  
  if (results.failed.length > 0) {
    console.log(chalk.yellow(`⚠️ Failed: ${results.failed.length}`));
    console.log(chalk.gray('Failed exercises:'));
    results.failed.forEach(({ exercise, reason }) => 
      console.log(chalk.gray(`  • ${exercise}: ${reason}`))
    );
  }
  
  console.log(chalk.cyan(`\nTotal completion: ${(results.successful.length / 97 * 100).toFixed(1)}%`));
}

// Run the solver
main().catch(error => {
  console.error(chalk.red('Fatal error:'), error);
  process.exit(1);
});