const { chromium } = require('playwright');
const fs = require('fs').promises;
const path = require('path');

class ExercismSolver {
  constructor(options = {}) {
    this.headless = options.headless ?? true;
    this.browser = null;
    this.context = null;
    this.page = null;
    this.baseUrl = 'https://exercism.org';
    this.credentials = options.credentials || {};
    this.solutionGenerator = options.solutionGenerator || null;
  }

  async init() {
    this.browser = await chromium.launch({ 
      headless: this.headless,
      args: ['--disable-blink-features=AutomationControlled']
    });
    
    this.context = await this.browser.newContext({
      viewport: { width: 1280, height: 720 },
      userAgent: 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36',
    });
    
    this.page = await this.context.newPage();
    
    // Load cookies if they exist
    const cookiesPath = path.join(__dirname, '..', 'cookies.json');
    try {
      const cookies = JSON.parse(await fs.readFile(cookiesPath, 'utf8'));
      await this.context.addCookies(cookies);
      console.log('Loaded saved session');
    } catch (e) {
      console.log('No saved session found');
    }
  }

  async loginWithGitHub() {
    // First check if we're already logged in
    await this.page.goto(`${this.baseUrl}/dashboard`);
    
    // Check if we're on the dashboard (logged in) or redirected to login
    if (!this.page.url().includes('/users/sign_in')) {
      console.log('Already logged in');
      return true;
    }
    
    // Not logged in, proceed with GitHub OAuth
    console.log('Not logged in, attempting GitHub OAuth...');
    
    // Click GitHub login button
    const githubButton = await this.page.$('a[href*="/users/auth/github"]');
    if (!githubButton) {
      console.log('GitHub login button not found');
      console.log('Please log in manually at https://exercism.org/users/sign_in');
      console.log('Then press Enter to continue...');
      
      // Wait for manual login
      await this.page.waitForTimeout(60000);
      return false;
    }
    
    await githubButton.click();
    
    // Handle GitHub OAuth
    try {
      // Wait for GitHub login page
      await this.page.waitForURL(/github\.com/, { timeout: 10000 });
      
      // You'll need to provide GitHub credentials here
      // For automated login, you'd need to handle 2FA if enabled
      console.log('Please complete GitHub login manually...');
      
      // Wait for redirect back to Exercism
      await this.page.waitForURL(/exercism\.org/, { timeout: 60000 });
    } catch (e) {
      // May already be authorized with GitHub
      console.log('GitHub OAuth may already be authorized');
    }
    
    // Save cookies
    const cookies = await this.context.cookies();
    if (cookies.length > 0) {
      await fs.writeFile(
        path.join(__dirname, '..', 'cookies.json'),
        JSON.stringify(cookies, null, 2)
      );
    }
    
    console.log('Login process completed');
    return true;
  }

  async login(apiToken) {
    // First, go to the website
    await this.page.goto(`${this.baseUrl}`);
    
    // Check if already logged in
    try {
      await this.page.waitForSelector('a[href*="/profiles/"]', { timeout: 3000 });
      console.log('Already logged in');
      return true;
    } catch (e) {
      // Not logged in, proceed with token auth
    }
    
    // Use the API token by visiting the CLI login URL
    // This is how the Exercism CLI authenticates
    await this.page.goto(`${this.baseUrl}/cli-walkthrough`);
    
    // Look for the configure token section
    try {
      // Navigate to settings page where we can use the token
      await this.page.goto(`${this.baseUrl}/settings/api`);
      
      // Check if we're redirected to login
      if (this.page.url().includes('/users/sign_in')) {
        // We need to use OAuth or standard login
        // For now, let's try direct navigation with token
        await this.page.evaluate((token) => {
          // Set token in localStorage
          localStorage.setItem('exercism_api_token', token);
        }, apiToken);
        
        // Try accessing the dashboard
        await this.page.goto(`${this.baseUrl}/dashboard`);
      }
    } catch (e) {
      console.log('Token authentication method not available, trying alternative...');
    }
    
    // Alternative: directly navigate to authenticated endpoints
    await this.page.setExtraHTTPHeaders({
      'Authorization': `Bearer ${apiToken}`,
      'X-Api-Token': apiToken
    });
    
    await this.page.goto(`${this.baseUrl}/tracks/c`);
    
    // Save any cookies we have
    const cookies = await this.context.cookies();
    if (cookies.length > 0) {
      await fs.writeFile(
        path.join(__dirname, '..', 'cookies.json'),
        JSON.stringify(cookies, null, 2)
      );
    }
    
    console.log('Proceeding with API token');
    return true;
  }

  async navigateToTrack(language) {
    await this.page.goto(`${this.baseUrl}/tracks/${language}/exercises`);
    await this.page.waitForSelector('.exercise-widget', { timeout: 10000 });
    console.log(`Navigated to ${language} track`);
  }

  async getAvailableExercises() {
    const exercises = await this.page.$$eval('.exercise-widget', widgets => {
      return widgets.map(widget => {
        const link = widget.querySelector('a[href*="/exercises/"]');
        const status = widget.querySelector('.--status')?.textContent?.trim();
        const difficulty = widget.querySelector('.--difficulty')?.textContent?.trim();
        const title = widget.querySelector('.--exercise-title')?.textContent?.trim();
        
        return {
          title,
          url: link?.href,
          status: status || 'available',
          difficulty: difficulty || 'unknown',
          slug: link?.href?.split('/').pop()
        };
      }).filter(ex => ex.url);
    });
    
    return exercises;
  }

