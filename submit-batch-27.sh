#!/bin/bash

# List of 27 exercises to submit
exercises=(
    "sublist"
    "dot-dsl"
    "isbn-verifier"
    "paasio"
    "queen-attack"
    "saddle-points"
    "scrabble-score"
    "sieve"
    "simple-linked-list"
    "spiral-matrix"
    "two-bucket"
    "variable-length-quantity"
    "largest-series-product"
    "list-ops"
    "rail-fence-cipher"
    "roman-numerals"
    "rotational-cipher"
    "parallel-letter-frequency"
    "macros"
    "poker"
    "forth"
    "react"
    "circular-buffer"
    "decimal"
    "grep"
    "dominoes"
    "doubly-linked-list"
)

cd /home/murr2k/projects/exercism/exercism-workspace/rust

echo "🚀 Submitting 27 Rust Exercises to Exercism"
echo "==========================================="
echo ""

submitted=0
failed=0
not_found=0
already_submitted=0

for exercise in "${exercises[@]}"; do
    echo "[$((submitted + failed + not_found + already_submitted + 1))/27] Processing $exercise..."
    
    # Check if exercise directory exists
    if [ ! -d "$exercise" ]; then
        echo "  ❓ Exercise directory not found, trying to download..."
        
        # Try to download the exercise
        if exercism download --track=rust --exercise="$exercise" 2>&1 | grep -q "Downloaded"; then
            echo "  ✓ Downloaded successfully"
        else
            echo "  ❌ Could not download exercise"
            ((not_found++))
            continue
        fi
    fi
    
    # Check if src/lib.rs exists
    if [ ! -f "$exercise/src/lib.rs" ]; then
        echo "  ❌ No src/lib.rs file found"
        ((not_found++))
        continue
    fi
    
    cd "$exercise" 2>/dev/null || {
        echo "  ❌ Cannot enter directory"
        ((not_found++))
        continue
    }
    
    # Check if implementation exists (not just todo)
    if grep -q "todo!()\|unimplemented!()" "src/lib.rs" 2>/dev/null; then
        echo "  ⚠️ Exercise has todo!() - needs implementation"
        ((failed++))
        cd ..
        continue
    fi
    
    # Run tests to verify they pass
    echo "  Running tests..."
    if cargo test --lib 2>&1 | grep -q "test result: ok"; then
        echo "  ✓ Tests pass"
        
        # Submit the exercise
        echo "  Submitting to Exercism..."
        output=$(exercism submit src/lib.rs 2>&1)
        
        if echo "$output" | grep -q "Your solution has been submitted successfully"; then
            echo "  ✅ Submitted successfully!"
            url=$(echo "$output" | grep -o "https://[^ ]*" | head -1)
            [ -n "$url" ] && echo "     View at: $url"
            ((submitted++))
            
            # Wait 5 seconds for processing
            echo "  Waiting 5 seconds..."
            sleep 5
            
        elif echo "$output" | grep -q "No files you submitted have changed"; then
            echo "  📋 Already submitted (no changes)"
            ((already_submitted++))
            
        elif echo "$output" | grep -q "429 Too Many Requests"; then
            wait_time=$(echo "$output" | grep -o "after [0-9]* seconds" | grep -o "[0-9]*")
            echo "  ⏰ Rate limited, waiting ${wait_time}s..."
            sleep $((wait_time + 1))
            
            # Retry
            output=$(exercism submit src/lib.rs 2>&1)
            if echo "$output" | grep -q "Your solution has been submitted successfully"; then
                echo "  ✅ Submitted successfully after retry!"
                ((submitted++))
            else
                echo "  ❌ Failed after retry"
                ((failed++))
            fi
        else
            echo "  ❌ Submission failed: ${output:0:100}..."
            ((failed++))
        fi
    else
        echo "  ❌ Tests fail - cannot submit"
        test_result=$(cargo test --lib 2>&1 | grep "test result:")
        echo "     $test_result"
        ((failed++))
    fi
    
    cd ..
    
    # Wait between submissions to avoid rate limiting
    echo "  Waiting 10 seconds before next submission..."
    sleep 10
    echo ""
done

echo "==========================================="
echo "SUBMISSION REPORT FOR 27 EXERCISES"
echo "==========================================="
echo "✅ Successfully submitted: $submitted"
echo "📋 Already submitted: $already_submitted"
echo "❌ Failed (tests or submission): $failed"
echo "❓ Not found/no implementation: $not_found"
echo ""
echo "Total processed: 27"
echo "Success rate: $(( (submitted + already_submitted) * 100 / 27 ))%"