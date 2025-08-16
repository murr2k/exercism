#!/usr/bin/env node

const ExercismSolver = require('./src/ExercismSolver');
const CSolutionGenerator = require('./src/CSolutionGenerator');
const { program } = require('commander');
const chalk = require('chalk');
require('dotenv').config();

program
  .version('1.0.0')
  .description('Autonomous Exercism C Track Solver')
  .option('-e, --exercise <slug>', 'Specific exercise to solve (e.g., hello-world)')
  .option('-a, --all', 'Solve all available exercises')
  .option('--headless', 'Run in headless mode')
  .option('--token <token>', 'Exercism API token')
  .parse(process.argv);

const options = program.opts();

async function main() {
  const token = options.token || process.env.EXERCISM_TOKEN;
  
  if (!token) {
    console.error(chalk.red('Please provide Exercism API token via --token or EXERCISM_TOKEN in .env file'));
    process.exit(1);
  }

  const solver = new ExercismSolver({
    headless: options.headless || process.env.HEADLESS === 'true',
    solutionGenerator: new CSolutionGenerator()
  });

  try {
    console.log(chalk.blue('Initializing browser...'));
    await solver.init();
    
    console.log(chalk.blue('Authenticating with Exercism...'));
    await solver.login(token);
    
    console.log(chalk.blue('Navigating to C track...'));
    await solver.navigateToTrack('c');
    
    if (options.exercise) {
      // Solve specific exercise
      console.log(chalk.green(`\nSolving exercise: ${options.exercise}`));
      const result = await solver.solveExercise(options.exercise);
      
      if (result.success) {
        console.log(chalk.green(`✓ Successfully solved ${options.exercise} in ${result.attempts} attempt(s)`));
      } else {
        console.log(chalk.yellow(`⚠ Could not fully solve ${options.exercise} after ${result.attempts} attempts`));
      }
    } else if (options.all) {
      // Solve all available exercises
      console.log(chalk.blue('\nFetching available exercises...'));
      const exercises = await solver.getAvailableExercises();
      const unsolved = exercises.filter(ex => ex.status !== 'completed');
      
      console.log(chalk.cyan(`Found ${unsolved.length} unsolved exercises`));
      
      for (const exercise of unsolved) {
        console.log(chalk.green(`\nSolving: ${exercise.title} (${exercise.difficulty})`));
        
        try {
          const result = await solver.solveExercise(exercise.slug);
          
          if (result.success) {
            console.log(chalk.green(`✓ Successfully solved ${exercise.title}`));
          } else {
            console.log(chalk.yellow(`⚠ Could not fully solve ${exercise.title}`));
          }
          
          // Wait between exercises to avoid rate limiting
          await new Promise(resolve => setTimeout(resolve, 3000));
        } catch (error) {
          console.log(chalk.red(`✗ Error solving ${exercise.title}: ${error.message}`));
        }
      }
    } else {
      // Default: solve hello-world
      console.log(chalk.green('\nSolving hello-world exercise...'));
      const result = await solver.solveExercise('hello-world');
      
      if (result.success) {
        console.log(chalk.green('✓ Successfully solved hello-world!'));
        console.log(chalk.cyan('\nTo solve other exercises, use:'));
        console.log(chalk.gray('  npm start -- --exercise <slug>'));
        console.log(chalk.gray('  npm start -- --all'));
      }
    }
  } catch (error) {
    console.error(chalk.red('Error:'), error.message);
    if (error.stack && process.env.DEBUG) {
      console.error(error.stack);
    }
  } finally {
    await solver.close();
  }
}

main().catch(console.error);