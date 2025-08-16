#!/usr/bin/env node

const ExercismSolver = require('./src/ExercismSolver');
const CSolutionGenerator = require('./src/CSolutionGenerator');
const RustSolutionGenerator = require('./src/RustSolutionGenerator');
const { program } = require('commander');
const chalk = require('chalk');
require('dotenv').config();

program
  .version('2.0.0')
  .description('Autonomous Multi-Track Exercism Solver')
  .option('-t, --track <track>', 'Programming language track (c, rust)', 'c')
  .option('-e, --exercise <slug>', 'Specific exercise to solve')
  .option('-a, --all', 'Solve all available exercises')
  .option('-c, --count <number>', 'Number of exercises to solve', parseInt)
  .option('--headless', 'Run in headless mode')
  .option('--token <token>', 'Exercism API token')
  .option('--merge-pr', 'Automatically merge PRs after submission', true)
  .parse(process.argv);

const options = program.opts();

// Solution generator factory
function getSolutionGenerator(track) {
  switch(track.toLowerCase()) {
    case 'c':
      return new CSolutionGenerator();
    case 'rust':
      return new RustSolutionGenerator();
    default:
      throw new Error(`Unsupported track: ${track}. Supported tracks: c, rust`);
  }
}

async function main() {
  const token = options.token || process.env.EXERCISM_TOKEN;
  
  if (!token) {
    console.error(chalk.red('Please provide Exercism API token via --token or EXERCISM_TOKEN in .env file'));
    process.exit(1);
  }

  const track = options.track.toLowerCase();
  console.log(chalk.cyan(`🚀 Starting Multi-Track Exercism Solver for ${track.toUpperCase()} track`));

  const solver = new ExercismSolver({
    headless: options.headless || process.env.HEADLESS === 'true',
    solutionGenerator: getSolutionGenerator(track),
    mergePR: options.mergePr
  });

  try {
    console.log(chalk.blue('Initializing browser...'));
    await solver.init();
    
    console.log(chalk.blue('Authenticating with Exercism...'));
    await solver.login(token);
    
    console.log(chalk.blue(`Navigating to ${track.toUpperCase()} track...`));
    await solver.navigateToTrack(track);
    
    if (options.exercise) {
      // Solve specific exercise
      console.log(chalk.green(`\nSolving exercise: ${options.exercise}`));
      const result = await solver.solveExercise(options.exercise);
      
      if (result.success) {
        console.log(chalk.green(`✓ Successfully solved ${options.exercise} in ${result.attempts} attempt(s)`));
      } else {
        console.log(chalk.yellow(`⚠ Could not fully solve ${options.exercise} after ${result.attempts} attempts`));
      }
    } else if (options.all || options.count) {
      // Solve multiple exercises
      console.log(chalk.blue('\nFetching available exercises...'));
      const exercises = await solver.getAvailableExercises();
      let unsolved = exercises.filter(ex => ex.status !== 'completed');
      
      // Sort by difficulty (easy first)
      const difficultyOrder = { 'easy': 1, 'medium': 2, 'hard': 3 };
      unsolved.sort((a, b) => {
        const diffA = difficultyOrder[a.difficulty] || 4;
        const diffB = difficultyOrder[b.difficulty] || 4;
        return diffA - diffB;
      });
      
      // Limit number of exercises if count is specified
      if (options.count) {
        unsolved = unsolved.slice(0, options.count);
        console.log(chalk.cyan(`Solving ${unsolved.length} exercises...`));
      } else {
        console.log(chalk.cyan(`Found ${unsolved.length} unsolved exercises`));
      }
      
      let solved = 0;
      let failed = 0;
      
      for (const exercise of unsolved) {
        console.log(chalk.green(`\n[${solved + failed + 1}/${unsolved.length}] Solving: ${exercise.title} (${exercise.difficulty})`));
        
        try {
          const result = await solver.solveExercise(exercise.slug);
          
          if (result.success) {
            solved++;
            console.log(chalk.green(`✓ Successfully solved ${exercise.title}`));
            
            // Merge PR if enabled
            if (options.mergePr) {
              console.log(chalk.blue('Merging PR...'));
              await solver.mergePullRequest(exercise.slug);
            }
          } else {
            failed++;
            console.log(chalk.yellow(`⚠ Could not fully solve ${exercise.title}`));
          }
          
          // Wait between exercises to avoid rate limiting
          await new Promise(resolve => setTimeout(resolve, 3000));
        } catch (error) {
          failed++;
          console.log(chalk.red(`✗ Error solving ${exercise.title}: ${error.message}`));
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
    } else {
      // Interactive mode - show available exercises
      console.log(chalk.blue('\nFetching available exercises...'));
      const exercises = await solver.getAvailableExercises();
      const unsolved = exercises.filter(ex => ex.status !== 'completed');
      
      console.log(chalk.cyan(`\nAvailable exercises in ${track.toUpperCase()} track:`));
      console.log(chalk.gray('─'.repeat(50)));
      
      const byDifficulty = {
        easy: unsolved.filter(ex => ex.difficulty === 'easy'),
        medium: unsolved.filter(ex => ex.difficulty === 'medium'),
        hard: unsolved.filter(ex => ex.difficulty === 'hard')
      };
      
      for (const [difficulty, exList] of Object.entries(byDifficulty)) {
        if (exList.length > 0) {
          console.log(chalk.yellow(`\n${difficulty.toUpperCase()} (${exList.length}):`));
          exList.slice(0, 5).forEach(ex => {
            console.log(chalk.gray(`  • ${ex.title} (${ex.slug})`));
          });
          if (exList.length > 5) {
            console.log(chalk.gray(`  ... and ${exList.length - 5} more`));
          }
        }
      }
      
      console.log(chalk.cyan('\n' + '─'.repeat(50)));
      console.log(chalk.cyan('Usage:'));
      console.log(chalk.gray('  Solve specific exercise:'));
      console.log(chalk.white('    node multi-track-solve.js -t rust -e hello-world'));
      console.log(chalk.gray('  Solve first 5 exercises:'));
      console.log(chalk.white('    node multi-track-solve.js -t rust -c 5'));
      console.log(chalk.gray('  Solve all exercises:'));
      console.log(chalk.white('    node multi-track-solve.js -t rust -a'));
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