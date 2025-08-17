#!/bin/bash

# Exercises that were just completed
exercises=(
    "secret-handshake"
    "say"
    "wordy"
    "alphametics"
    "grade-school"
    "custom-set"
    "tournament"
    "ocr-numbers"
    "yacht"
    "bowling"
)

cd /home/murr2k/projects/exercism/exercism-workspace/rust

echo "🦀 Submitting Final Batch of Rust Exercises"
echo "==========================================="
echo ""

submitted=0
failed=0
skipped=0

for exercise in "${exercises[@]}"; do
    echo "[$((submitted + failed + skipped + 1))/${#exercises[@]}] Processing $exercise..."
    
    if [ -d "$exercise" ] && [ -f "$exercise/src/lib.rs" ]; then
        cd "$exercise"
        
        # Run tests to verify
        echo "  Running tests..."
        test_output=$(cargo test --lib 2>&1)
        
        if echo "$test_output" | grep -q "test result: ok"; then
            echo "  ✓ All tests pass"
            
            # Submit the exercise
            echo "  Submitting to Exercism..."
            output=$(exercism submit src/lib.rs 2>&1)
            
            if echo "$output" | grep -q "Your solution has been submitted successfully"; then
                echo "  ✅ Submitted successfully!"
                url=$(echo "$output" | grep -o "https://[^ ]*" | head -1)
                [ -n "$url" ] && echo "     View at: $url"
                ((submitted++))
                
                # Wait for processing
                echo "  Waiting 5 seconds for Exercism to process..."
                sleep 5
                
            elif echo "$output" | grep -q "No files you submitted have changed"; then
                echo "  📋 Already submitted (no changes)"
                ((submitted++))
                
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
                echo "  ❌ Submission failed"
                echo "     Error: ${output:0:100}..."
                ((failed++))
            fi
        else
            echo "  ⚠️ Tests failing, skipping submission"
            # Show which tests are failing
            echo "$test_output" | grep "test.*FAILED" | head -3
            ((skipped++))
        fi
        
        cd ..
        
        # Wait between submissions
        echo "  Waiting 10 seconds before next..."
        sleep 10
    else
        echo "  ❌ Exercise not found or no lib.rs"
        ((skipped++))
    fi
    
    echo ""
done

echo "==========================================="
echo "FINAL SUBMISSION REPORT"
echo "==========================================="
echo "✅ Successfully submitted: $submitted"
echo "❌ Failed submissions: $failed"
echo "⚠️ Skipped (not ready): $skipped"
echo ""
echo "Total exercises processed: ${#exercises[@]}"
echo ""

# Count total completed exercises
total_complete=$(find . -name "lib.rs" -path "*/src/*" -exec grep -L "todo!()\|unimplemented!()\|// TODO" {} \; | wc -l)
echo "📊 Total Rust exercises with implementations: $total_complete"
echo ""
echo "Check https://exercism.org/my/tracks/rust to see your progress!"