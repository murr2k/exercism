const { exec } = require('child_process');
const { promisify } = require('util');
const fs = require('fs').promises;
const path = require('path');
const CSolutionGenerator = require('./CSolutionGenerator');

const execAsync = promisify(exec);

class ExercismCLI {
  constructor(options = {}) {
    this.apiToken = options.apiToken;
    this.workspace = options.workspace || path.join(process.cwd(), 'exercism-workspace');
    this.solutionGenerator = options.solutionGenerator || new CSolutionGenerator();
    this.configured = false;
  }

  async configure() {
    // Configure Exercism CLI with token
    const exercismCmd = `${process.env.HOME}/.local/bin/exercism`;
    
    try {
      // Check if exercism exists
      await execAsync(`${exercismCmd} version`);
    } catch (error) {
      // Try to install exercism CLI first
      console.log('Installing Exercism CLI...');
      await this.installCLI();
    }
    
    await execAsync(`${exercismCmd} configure --token=${this.apiToken} --workspace=${this.workspace}`);
    this.configured = true;
    console.log('Exercism CLI configured successfully');
  }

  async installCLI() {
    // Download and install Exercism CLI
    const platform = process.platform;
    
    // Get latest release info
    const { stdout: releaseInfo } = await execAsync(
      `curl -s https://api.github.com/repos/exercism/cli/releases/latest`
    );
    const release = JSON.parse(releaseInfo);
    const version = release.tag_name.replace('v', '');
    
    let downloadUrl;
    if (platform === 'linux') {
      downloadUrl = `https://github.com/exercism/cli/releases/download/v${version}/exercism-${version}-linux-x86_64.tar.gz`;
    } else if (platform === 'darwin') {
      downloadUrl = `https://github.com/exercism/cli/releases/download/v${version}/exercism-${version}-darwin-x86_64.tar.gz`;
    } else if (platform === 'win32') {
      downloadUrl = `https://github.com/exercism/cli/releases/download/v${version}/exercism-${version}-windows-x86_64.zip`;
    }

    const tempDir = '/tmp/exercism-install';
    const localBin = path.join(process.env.HOME, '.local', 'bin');
    
    await execAsync(`mkdir -p ${tempDir}`);
    await execAsync(`mkdir -p ${localBin}`);
    await execAsync(`curl -L ${downloadUrl} -o ${tempDir}/exercism.tar.gz`);
    await execAsync(`tar xzf ${tempDir}/exercism.tar.gz -C ${tempDir}`);
    await execAsync(`mv ${tempDir}/exercism ${localBin}/exercism`);
    await execAsync(`chmod +x ${localBin}/exercism`);
    await execAsync(`rm -rf ${tempDir}`);
    
    // Add to PATH for this session
    process.env.PATH = `${localBin}:${process.env.PATH}`;
    console.log(`Exercism CLI installed to ${localBin}/exercism`);
  }

  async downloadExercise(track, exercise) {
    if (!this.configured) await this.configure();
    
    const exercisePath = path.join(this.workspace, track, exercise);
    
    // Check if already exists
    try {
      await fs.access(exercisePath);
      console.log(`Exercise already exists at ${exercisePath}, using existing...`);
      return exercisePath;
    } catch (e) {
      // Doesn't exist, download it
    }
    
    const { stdout } = await execAsync(`${process.env.HOME}/.local/bin/exercism download --track=${track} --exercise=${exercise}`);
    const pathMatch = stdout.match(/Downloaded to: (.+)/);
    return pathMatch ? pathMatch[1].trim() : exercisePath;
  }

  async submitSolution(exercisePath) {
    const { stdout } = await execAsync(`${process.env.HOME}/.local/bin/exercism submit ${exercisePath}`, {
      cwd: path.dirname(exercisePath)
    });
    return stdout;
  }

  async runTests(exercisePath) {
    const exerciseDir = path.dirname(exercisePath);
    
    try {
      // Try to run make test
      const { stdout, stderr } = await execAsync('make test', { cwd: exerciseDir });
      return this.parseTestOutput(stdout + stderr);
    } catch (error) {
      // Parse error output for test failures
      return this.parseTestOutput(error.stdout + error.stderr);
    }
  }

  parseTestOutput(output) {
    const lines = output.split('\n');
    const tests = [];
    let passed = 0;
    let failed = 0;

    for (const line of lines) {
      if (line.includes('PASS')) {
        passed++;
        tests.push({ name: line, passed: true });
      } else if (line.includes('FAIL') || line.includes('error:')) {
        failed++;
        tests.push({ name: line, passed: false, error: line });
      }
    }

    return {
      allPassed: failed === 0 && passed > 0,
      passedCount: passed,
      totalCount: passed + failed,
      tests,
      output
    };
  }

  async solveExercise(track, exercise, maxAttempts = 5) {
    console.log(`Downloading ${exercise} from ${track} track...`);
    const exerciseDir = await this.downloadExercise(track, exercise);
    
    if (!exerciseDir) {
      throw new Error('Failed to download exercise');
    }

    console.log(`Exercise located at: ${exerciseDir}`);
    
    // Read the exercise files
    const files = await fs.readdir(exerciseDir);
    const headerFile = files.find(f => f.endsWith('.h') && !f.includes('test'));
    const testFile = files.find(f => f.includes('test') && f.endsWith('.c'));
    
    const testCode = testFile ? await fs.readFile(path.join(exerciseDir, testFile), 'utf8') : '';
    const headerCode = headerFile ? await fs.readFile(path.join(exerciseDir, headerFile), 'utf8') : '';
    
    const exerciseDetails = {
      instructions: '', // Could parse README.md if needed
      testCode,
      starterCode: headerCode
    };

    // Generate initial solution
    let solution = await this.solutionGenerator.generate(exerciseDetails);
    
    // Find the C source file to write
    const sourceFile = files.find(f => f.endsWith('.c') && !f.includes('test')) || 
                      exercise.replace(/-/g, '_') + '.c';
    const sourcePath = path.join(exerciseDir, sourceFile);
    
    let attempts = 0;
    while (attempts < maxAttempts) {
      console.log(`Attempt ${attempts + 1}/${maxAttempts}`);
      
      // Write solution
      await fs.writeFile(sourcePath, solution);
      
      // Run tests
      const results = await this.runTests(sourcePath);
      console.log(`Tests passed: ${results.passedCount}/${results.totalCount}`);
      
      if (results.allPassed) {
        console.log('All tests passed! Submitting solution...');
        const submission = await this.submitSolution(sourcePath);
        console.log(submission);
        return { success: true, attempts: attempts + 1, solution };
      }
      
      // Improve solution
      solution = await this.solutionGenerator.improve(solution, results, exerciseDetails);
      attempts++;
    }
    
    return { success: false, attempts, solution };
  }

  async listExercises(track) {
    if (!this.configured) await this.configure();
    
    const { stdout } = await execAsync(`${process.env.HOME}/.local/bin/exercism list --track=${track}`);
    const exercises = [];
    const lines = stdout.split('\n');
    
    for (const line of lines) {
      const match = line.match(/(\w+(?:-\w+)*)/);
      if (match && !line.includes('Track:')) {
        exercises.push(match[1]);
      }
    }
    
    return exercises;
  }
}

module.exports = ExercismCLI;