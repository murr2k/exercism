#!/usr/bin/env node

const RustSolutionGenerator = require('./src/RustSolutionGenerator');
const ExercismCLI = require('./src/ExercismCLI');
const chalk = require('chalk');
require('dotenv').config();

async function testRustSetup() {
  console.log(chalk.cyan('🦀 Testing Rust Track Setup'));
  console.log(chalk.gray('─'.repeat(50)));
  
  const token = process.env.EXERCISM_TOKEN;
  
  if (!token) {
    console.error(chalk.red('❌ No EXERCISM_TOKEN found in environment'));
    console.log(chalk.yellow('Please set EXERCISM_TOKEN in .env file'));
    process.exit(1);
  }
  
  console.log(chalk.green('✓ Exercism token found'));
  
  // Test Rust solution generator
  const rustGen = new RustSolutionGenerator();
  console.log(chalk.green('✓ Rust solution generator initialized'));
  
  // Test CLI configuration
  const cli = new ExercismCLI({
    apiToken: token,
    solutionGenerator: rustGen
  });
  
  try {
    console.log(chalk.blue('\n📥 Testing Exercism CLI...'));
    await cli.configure();
    console.log(chalk.green('✓ Exercism CLI configured'));
    
    // Test downloading a Rust exercise
    console.log(chalk.blue('\n📦 Testing exercise download...'));
    const exercisePath = await cli.downloadExercise('rust', 'hello-world');
    console.log(chalk.green(`✓ Downloaded hello-world to: ${exercisePath}`));
    
    // Check if Cargo is available
    console.log(chalk.blue('\n🔧 Checking Rust toolchain...'));
    const { exec } = require('child_process');
    const { promisify } = require('util');
    const execAsync = promisify(exec);
    
    try {
      const { stdout: rustVersion } = await execAsync('rustc --version');
      console.log(chalk.green(`✓ Rust compiler found: ${rustVersion.trim()}`));
      
      const { stdout: cargoVersion } = await execAsync('cargo --version');
      console.log(chalk.green(`✓ Cargo found: ${cargoVersion.trim()}`));
    } catch (error) {
      console.error(chalk.red('❌ Rust toolchain not found'));
      console.log(chalk.yellow('Please install Rust from https://rustup.rs/'));
      process.exit(1);
    }
    
    // Test solution generation
    console.log(chalk.blue('\n🧪 Testing solution generation...'));
    const result = await rustGen.generateSolution('hello-world');
    
    if (result.success) {
      console.log(chalk.green('✓ Solution generated and tests passed!'));
      console.log(chalk.gray('\nTest output:'));
      console.log(chalk.gray(result.testsOutput.substring(0, 200) + '...'));
    } else {
      console.log(chalk.yellow('⚠ Solution generated but tests failed'));
      console.log(chalk.gray(result.message));
    }
    
    console.log(chalk.cyan('\n' + '─'.repeat(50)));
    console.log(chalk.green('🎉 Rust track setup is ready!'));
    console.log(chalk.cyan('\nYou can now run:'));
    console.log(chalk.white('  npm run rust -- -e hello-world'));
    console.log(chalk.gray('  # or'));
    console.log(chalk.white('  node multi-track-solve.js -t rust -c 5'));
    
  } catch (error) {
    console.error(chalk.red('❌ Setup test failed:'), error.message);
    if (process.env.DEBUG) {
      console.error(error.stack);
    }
    process.exit(1);
  }
}

testRustSetup().catch(console.error);