#!/bin/bash

# Fixed exercises to resubmit
exercises=(
    "crypto-square"
    "kindergarten-garden"
    "perfect-numbers"
    "phone-number"
    "pig-latin"
    "protein-translation"
)

cd /home/murr2k/projects/exercism/exercism-workspace/rust

echo "🦀 Resubmitting Fixed Rust Exercises"
echo "===================================="
echo ""

submitted=0
failed=0

for exercise in "${exercises[@]}"; do
    echo "[$((submitted + failed + 1))/${#exercises[@]}] Resubmitting $exercise..."
    
    if [ -d "$exercise" ] && [ -f "$exercise/src/lib.rs" ]; then
        cd "$exercise"
        
        # First verify tests pass
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
                
                # Wait 5 seconds to check iteration status
                echo "  Waiting 5 seconds for Exercism to process..."
                sleep 5
                
                # Optional: Check if tests pass on Exercism
                # This would require API access or parsing web response
                
            elif echo "$output" | grep -q "429 Too Many Requests"; then
                wait_time=$(echo "$output" | grep -o "after [0-9]* seconds" | grep -o "[0-9]*")
                echo "  ⏰ Rate limited, waiting ${wait_time}s..."
                sleep $((wait_time + 1))
                
                # Retry submission
                output=$(exercism submit src/lib.rs 2>&1)
                if echo "$output" | grep -q "Your solution has been submitted successfully"; then
                    echo "  ✅ Submitted successfully after retry!"
                    url=$(echo "$output" | grep -o "https://[^ ]*" | head -1)
                    [ -n "$url" ] && echo "     View at: $url"
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
            echo "  ❌ Tests failed - not submitting"
            ((failed++))
        fi
        
        cd ..
        
        # Wait between submissions
        echo "  Waiting 10 seconds before next submission..."
        sleep 10
    else
        echo "  ❌ Exercise not found"
        ((failed++))
    fi
    
    echo ""
done

echo "======================================="
echo "RESUBMISSION SUMMARY"
echo "======================================="
echo "✅ Successfully resubmitted: $submitted"
echo "❌ Failed: $failed"
echo ""
echo "Check the Exercism website to verify that the iterations now pass all tests."
echo "The fixes should result in passing iterations for all resubmitted exercises."