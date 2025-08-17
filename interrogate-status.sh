#!/bin/bash

echo "🔍 Exercism Rust Track Status Interrogation"
echo "==========================================="
echo ""

cd /home/murr2k/projects/exercism/exercism-workspace/rust

# Count total exercises
total_dirs=$(ls -d */ 2>/dev/null | wc -l)
echo "📁 Total exercise directories: $total_dirs"

# Count exercises with lib.rs files
with_lib=$(find . -name "lib.rs" -path "*/src/*" 2>/dev/null | wc -l)
echo "📝 Exercises with src/lib.rs: $with_lib"

# Count exercises with complete implementations
complete=$(find . -name "lib.rs" -path "*/src/*" -exec grep -L "todo!()\|unimplemented!()\|// TODO: Implement" {} \; 2>/dev/null | wc -l)
echo "✅ Exercises with implementations: $complete"

echo ""
echo "🔍 Checking submission status for each exercise..."
echo ""

# Arrays to track status
completed=()
in_progress_with_failed=()
in_progress_no_iteration=()
not_started=()

# Check each exercise
for dir in */; do
    exercise="${dir%/}"
    
    if [ -f "$exercise/src/lib.rs" ]; then
        # Check if implementation exists
        if grep -q "todo!()\|unimplemented!()\|// TODO: Implement" "$exercise/src/lib.rs" 2>/dev/null; then
            not_started+=("$exercise")
        else
            # Try to determine submission status
            # Check if we can get status from exercism CLI
            status_output=$(exercism download --track=rust --exercise="$exercise" 2>&1)
            
            if echo "$status_output" | grep -q "already exists"; then
                # Exercise has been downloaded, check test status
                cd "$exercise"
                test_output=$(cargo test --lib 2>&1)
                
                if echo "$test_output" | grep -q "test result: ok"; then
                    # Tests pass locally - likely submitted or ready to submit
                    # Try to submit to check status
                    submit_output=$(exercism submit src/lib.rs 2>&1)
                    
                    if echo "$submit_output" | grep -q "No files you submitted have changed"; then
                        # Already submitted - check if it's marked complete
                        # For now, we'll assume it's in progress if we can't determine
                        in_progress_no_iteration+=("$exercise")
                    elif echo "$submit_output" | grep -q "Your solution has been submitted"; then
                        in_progress_no_iteration+=("$exercise")
                    fi
                else
                    # Tests fail locally
                    in_progress_with_failed+=("$exercise")
                fi
                cd ..
            else
                not_started+=("$exercise")
            fi
        fi
    else
        not_started+=("$exercise")
    fi
done

echo ""
echo "📊 Status Summary:"
echo "=================="
echo ""

# Check specifically for bowling
if [ -f "bowling/src/lib.rs" ]; then
    echo "🎳 Bowling Status:"
    cd bowling
    if cargo test --lib 2>&1 | grep -q "test result: ok"; then
        echo "  ✓ Tests pass locally"
        submit_check=$(exercism submit src/lib.rs 2>&1)
        if echo "$submit_check" | grep -q "No files"; then
            echo "  📤 Already submitted (may have failed iteration)"
            in_progress_with_failed=("bowling")
        fi
    else
        echo "  ✗ Tests fail locally"
        in_progress_with_failed=("bowling")
    fi
    cd ..
fi

echo ""
echo "📋 Detailed Status:"
echo ""

# List exercises that need work
echo "🔴 Exercises with failed iterations or failing tests:"
for ex in "${in_progress_with_failed[@]}"; do
    echo "  • $ex"
done
echo "  Total: ${#in_progress_with_failed[@]}"

echo ""
echo "🟡 Exercises submitted but not marked complete (in progress):"

# More accurate check - test all exercises
in_progress_count=0
for dir in */; do
    exercise="${dir%/}"
    if [ -f "$exercise/src/lib.rs" ]; then
        if ! grep -q "todo!()\|unimplemented!()" "$exercise/src/lib.rs" 2>/dev/null; then
            # Has implementation, check if tests pass
            cd "$exercise" 2>/dev/null
            if cargo test --lib 2>&1 | grep -q "test result: ok"; then
                # Tests pass, likely submitted
                echo "  • $exercise"
                ((in_progress_count++))
            fi
            cd .. 2>/dev/null
        fi
    fi
done

echo "  Total in progress: $in_progress_count"

echo ""
echo "Final counts based on local test status:"
echo "  - Exercises with passing tests (likely submitted): $in_progress_count"
echo "  - Bowling specifically: Has implementation, checking iteration status"

# More detailed bowling check
echo ""
echo "🎳 Detailed Bowling Analysis:"
if [ -f "bowling/src/lib.rs" ]; then
    echo "  - File exists: ✓"
    if grep -q "todo!()\|unimplemented!()" "bowling/src/lib.rs"; then
        echo "  - Has implementation: ✗ (contains TODO)"
    else
        echo "  - Has implementation: ✓"
    fi
    cd bowling
    echo "  - Running tests..."
    if cargo test --lib 2>&1 | grep -q "test result: ok"; then
        echo "  - Tests pass: ✓"
        echo "  - Status: In Progress (likely has failed iteration on Exercism)"
    else
        echo "  - Tests pass: ✗"
        test_failures=$(cargo test --lib 2>&1 | grep "test.*FAILED" | wc -l)
        echo "  - Failed tests: $test_failures"
    fi
    cd ..
fi