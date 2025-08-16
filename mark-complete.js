#!/usr/bin/env node

const { chromium } = require('playwright');
const chalk = require('chalk');

async function markExerciseComplete(exerciseSlug) {
  const browser = await chromium.launch({ 
    headless: false,
    args: ['--disable-blink-features=AutomationControlled']
  });
  
  const context = await browser.newContext({
    viewport: { width: 1280, height: 720 },
    userAgent: 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36'
  });
  
  const page = await context.newPage();
  
  try {
    console.log(chalk.blue('Please log in to Exercism if not already logged in...'));
    
    // Navigate directly to the exercise
    const exerciseUrl = `https://exercism.org/tracks/c/exercises/${exerciseSlug}`;
    console.log(chalk.blue(`Navigating to ${exerciseUrl}`));
    await page.goto(exerciseUrl);
    
    // Wait for page to load
    await page.waitForLoadState('networkidle');
    
    // Give user time to log in if needed
    const needsLogin = page.url().includes('/users/sign_in');
    if (needsLogin) {
      console.log(chalk.yellow('Please log in manually in the browser window...'));
      console.log(chalk.yellow('Waiting for login...'));
      
      // Wait for navigation away from login page
      await page.waitForURL(/tracks\/c\/exercises/, { timeout: 120000 });
      console.log(chalk.green('Login successful!'));
    }
    
    // Look for completion indicators
    await page.waitForTimeout(2000);
    
    // Check if exercise is already complete
    const isComplete = await page.$('.completed-badge, .--completed, [data-testid="completed-badge"]');
    if (isComplete) {
      console.log(chalk.green(`✓ ${exerciseSlug} is already marked as complete!`));
      
      // Check for next exercise
      const nextLink = await page.$('a:has-text("Next exercise"), a:has-text("Continue")');
      if (nextLink) {
        const nextUrl = await nextLink.getAttribute('href');
        const nextSlug = nextUrl?.split('/').pop();
        console.log(chalk.cyan(`Next exercise available: ${nextSlug}`));
        return { completed: true, next: nextSlug };
      }
      
      return { completed: true };
    }
    
    // Look for "Mark as complete" button
    console.log(chalk.blue('Looking for completion button...'));
    
    // Try various selectors for the complete button
    const selectors = [
      'button:has-text("Mark as complete")',
      'button:has-text("Complete exercise")',
      'button:has-text("Complete")',
      '[data-testid="complete-exercise-button"]',
      '.complete-exercise-btn'
    ];
    
    let completeButton = null;
    for (const selector of selectors) {
      completeButton = await page.$(selector);
      if (completeButton) break;
    }
    
    if (completeButton) {
      console.log(chalk.green('Found completion button, clicking...'));
      await completeButton.click();
      
      // Wait for completion
      await page.waitForTimeout(3000);
      
      // Handle any modal or confirmation
      const confirmButton = await page.$('button:has-text("Yes"), button:has-text("Confirm"), button:has-text("Continue")');
      if (confirmButton) {
        await confirmButton.click();
        await page.waitForTimeout(2000);
      }
      
      console.log(chalk.green(`✓ Successfully marked ${exerciseSlug} as complete!`));
      
      // Check for unlocked exercises
      const unlockedModal = await page.$('.modal:has-text("unlocked"), .notification:has-text("unlocked")');
      if (unlockedModal) {
        console.log(chalk.cyan('New exercises have been unlocked!'));
      }
      
      return { completed: true };
    } else {
      console.log(chalk.yellow('Could not find completion button.'));
      console.log(chalk.yellow('The exercise may need to be submitted first, or may already be complete.'));
      
      // Check if we need to submit first
      const submitButton = await page.$('button:has-text("Submit")');
      if (submitButton) {
        console.log(chalk.yellow('Found submit button - exercise needs to be submitted first.'));
        return { completed: false, needsSubmission: true };
      }
      
      return { completed: false };
    }
  } catch (error) {
    console.error(chalk.red('Error:'), error.message);
    return { completed: false, error: error.message };
  } finally {
    await browser.close();
  }
}

async function main() {
  const exerciseSlug = process.argv[2] || 'hello-world';
  
  console.log(chalk.cyan(`Marking ${exerciseSlug} as complete on Exercism...`));
  console.log(chalk.gray('This will open a browser window.'));
  console.log();
  
  const result = await markExerciseComplete(exerciseSlug);
  
  if (result.completed) {
    console.log(chalk.green('\n✓ Exercise marked as complete!'));
    if (result.next) {
      console.log(chalk.cyan(`\nYou can now solve: ${result.next}`));
      console.log(chalk.gray(`Run: node cli-solve.js --exercise ${result.next}`));
    }
  } else if (result.needsSubmission) {
    console.log(chalk.yellow('\n⚠ Exercise needs to be submitted first.'));
    console.log(chalk.gray(`Run: node cli-solve.js --exercise ${exerciseSlug}`));
  } else {
    console.log(chalk.red('\n✗ Could not mark exercise as complete.'));
  }
}

if (require.main === module) {
  main().catch(console.error);
}

module.exports = { markExerciseComplete };