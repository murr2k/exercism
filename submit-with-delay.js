#!/usr/bin/env node

const fs = require('fs').promises;
const path = require('path');
const { exec } = require('child_process');
const util = require('util');
const execPromise = util.promisify(exec);
const chalk = require('chalk');

const WORKSPACE = '/home/murr2k/projects/exercism/exercism-workspace/rust';
const SUBMISSION_DELAY = 12000; // 12 seconds between submissions to avoid rate limiting

async function checkIfSolved(exerciseDir) {
  try {
    const libPath = path.join(exerciseDir, 'src', 'lib.rs');
    const content = await fs.readFile(libPath, 'utf8');
    
    // Check if it has actual implementation (not just TODO)
    if (content.includes('// TODO') || content.includes('todo!()') || content.includes('unimplemented!()')) {
      return false;
    }
    
    // Quick test run to see if solution exists
    const { stdout } = await execPromise('cargo test --lib', { 
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
    
    console.log(chalk.blue(`📤 Submitting ${exerciseName}...`));
    
    // Submit the solution
    const { stdout, stderr } = await execPromise(`exercism submit ${libPath}`, {
      cwd: exerciseDir,
      timeout: 30000
    });
    
    if (stdout.includes('Your solution has been submitted successfully')) {
      console.log(chalk.green(`✅ ${exerciseName} submitted successfully`));
      console.log(chalk.gray(`   View at: ${stdout.match(/https:\/\/[^\s]+/)?.[0] || ''}`));
      
      // Wait 5 seconds before checking iteration
      console.log(chalk.gray('   Waiting 5 seconds for processing...'));
      await new Promise(resolve => setTimeout(resolve, 5000));
      
      return { exercise: exerciseName, status: 'submitted', success: true };
      
    } else if (stderr.includes('No files you submitted have changed')) {
      console.log(chalk.gray(`⏭️  ${exerciseName} already submitted (no changes)`));
      return { exercise: exerciseName, status: 'already_submitted', success: true };
      
    } else if (stderr.includes('429 Too Many Requests')) {
      const waitTime = stderr.match(/after (\d+) seconds/)?.[1] || '10';
      console.log(chalk.yellow(`⚠️  Rate limited. Waiting ${waitTime} seconds...`));
      await new Promise(resolve => setTimeout(resolve, parseInt(waitTime) * 1000 + 1000));
      // Retry once
      return await submitExercise(exerciseName, exerciseDir);
      
    } else {
      console.log(chalk.red(`❌ ${exerciseName} submission failed`));
      return { exercise: exerciseName, status: 'failed', error: stderr, success: false };
    }
  } catch (error) {
    console.log(chalk.red(`❌ Error submitting ${exerciseName}: ${error.message}`));
    return { exercise: exerciseName, status: 'error', error: error.message, success: false };
  }
}

async function main() {
  console.log(chalk.cyan('🦀 Rust Exercise Batch Submission'));
  console.log(chalk.cyan('=================================='));
  console.log(chalk.gray('Rate-limited submission with 12-second delays\n'));
  
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
  
  console.log(chalk.blue(`Found ${exerciseDirs.length} total exercises\n`));
  
  // Check which ones are solved
  const toSubmit = [];
  console.log(chalk.blue('Checking solved exercises...'));
  
  let solvedCount = 0;
  for (const { name, path: exercisePath } of exerciseDirs) {
    const isSolved = await checkIfSolved(exercisePath);
    if (isSolved) {
      solvedCount++;
      process.stdout.write(chalk.green(`✓ ${name} `));
      if (solvedCount % 5 === 0) console.log();
      toSubmit.push({ name, path: exercisePath });
    }
  }
  console.log();
  
  console.log(chalk.cyan(`\n✅ Found ${toSubmit.length} solved exercises\n`));
  console.log(chalk.yellow('Starting submissions with rate limiting...\n'));
  
  // Submit each solved exercise with delays
  const results = {
    submitted: [],
    already_submitted: [],
    failed: []
  };
  
  for (let i = 0; i < toSubmit.length; i++) {
    const { name, path: exercisePath } = toSubmit[i];
    console.log(chalk.cyan(`[${i + 1}/${toSubmit.length}] ${name}`));
    
    const result = await submitExercise(name, exercisePath);
    
    if (result.success) {
      if (result.status === 'submitted') {
        results.submitted.push(result.exercise);
      } else if (result.status === 'already_submitted') {
        results.already_submitted.push(result.exercise);
      }
    } else {
      results.failed.push({ exercise: result.exercise, error: result.error });
    }
    
    // Wait between submissions to avoid rate limiting
    if (i < toSubmit.length - 1) {
      console.log(chalk.gray(`⏰ Waiting ${SUBMISSION_DELAY/1000} seconds before next submission...\n`));
      await new Promise(resolve => setTimeout(resolve, SUBMISSION_DELAY));
    }
  }
  
  // Final report
  console.log(chalk.cyan('\n' + '='.repeat(50)));
  console.log(chalk.cyan('FINAL SUBMISSION REPORT'));
  console.log(chalk.cyan('='.repeat(50)));
  
  if (results.submitted.length > 0) {
    console.log(chalk.green(`\n✅ Newly Submitted: ${results.submitted.length}`));
    results.submitted.forEach(ex => console.log(chalk.gray(`  • ${ex}`)));
  }
  
  if (results.already_submitted.length > 0) {
    console.log(chalk.blue(`\n📋 Already Submitted: ${results.already_submitted.length}`));
    console.log(chalk.gray(`  (${results.already_submitted.slice(0, 5).join(', ')}${results.already_submitted.length > 5 ? '...' : ''})`));
  }
  
  if (results.failed.length > 0) {
    console.log(chalk.red(`\n❌ Failed: ${results.failed.length}`));
    results.failed.forEach(({ exercise, error }) => 
      console.log(chalk.gray(`  • ${exercise}: ${error?.substring(0, 50)}...`))
    );
  }
  
  const totalSuccess = results.submitted.length + results.already_submitted.length;
  console.log(chalk.cyan(`\n📊 Total Success Rate: ${totalSuccess}/${toSubmit.length} (${(totalSuccess/toSubmit.length*100).toFixed(1)}%)`));
  console.log(chalk.green(`🎯 Progress Towards 97: ${totalSuccess}/97 exercises`));
}

main().catch(error => {
  console.error(chalk.red('Fatal error:'), error);
  process.exit(1);
});