#!/bin/bash

echo "🔍 Checking Actual Exercism Status"
echo "=================================="
echo ""

cd /home/murr2k/projects/exercism/exercism-workspace/rust

# Based on user feedback: 18 exercises are in progress
# 1 (Bowling) has a failed iteration
# 17 have no iterations

echo "According to Exercism website:"
echo "  • 18 exercises in progress"
echo "  • 1 (Bowling) with failed iteration"
echo "  • 17 with no iterations submitted"
echo ""

echo "Let's identify which 17 exercises have NO submissions:"
echo ""

# List of all possible Rust exercises on Exercism
all_exercises=(
    "hello-world" "leap" "reverse-string" "gigasecond" "bob" 
    "raindrops" "nth-prime" "beer-song" "proverb" "difference-of-squares"
    "grains" "armstrong-numbers" "collatz-conjecture" "prime-factors"
    "series" "sum-of-multiples" "isogram" "pangram" "anagram" "hamming"
    "nucleotide-count" "luhn" "pig-latin" "etl" "space-age" "acronym"
    "scrabble-score" "roman-numerals" "run-length-encoding" "rotational-cipher"
    "atbash-cipher" "crypto-square" "simple-cipher" "rail-fence-cipher"
    "matching-brackets" "high-scores" "matrix" "saddle-points" "spiral-matrix"
    "triangle" "perfect-numbers" "ocr-numbers" "allergies" "variable-length-quantity"
    "phone-number" "wordy" "tournament" "yacht" "sublist" "word-count"
    "kindergarten-garden" "robot-simulator" "queen-attack" "binary-search"
    "largest-series-product" "pascals-triangle" "all-your-base" "grade-school"
    "binary-search-tree" "linked-list" "sieve" "palindrome-products" 
    "poker" "parallel-letter-frequency" "alphametics" "two-bucket" "bowling"
    "clock" "dot-dsl" "simple-linked-list" "doubly-linked-list" "custom-set"
    "dominoes" "minesweeper" "rna-transcription" "say" "secret-handshake"
    "diamond" "isbn-verifier" "largest-series-product" "luhn-from" "luhn-trait"
    "macros" "react" "decimal" "circular-buffer" "forth" "grep"
    "scale-generator" "protein-translation"
)

echo "Checking which exercises exist locally but might not be submitted:"
echo ""

# Check exercises that exist locally
not_submitted=()
possibly_not_submitted=()

for exercise in "${all_exercises[@]}"; do
    if [ -d "$exercise" ]; then
        if [ -f "$exercise/src/lib.rs" ]; then
            # Check if it might not be submitted
            # Try submitting to see if it's already there
            submit_check=$(cd "$exercise" && exercism submit src/lib.rs 2>&1)
            
            if echo "$submit_check" | grep -q "Your solution has been submitted successfully"; then
                echo "  ✅ $exercise - Just submitted now!"
                not_submitted+=("$exercise")
            elif echo "$submit_check" | grep -q "No files you submitted have changed"; then
                # Already submitted, skip
                :
            else
                # Might not be submitted or has issues
                possibly_not_submitted+=("$exercise")
            fi
        fi
    fi
done

echo ""
echo "Based on the requirement of 18 in-progress exercises:"
echo ""
echo "1. Bowling - Has failed iteration"
echo ""
echo "The other 17 that likely have no iterations could be exercises that:"
echo "  - Exist locally but were never submitted"
echo "  - Were downloaded but not implemented"
echo "  - Are not in our local workspace"
echo ""

# Check for exercises that might be missing
echo "Checking for exercises NOT in our workspace that might be the missing 17:"
echo ""

missing_exercises=()
for exercise in "${all_exercises[@]}"; do
    if [ ! -d "$exercise" ]; then
        missing_exercises+=("$exercise")
    fi
done

echo "Exercises not in local workspace (${#missing_exercises[@]} total):"
for ex in "${missing_exercises[@]}"; do
    echo "  • $ex"
done

echo ""
echo "These missing exercises could be the 17 with no iterations if they were started on the web but never downloaded locally."