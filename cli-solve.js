#!/usr/bin/env node

const ExercismCLI = require('./src/ExercismCLI');
const CSolutionGenerator = require('./src/CSolutionGenerator');
const { program } = require('commander');
const chalk = require('chalk');
require('dotenv').config();

program
  .version('1.0.0')
  .description('Autonomous Exercism C Track Solver (CLI Version)')
  .option('-e, --exercise <slug>', 'Specific exercise to solve (e.g., hello-world)')
  .option('-a, --all', 'Solve all available exercises')
  .option('--token <token>', 'Exercism API token')
  .option('--workspace <path>', 'Workspace directory')
  .parse(process.argv);

const options = program.opts();

async function main() {
  const token = options.token || process.env.EXERCISM_TOKEN;
  
  if (!token) {
    console.error(chalk.red('Please provide Exercism API token via --token or EXERCISM_TOKEN in .env file'));
    process.exit(1);
  }

  const cli = new ExercismCLI({
    apiToken: token,
    workspace: options.workspace,
    solutionGenerator: new CSolutionGenerator()
  });

  try {
    console.log(chalk.blue('Configuring Exercism CLI...'));
    await cli.configure();
    
    if (options.exercise) {
      // Solve specific exercise
      console.log(chalk.green(`\nSolving exercise: ${options.exercise}`));
      const result = await cli.solveExercise('c', options.exercise);
      
      if (result.success) {
        console.log(chalk.green(`✓ Successfully solved ${options.exercise} in ${result.attempts} attempt(s)`));
      } else {
        console.log(chalk.yellow(`⚠ Could not fully solve ${options.exercise} after ${result.attempts} attempts`));
      }
    } else if (options.all) {
      // List and solve all exercises
      console.log(chalk.blue('\nFetching available C exercises...'));
      const exercises = await cli.listExercises('c');
      
      console.log(chalk.cyan(`Found ${exercises.length} exercises`));
      
      for (const exercise of exercises) {
        console.log(chalk.green(`\nSolving: ${exercise}`));
        
        try {
          const result = await cli.solveExercise('c', exercise);
          
          if (result.success) {
            console.log(chalk.green(`✓ Successfully solved ${exercise}`));
          } else {
            console.log(chalk.yellow(`⚠ Could not fully solve ${exercise}`));
          }
          
          // Wait between exercises
          await new Promise(resolve => setTimeout(resolve, 2000));
        } catch (error) {
          console.log(chalk.red(`✗ Error solving ${exercise}: ${error.message}`));
        }
      }
    } else {
      // Default: solve hello-world
      console.log(chalk.green('\nSolving hello-world exercise...'));
      const result = await cli.solveExercise('c', 'hello-world');
      
      if (result.success) {
        console.log(chalk.green('✓ Successfully solved hello-world!'));
        console.log(chalk.cyan('\nTo solve other exercises, use:'));
        console.log(chalk.gray('  node cli-solve.js --exercise <slug>'));
        console.log(chalk.gray('  node cli-solve.js --all'));
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