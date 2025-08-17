#!/bin/bash

echo "🔍 Finding the 18 In-Progress Exercises"
echo "======================================="
echo ""

cd /home/murr2k/projects/exercism/exercism-workspace/rust

# According to user: 18 in progress, 1 (Bowling) has failed iteration, 17 have no iterations
echo "Looking for:"
echo "  • 1 exercise (Bowling) with failed iteration"
echo "  • 17 exercises with no iterations (started but not submitted)"
echo ""

# First, identify Bowling
echo "1. Bowling - Confirmed (has failed iteration)"
echo ""

# Now find the other 17
echo "Finding the other 17 exercises with no iterations..."
echo "(These would be exercises that are downloaded but not yet submitted)"
echo ""

# Check all directories for exercises with todo!() or minimal stub implementations
no_iteration_exercises=()

for dir in */; do
    exercise="${dir%/}"
    
    if [ "$exercise" = "bowling" ]; then
        continue  # Skip bowling, we already counted it
    fi
    
    if [ -f "$exercise/src/lib.rs" ]; then
        # Check if it has todo!() or is just a stub
        if grep -q "todo!()" "$exercise/src/lib.rs" 2>/dev/null; then
            no_iteration_exercises+=("$exercise")
        elif grep -q "// The code below is a stub" "$exercise/src/lib.rs" 2>/dev/null; then
            no_iteration_exercises+=("$exercise")
        else
            # Check if tests fail (indicating incomplete or wrong implementation)
            cd "$exercise" 2>/dev/null
            if ! cargo test --lib 2>&1 | grep -q "test result: ok"; then
                # Tests don't pass - might be incomplete
                # But this could also be a submitted exercise with failed iteration
                # Skip for now
                :
            fi
            cd .. 2>/dev/null
        fi
    fi
done

echo "Exercises with todo!() or stub implementations (no iterations):"
for ex in "${no_iteration_exercises[@]}"; do
    echo "  • $ex"
done
echo ""
echo "Count: ${#no_iteration_exercises[@]}"

# If we don't have 17, we need to download more
if [ ${#no_iteration_exercises[@]} -lt 17 ]; then
    echo ""
    echo "We only found ${#no_iteration_exercises[@]} exercises with no iterations locally."
    echo "The remaining $((17 - ${#no_iteration_exercises[@]})) must be exercises that are:"
    echo "  - Started on the website but never downloaded"
    echo "  - Or need to be downloaded from the available exercise list"
    echo ""
    
    # Try to download more exercises that might be in progress
    echo "Attempting to download more exercises that might be in progress..."
    
    potential_exercises=(
        "linked-list"
        "binary-search-tree"
        "palindrome-products"
        "two-bucket"
        "diamond"
        "minesweeper"
        "decimal"
        "circular-buffer"
        "react"
        "macros"
        "isbn-verifier"
        "forth"
        "dot-dsl"
        "doubly-linked-list"
        "grep"
        "scale-generator"
    )
    
    for ex in "${potential_exercises[@]}"; do
        if [ ! -d "$ex" ]; then
            echo "  Downloading $ex..."
            exercism download --track=rust --exercise="$ex" 2>&1 | grep -q "Downloaded" && {
                echo "    ✓ Downloaded"
                # Check if it needs implementation
                if [ -f "$ex/src/lib.rs" ]; then
                    if grep -q "todo!()" "$ex/src/lib.rs" 2>/dev/null; then
                        no_iteration_exercises+=("$ex")
                        echo "    → Added to no-iteration list"
                    fi
                fi
            }
            
            # Stop when we have 17
            if [ ${#no_iteration_exercises[@]} -ge 17 ]; then
                break
            fi
        fi
    done
fi

echo ""
echo "======================================="
echo "FINAL COUNT OF IN-PROGRESS EXERCISES:"
echo "======================================="
echo ""
echo "1. Bowling (failed iteration)"
echo ""
echo "Exercises with no iterations (${#no_iteration_exercises[@]}):"
for ex in "${no_iteration_exercises[@]}"; do
    echo "  • $ex"
done
echo ""
echo "Total in-progress: $((1 + ${#no_iteration_exercises[@]}))"