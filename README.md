# Exercism C Track Solutions

This repository contains my solutions to the Exercism C programming track, completed using an autonomous Playwright-based solver.

## 📊 Overall Statistics
- **Total Exercises Solved**: 84 C exercises
- **Success Rate**: 100% of attempted exercises passed all tests
- **Completion Date**: August 16, 2025
- **Automation**: Fully autonomous solving using custom Node.js/Playwright solver

## 🎯 Completed Exercises

### Core Concepts (1-5)
1. **hello-world** - Basic C program structure
2. **leap** - Boolean logic and leap year calculation
3. **two-fer** - String formatting and default parameters
4. **resistor-color** - Enums and array indexing
5. **hamming** - String comparison and error counting

### Mathematics & Algorithms (6-20)
6. **grains** - Exponential growth and bit manipulation
7. **difference-of-squares** - Mathematical formulas
8. **armstrong-numbers** - Number decomposition and powers
9. **collatz-conjecture** - Iterative algorithms
10. **square-root** - Newton's method implementation
11. **perfect-numbers** - Divisor calculations
12. **prime-factors** - Prime factorization
13. **nth-prime** - Prime number generation
14. **sieve** - Sieve of Eratosthenes
15. **rational-numbers** - Fraction arithmetic with GCD
16. **complex-numbers** - Complex arithmetic and Euler's formula
17. **pascals-triangle** - Dynamic memory and combinatorics
18. **pythagorean-triplet** - Finding Pythagorean triplets with sum constraint
19. **largest-series-product** - Finding maximum product in consecutive digits
20. **knapsack** - Dynamic programming optimization

### String Processing (21-35)
21. **isogram** - Character frequency tracking
22. **reverse-string** - In-place string reversal
23. **acronym** - String parsing and word extraction
24. **raindrops** - Number to string conversion
25. **pangram** - Alphabet checking
26. **anagram** - String sorting and comparison
27. **roman-numerals** - Number system conversion
28. **run-length-encoding** - Compression algorithm
29. **atbash-cipher** - Character substitution cipher
30. **rotational-cipher** - Caesar cipher implementation
31. **beer-song** - Dynamic text generation
32. **word-count** - Text parsing and counting
33. **crypto-square** - Classic square cipher algorithm
34. **pig-latin** - Text translation with linguistic rules
35. **rail-fence-cipher** - Zigzag transposition cipher

### Data Structures (36-42)
36. **linked-list** - Doubly-linked list implementation
37. **circular-buffer** - Ring buffer with overwrite
38. **list-ops** - Functional list operations
39. **matching-brackets** - Stack-based validation
40. **binary-search** - Efficient searching algorithm
41. **binary-search-tree** - Tree structure with traversal
42. **variable-length-quantity** - VLQ encoding/decoding

### Game Logic & Simulation (43-52)
43. **darts** - Coordinate geometry
44. **space-age** - Planetary calculations
45. **gigasecond** - Time manipulation
46. **allergies** - Bit flags and masking
47. **bob** - Pattern matching and responses
48. **triangle** - Triangle inequality validation
49. **queen-attack** - Chess piece movement
50. **robot-simulator** - Direction and movement
51. **secret-handshake** - Binary to actions
52. **yacht** - Dice scoring game (Yahtzee-like)

### Advanced Challenges (53-70)
53. **nucleotide-count** - DNA sequence analysis
54. **phone-number** - NANP validation
55. **scrabble-score** - Letter scoring system
56. **luhn** - Credit card validation
57. **sum-of-multiples** - Set operations
58. **clock** - Time arithmetic without dates
59. **grade-school** - Student roster management
60. **saddle-points** - Matrix analysis
61. **series** - Substring extraction
62. **diamond** - ASCII art generation
63. **sublist** - List relationship detection
64. **all-your-base** - Base conversion
65. **meetup** - Complex date calculations
66. **kindergarten-garden** - Pattern parsing
67. **rna-transcription** - DNA to RNA conversion
68. **high-scores** - Game score management
69. **spiral-matrix** - Matrix generation in spiral order
70. **etl** - Extract, Transform, Load data conversion

### Expert Level (71-84)
71. **protein-translation** - RNA codon to protein translation
72. **wordy** - Word problem parser and evaluator
73. **say** - Number to English words converter
74. **react** - Reactive programming with cells and callbacks
75. **zebra-puzzle** - Constraint satisfaction logic puzzle
76. **two-bucket** - Water pouring puzzle with BFS
77. **palindrome-products** - Finding palindrome products in ranges
78. **resistor-color-duo** - Two-band resistor value calculation
79. **binary** - Binary to decimal conversion
80. **eliuds-eggs** - Bit counting (popcount) algorithm
81. **dnd-character** - D&D character generation with dice rolls
82. **resistor-color-trio** - Three-band resistor with unit notation
83. **flower-field** - Minesweeper-like flower counting
84. **intergalactic-transmission** - Parity bit error detection

