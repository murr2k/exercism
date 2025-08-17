#!/usr/bin/env node

const fs = require('fs').promises;
const path = require('path');
const { exec } = require('child_process');
const util = require('util');
const execPromise = util.promisify(exec);
const chalk = require('chalk');

const WORKSPACE = '/home/murr2k/projects/exercism/exercism-workspace/rust';

async function checkIfSolved(exerciseDir) {
  try {
    const libPath = path.join(exerciseDir, 'src', 'lib.rs');
    const content = await fs.readFile(libPath, 'utf8');
    
    // Check if it has actual implementation (not just TODO)
    if (content.includes('// TODO') || content.includes('todo!()') || content.includes('unimplemented!()')) {
      return false;
    }
    
    // Run tests to see if they pass
    const { stdout, stderr } = await execPromise('cargo test', { 
      cwd: exerciseDir,
      timeout: 30000 
    });
    
    return stdout.includes('test result: ok') || stdout.includes('passed');
  } catch (error) {
    return false;
  }
}

async function submitExercise(exerciseName, exerciseDir) {
  try {
    const libPath = path.join(exerciseDir, 'src', 'lib.rs');
    
    console.log(chalk.blue(`Submitting ${exerciseName}...`));
    
    // Submit the solution
    const { stdout, stderr } = await execPromise(`exercism submit ${libPath}`, {
      cwd: exerciseDir
    });
    
    if (stdout.includes('Your solution has been submitted successfully')) {
      console.log(chalk.green(`✅ ${exerciseName} submitted successfully`));
      
      // Wait 5 seconds before checking iteration
      console.log(chalk.gray('Waiting 5 seconds for Exercism to process...'));
      await new Promise(resolve => setTimeout(resolve, 5000));
      
      // Check iteration status (simple check via CLI output)
      try {
        const { stdout: statusOut } = await execPromise(`exercism status --track=rust --exercise=${exerciseName}`);
        if (statusOut.includes('Passed') || statusOut.includes('Complete')) {
          console.log(chalk.green(`✅ ${exerciseName} passed all tests on Exercism!`));
          return { exercise: exerciseName, status: 'passed' };
        } else {
          console.log(chalk.yellow(`⚠️ ${exerciseName} submitted but status unclear`));
          return { exercise: exerciseName, status: 'unclear' };
        }
      } catch {
        // Status check failed, but submission was successful
        return { exercise: exerciseName, status: 'submitted' };
      }
    } else if (stderr.includes('No files you submitted have changed')) {
      console.log(chalk.gray(`${exerciseName} already submitted (no changes)`));
      return { exercise: exerciseName, status: 'already_submitted' };
    } else {
      console.log(chalk.red(`❌ ${exerciseName} submission failed: ${stderr}`));
      return { exercise: exerciseName, status: 'failed', error: stderr };
    }
  } catch (error) {
    console.log(chalk.red(`❌ Error submitting ${exerciseName}: ${error.message}`));
    return { exercise: exerciseName, status: 'error', error: error.message };
  }
}

async function main() {
  console.log(chalk.cyan('🦀 Rust Exercise Submission Tool'));
  console.log(chalk.cyan('================================'));
  
  // Get all exercise directories
  const exercises = await fs.readdir(WORKSPACE);
  const exerciseDirs = [];
  
  for (const exercise of exercises) {
    const exercisePath = path.join(WORKSPACE, exercise);
    const stat = await fs.stat(exercisePath);
    if (stat.isDirectory()) {
      exerciseDirs.push({ name: exercise, path: exercisePath });
    }
  }
  
  console.log(chalk.blue(`Found ${exerciseDirs.length} exercises\n`));
  
  // Check which ones are solved but not submitted
  const toSubmit = [];
  console.log(chalk.blue('Checking which exercises are solved...'));
  
  for (const { name, path: exercisePath } of exerciseDirs) {
    process.stdout.write(chalk.gray(`Checking ${name}...`));
    const isSolved = await checkIfSolved(exercisePath);
    if (isSolved) {
      process.stdout.write(chalk.green(' ✓ solved\n'));
      toSubmit.push({ name, path: exercisePath });
    } else {
      process.stdout.write(chalk.gray(' - not solved\n'));
    }
  }
  
  console.log(chalk.cyan(`\nFound ${toSubmit.length} solved exercises to submit\n`));
  
  // Submit each solved exercise
  const results = {
    passed: [],
    submitted: [],
    already_submitted: [],
    failed: [],
    errors: []
  };
  
  for (let i = 0; i < toSubmit.length; i++) {
    const { name, path: exercisePath } = toSubmit[i];
    console.log(chalk.cyan(`\n[${i + 1}/${toSubmit.length}] Processing ${name}`));
    
    const result = await submitExercise(name, exercisePath);
    
    switch (result.status) {
      case 'passed':
        results.passed.push(result.exercise);
        break;
      case 'submitted':
      case 'unclear':
        results.submitted.push(result.exercise);
        break;
      case 'already_submitted':
        results.already_submitted.push(result.exercise);
        break;
      case 'failed':
        results.failed.push(result);
        break;
      case 'error':
        results.errors.push(result);
        break;
    }
  }
  
  // Final report
  console.log(chalk.cyan('\n' + '='.repeat(50)));
  console.log(chalk.cyan('SUBMISSION REPORT'));
  console.log(chalk.cyan('='.repeat(50)));
  
  if (results.passed.length > 0) {
    console.log(chalk.green(`\n✅ Passed (${results.passed.length}):`));
    results.passed.forEach(ex => console.log(chalk.gray(`  • ${ex}`)));
  }
  
  if (results.submitted.length > 0) {
    console.log(chalk.blue(`\n📤 Submitted (${results.submitted.length}):`));
    results.submitted.forEach(ex => console.log(chalk.gray(`  • ${ex}`)));
  }
  
  if (results.already_submitted.length > 0) {
    console.log(chalk.gray(`\n📋 Already Submitted (${results.already_submitted.length}):`));
    results.already_submitted.forEach(ex => console.log(chalk.gray(`  • ${ex}`)));
  }
  
  if (results.failed.length > 0) {
    console.log(chalk.yellow(`\n⚠️ Failed (${results.failed.length}):`));
    results.failed.forEach(({ exercise, error }) => 
      console.log(chalk.gray(`  • ${exercise}: ${error}`))
    );
  }
  
  if (results.errors.length > 0) {
    console.log(chalk.red(`\n❌ Errors (${results.errors.length}):`));
    results.errors.forEach(({ exercise, error }) => 
      console.log(chalk.gray(`  • ${exercise}: ${error}`))
    );
  }
  
  const total = results.passed.length + results.submitted.length + results.already_submitted.length;
  console.log(chalk.cyan(`\nTotal successfully processed: ${total}/${toSubmit.length}`));
}

main().catch(error => {
  console.error(chalk.red('Fatal error:'), error);
  process.exit(1);
});