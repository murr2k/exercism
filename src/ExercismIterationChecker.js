const { exec } = require('child_process');
const { promisify } = require('util');
const execAsync = promisify(exec);

class ExercismIterationChecker {
  constructor(apiToken) {
    this.apiToken = apiToken;
    this.maxRetries = 10;
    this.retryDelay = 3000; // 3 seconds
  }

  /**
   * Check if the latest iteration passed all tests on Exercism
   * @param {string} track - The language track (e.g., 'rust', 'c')
   * @param {string} exerciseSlug - The exercise slug
   * @returns {Promise<{passed: boolean, status: string, message: string}>}
   */
  async checkIterationStatus(track, exerciseSlug) {
    try {
      // Use exercism CLI to check status
      const { stdout } = await execAsync(
        `exercism status --track=${track} --exercise=${exerciseSlug}`,
        { env: { ...process.env, EXERCISM_TOKEN: this.apiToken } }
      );
      
      // Parse the output to determine status
      if (stdout.includes('Passed all tests') || stdout.includes('Complete')) {
        return {
          passed: true,
          status: 'passed',
          message: 'All tests passed on Exercism'
        };
      } else if (stdout.includes('Failed') || stdout.includes('failed')) {
        return {
          passed: false,
          status: 'failed',
          message: 'Tests failed on Exercism'
        };
      } else if (stdout.includes('In Progress') || stdout.includes('Pending')) {
        return {
          passed: false,
          status: 'pending',
          message: 'Tests are still running on Exercism'
        };
      }
      
      // Default to checking via API if CLI doesn't give clear status
      return await this.checkViaAPI(track, exerciseSlug);
    } catch (error) {
      console.error(`Error checking iteration status: ${error.message}`);
      return {
        passed: false,
        status: 'error',
        message: error.message
      };
    }
  }

  /**
   * Check status via Exercism API
   * @private
   */
  async checkViaAPI(track, exerciseSlug) {
    try {
      const response = await fetch(
        `https://exercism.org/api/v2/tracks/${track}/exercises/${exerciseSlug}`,
        {
          headers: {
            'Authorization': `Bearer ${this.apiToken}`,
            'Content-Type': 'application/json'
          }
        }
      );
      
      if (!response.ok) {
        throw new Error(`API request failed: ${response.status}`);
      }
      
      const data = await response.json();
      const latestIteration = data.track_exercise?.latest_iteration;
      
      if (!latestIteration) {
        return {
          passed: false,
          status: 'no_iteration',
          message: 'No iteration found'
        };
      }
      
      if (latestIteration.status === 'passed' || latestIteration.tests_status === 'passed') {
        return {
          passed: true,
          status: 'passed',
          message: 'All tests passed on Exercism'
        };
      } else if (latestIteration.status === 'failed' || latestIteration.tests_status === 'failed') {
        return {
          passed: false,
          status: 'failed',
          message: `Tests failed: ${latestIteration.tests_failed || 'Unknown failure'}`
        };
      } else {
        return {
          passed: false,
          status: 'pending',
          message: 'Tests are still running'
        };
      }
    } catch (error) {
      // Fallback to simple check
      return {
        passed: false,
        status: 'unknown',
        message: `Could not determine status: ${error.message}`
      };
    }
  }

  /**
   * Wait for iteration to complete and return final status
   * @param {string} track - The language track
   * @param {string} exerciseSlug - The exercise slug
   * @returns {Promise<{passed: boolean, status: string, message: string}>}
   */
  async waitForIterationCompletion(track, exerciseSlug) {
    console.log(`Waiting for Exercism to process submission for ${exerciseSlug}...`);
    
    for (let i = 0; i < this.maxRetries; i++) {
      await new Promise(resolve => setTimeout(resolve, this.retryDelay));
      
      const status = await this.checkIterationStatus(track, exerciseSlug);
      
      if (status.status !== 'pending' && status.status !== 'unknown') {
        return status;
      }
      
      console.log(`Iteration still processing... (attempt ${i + 1}/${this.maxRetries})`);
    }
    
    return {
      passed: false,
      status: 'timeout',
      message: 'Timeout waiting for iteration to complete'
    };
  }

  /**
   * Submit solution and wait for tests to pass
   * @param {string} track - The language track
   * @param {string} exerciseSlug - The exercise slug
   * @param {string} filePath - Path to the solution file
   * @returns {Promise<{success: boolean, message: string}>}
   */
  async submitAndVerify(track, exerciseSlug, filePath) {
    try {
      // Submit the solution
      console.log(`Submitting ${exerciseSlug} to Exercism...`);
      const { stdout: submitOutput } = await execAsync(
        `exercism submit ${filePath}`,
        { 
          cwd: path.dirname(filePath),
          env: { ...process.env, EXERCISM_TOKEN: this.apiToken }
        }
      );
      
      console.log('Solution submitted successfully');
      
      // Wait for tests to complete
      const result = await this.waitForIterationCompletion(track, exerciseSlug);
      
      if (result.passed) {
        console.log(`✅ ${exerciseSlug}: All tests passed on Exercism!`);
        return {
          success: true,
          message: result.message
        };
      } else {
        console.log(`❌ ${exerciseSlug}: ${result.message}`);
        return {
          success: false,
          message: result.message
        };
      }
    } catch (error) {
      console.error(`Error submitting ${exerciseSlug}: ${error.message}`);
      return {
        success: false,
        message: error.message
      };
    }
  }
}

module.exports = ExercismIterationChecker;