## 🔧 Autonomous Solver Architecture

The solution uses a Node.js-based automation system with:
- **Playwright** for web browser automation
- **Exercism CLI** integration for downloading and submitting exercises
- **CSolutionGenerator** class with pattern-based C code generation
- **Automated testing** with make and Unity framework
- **GitHub integration** for automatic PR merging

### Key Components
```
exercism/
├── src/
│   ├── ExercismSolver.js    # Web automation with Playwright
│   ├── ExercismCLI.js       # CLI-based solver
│   └── CSolutionGenerator.js # C code generation logic
├── cli-solve.js             # CLI entry point
├── solve.js                  # Web automation entry point
└── exercism-workspace/      # Downloaded exercises (84 completed)
```

## 🚀 Getting Started

### Prerequisites
- GCC compiler
- Make build tool
- Exercism CLI
- Node.js 16+ (for the automated solver)

### Running Solutions
Each exercise is in its own directory under `exercism-workspace/c/`. To test any solution:

```bash
cd exercism-workspace/c/[exercise-name]
make test
```

### Using the Automated Solver
```bash
# Install dependencies
npm install

# Configure Exercism token in .env
echo "EXERCISM_TOKEN=your-token-here" > .env

# Solve a specific exercise
node cli-solve.js --exercise hello-world

# Solve all available exercises
node cli-solve.js --all
```

## 📈 Technical Achievements

### Memory Management
- Proper use of malloc/free with no memory leaks
- Dynamic array resizing and buffer management
- Careful handling of string allocations

### Algorithm Complexity
- O(n log n) sorting implementations
- O(1) space complexity where possible
- Efficient prime number generation with Sieve of Eratosthenes
- Binary search for logarithmic time lookups
- BFS for optimal pathfinding in Two-Bucket puzzle
- Dynamic programming for Knapsack optimization

### Code Quality
- Clean, readable code following C best practices
- Comprehensive error handling
- Input validation and edge case coverage
- 100% test pass rate across all exercises

### Advanced Features Implemented
- Doubly-linked list with full CRUD operations
- Circular buffer with overwrite capability
- Rational number arithmetic with automatic reduction
- Complex number operations including exponentials
- Date/time calculations without standard library functions
- Base conversion for arbitrary number systems
- Reactive programming system with dependency tracking
- Constraint satisfaction solver for logic puzzles
- VLQ encoding/decoding for MIDI protocol
- Parity bit error detection for data transmission
- Minesweeper-like game logic

## 📝 Development Process

Each exercise followed a systematic approach:
1. **Download** - Automated retrieval using Exercism CLI
2. **Analysis** - Parse test files to understand requirements
3. **Implementation** - Generate or write C solution
4. **Testing** - Run Unity tests with make
5. **Iteration** - Fix any failing tests
6. **Submission** - Automatic upload to Exercism
7. **PR Merge** - GitHub integration for tracking

## 🏆 Accomplishments

- Successfully completed **84 exercises** autonomously
- Achieved **100% test pass rate** across all exercises
- Demonstrated proficiency in:
  - Data structures (lists, buffers, trees, hash tables)
  - Algorithms (searching, sorting, mathematical, graph traversal)
  - String processing and parsing
  - Memory management
  - Error handling
  - Test-driven development
  - Advanced programming concepts (reactive systems, constraint satisfaction)
  - Bit manipulation and low-level operations
  - Game logic and simulations
- All solutions submitted and accepted by Exercism
- Full GitHub integration with automatic PR merging

## 📚 Notable Complex Exercises

### React (Exercise #74)
Implemented a complete reactive programming system with:
- Input and compute cells with automatic value propagation
- Callback system that fires when cell values change
- Proper memory management and circular dependency handling

### Zebra Puzzle (Exercise #75)
Solved the famous Einstein's Riddle using:
- Constraint satisfaction with backtracking
- Efficient pruning to reduce search space
- Correctly identified that the Norwegian drinks water and the Japanese owns the zebra

### Two-Bucket (Exercise #76)
Implemented the classic water pouring puzzle using:
- Breadth-First Search (BFS) for finding minimum moves
- Hash table for visited state tracking
- GCD-based impossibility detection

### Intergalactic Transmission (Exercise #84)
Implemented error detection system with:
- Parity bit calculation and validation
- Bit-level message encoding/decoding
- Proper error handling for transmission errors

### Flower Field (Exercise #83)
Created Minesweeper-like game logic with:
- 8-directional neighbor counting
- Dynamic memory management for grid annotation
- Edge case handling for various grid sizes

---

*Solutions developed using an autonomous solver with assistance from Claude Code*
*All code follows C best practices and passes Exercism's test suites*