  async selectExercise(exerciseSlug) {
    const exerciseUrl = `${this.baseUrl}/tracks/c/exercises/${exerciseSlug}`;
    await this.page.goto(exerciseUrl);
    
    // Check if we need to start the exercise
    const startButton = await this.page.$('button:has-text("Start")');
    if (startButton) {
      await startButton.click();
      await this.page.waitForURL(/editor/, { timeout: 10000 });
    }
    
    console.log(`Selected exercise: ${exerciseSlug}`);
  }

  async getExerciseDetails() {
    // Wait for editor to load
    await this.page.waitForSelector('.c-editor', { timeout: 15000 });
    
    const details = await this.page.evaluate(() => {
      const instructions = document.querySelector('.c-textual-content')?.innerText || '';
      const testCode = document.querySelector('[data-testid="test-file"]')?.textContent || '';
      const starterCode = document.querySelector('.view-lines')?.textContent || '';
      
      return {
        instructions,
        testCode,
        starterCode
      };
    });
    
    return details;
  }

  async submitSolution(code) {
    // Wait for Monaco editor
    await this.page.waitForSelector('.monaco-editor', { timeout: 10000 });
    
    // Clear existing code
    await this.page.click('.monaco-editor');
    await this.page.keyboard.press('Control+A');
    await this.page.keyboard.press('Delete');
    
    // Type new solution
    await this.page.keyboard.type(code);
    
    // Run tests
    await this.page.click('button:has-text("Run Tests")');
    
    // Wait for test results
    await this.page.waitForSelector('.test-summary, .test-results', { timeout: 30000 });
    
    const results = await this.parseTestResults();
    
    if (results.allPassed) {
      // Submit solution
      const submitButton = await this.page.$('button:has-text("Submit")');
      if (submitButton) {
        await submitButton.click();
        console.log('Solution submitted successfully!');
      }
    }
    
    return results;
  }

  async parseTestResults() {
    await this.page.waitForTimeout(2000); // Let results fully load
    
    const results = await this.page.evaluate(() => {
      const summary = document.querySelector('.test-summary');
      const testItems = document.querySelectorAll('.test-run');
      
      const tests = Array.from(testItems).map(item => ({
        name: item.querySelector('.test-name')?.textContent?.trim(),
        passed: item.classList.contains('passed') || item.querySelector('.--passed'),
        error: item.querySelector('.test-output')?.textContent?.trim()
      }));
      
      const allPassed = tests.every(t => t.passed);
      const passedCount = tests.filter(t => t.passed).length;
      
      return {
        allPassed,
        passedCount,
        totalCount: tests.length,
        tests
      };
    });
    
    return results;
  }

  async iterateOnSolution(previousCode, testResults, exerciseDetails) {
    if (!this.solutionGenerator) {
      throw new Error('No solution generator configured');
    }
    
    return await this.solutionGenerator.improve(
      previousCode,
      testResults,
      exerciseDetails
    );
  }

  async markExerciseComplete(exerciseSlug) {
    // Navigate to the exercise page
    const exerciseUrl = `${this.baseUrl}/tracks/c/exercises/${exerciseSlug}`;
    await this.page.goto(exerciseUrl);
    
    // Wait for page to load
    await this.page.waitForLoadState('networkidle');
    
    // Look for "Mark as complete" button or similar
    const completeButton = await this.page.$('button:has-text("Mark as complete"), button:has-text("Complete exercise")');
    
    if (completeButton) {
      await completeButton.click();
      console.log(`Marked ${exerciseSlug} as complete`);
      
      // Wait for confirmation
      await this.page.waitForTimeout(2000);
      
      // Check for unlock modal or next exercise prompt
      const nextButton = await this.page.$('a:has-text("Continue to next exercise"), button:has-text("Continue")');
      if (nextButton) {
        await nextButton.click();
        console.log('Proceeding to next exercise');
      }
      
      return true;
    } else {
      console.log('Exercise may already be complete or button not found');
      return false;
    }
  }

  async getUnlockedExercises() {
    await this.page.goto(`${this.baseUrl}/tracks/c/exercises`);
    await this.page.waitForSelector('.exercise-widget', { timeout: 10000 });
    
    const exercises = await this.page.$$eval('.exercise-widget', widgets => {
      return widgets.map(widget => {
        const link = widget.querySelector('a[href*="/exercises/"]');
        const isLocked = widget.classList.contains('--locked') || 
                        widget.querySelector('.--locked') !== null;
        const isCompleted = widget.classList.contains('--completed') || 
                           widget.querySelector('.--completed') !== null;
        const title = widget.querySelector('.--exercise-title')?.textContent?.trim();
        
        return {
          title,
          url: link?.href,
          slug: link?.href?.split('/').pop(),
          locked: isLocked,
          completed: isCompleted,
          available: !isLocked && !isCompleted
        };
      }).filter(ex => ex.url);
    });
    
    return exercises;
  }

  async solveExercise(exerciseSlug, maxAttempts = 5) {
    await this.selectExercise(exerciseSlug);
    const details = await this.getExerciseDetails();
    
    let solution = await this.solutionGenerator.generate(details);
    let attempts = 0;
    
    while (attempts < maxAttempts) {
      console.log(`Attempt ${attempts + 1}/${maxAttempts}`);
      
      const results = await this.submitSolution(solution);
      console.log(`Tests passed: ${results.passedCount}/${results.totalCount}`);
      
      if (results.allPassed) {
        console.log('All tests passed! Marking as complete...');
        await this.page.waitForTimeout(3000); // Wait for submission to process
        await this.markExerciseComplete(exerciseSlug);
        return { success: true, attempts: attempts + 1, solution };
      }
      
      // Generate improved solution
      solution = await this.iterateOnSolution(solution, results, details);
      attempts++;
    }
    
    return { success: false, attempts, solution };
  }

  async close() {
    if (this.browser) {
      await this.browser.close();
    }
  }
}

module.exports = ExercismSolver;