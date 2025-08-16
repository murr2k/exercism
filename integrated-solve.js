#!/usr/bin/env node

const ExercismSolver = require('./src/ExercismSolver');
const ExercismCLI = require('./src/ExercismCLI');
const CSolutionGenerator = require('./src/CSolutionGenerator');
const { program } = require('commander');
const chalk = require('chalk');
require('dotenv').config();

program
  .version('1.0.0')
  .description('Integrated Exercism Solver - CLI + Web Automation')
  .option('-e, --exercise <slug>', 'Specific exercise to solve')
  .option('-a, --all', 'Solve all available exercises')
  .option('--headless', 'Run browser in headless mode')
  .option('--token <token>', 'Exercism API token')
  .option('--mark-complete', 'Mark exercise as complete via web UI')
  .parse(process.argv);

const options = program.opts();

async function markCompleteViaWeb(exerciseSlug) {
  const webSolver = new ExercismSolver({
    headless: options.headless || false
  });

  try {
    console.log(chalk.blue('Opening browser to mark exercise complete...'));
    await webSolver.init();
    
    console.log(chalk.blue('Logging in via GitHub...'));
    await webSolver.loginWithGitHub();
    
    console.log(chalk.blue(`Marking ${exerciseSlug} as complete...`));
    await webSolver.markExerciseComplete(exerciseSlug);
    
    // Get list of unlocked exercises
    console.log(chalk.blue('Checking for newly unlocked exercises...'));
    const exercises = await webSolver.getUnlockedExercises();
    const available = exercises.filter(ex => ex.available);
    
    console.log(chalk.cyan(`Found ${available.length} available exercises:`));
    available.forEach(ex => {
      console.log(chalk.gray(`  - ${ex.title} (${ex.slug})`));
    });
    
    await webSolver.close();
    return available;
  } catch (error) {
    console.error(chalk.red('Web automation error:'), error.message);
    await webSolver.close();
    return [];
  }
}

async function solveViaCI(track, exercise) {
  const token = options.token || process.env.EXERCISM_TOKEN;
  
  if (!token) {
    console.error(chalk.red('Please provide Exercism API token'));
    return { success: false };
  }

  const cli = new ExercismCLI({
    apiToken: token,
    solutionGenerator: new CSolutionGenerator()
  });

  try {
    await cli.configure();
    const result = await cli.solveExercise(track, exercise);
    return result;
  } catch (error) {
    console.error(chalk.red('CLI solve error:'), error.message);
    return { success: false };
  }
}

async function solveAndComplete(exerciseSlug) {
  console.log(chalk.green(`\n=== Solving ${exerciseSlug} ===`));
  
  // Step 1: Solve via CLI
  const solveResult = await solveViaCI('c', exerciseSlug);
  
  if (!solveResult.success) {
    console.log(chalk.yellow(`Could not solve ${exerciseSlug} automatically`));
    return false;
  }
  
  console.log(chalk.green(`✓ Successfully solved ${exerciseSlug}!`));
  
  // Step 2: Mark complete via web
  if (options.markComplete !== false) {
    console.log(chalk.blue('\nMarking exercise as complete on website...'));
    await markCompleteViaWeb(exerciseSlug);
  }
  
  return true;
}

async function main() {
  if (options.exercise) {
    // Solve specific exercise
    await solveAndComplete(options.exercise);
  } else if (options.all) {
    // Continuously solve exercises
    console.log(chalk.cyan('Starting continuous solve mode...'));
    
    // Start with hello-world if not completed
    let currentExercise = 'hello-world';
    let attempts = 0;
    const maxExercises = 10; // Safety limit
    
    while (attempts < maxExercises) {
      const success = await solveAndComplete(currentExercise);
      
      if (!success) {
        console.log(chalk.yellow(`Stopping at ${currentExercise}`));
        break;
      }
      
      // Get next available exercise
      console.log(chalk.blue('\nChecking for next exercise...'));
      const available = await markCompleteViaWeb(currentExercise);
      
      if (available.length === 0) {
        console.log(chalk.green('No more exercises available!'));
        break;
      }
      
      // Pick the first available exercise
      currentExercise = available[0].slug;
      console.log(chalk.cyan(`Next exercise: ${currentExercise}`));
      
      // Small delay between exercises
      await new Promise(resolve => setTimeout(resolve, 5000));
      attempts++;
    }
  } else {
    // Default: complete hello-world
    console.log(chalk.blue('Completing hello-world exercise...'));
    await markCompleteViaWeb('hello-world');
    
    console.log(chalk.cyan('\nUsage:'));
    console.log(chalk.gray('  node integrated-solve.js --exercise <slug>  # Solve specific exercise'));
    console.log(chalk.gray('  node integrated-solve.js --all              # Solve all exercises'));
  }
}

main().catch(console.error);