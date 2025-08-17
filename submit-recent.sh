#!/bin/bash

# List of exercises that were recently solved
exercises=(
    "matching-brackets"
    "matrix"
    "nth-prime"
    "nucleotide-count"
    "pangram"
    "proverb"
    "space-age"
    "sum-of-multiples"
    "triangle"
    "word-count"
    "clock"
    "crypto-square"
    "grade-school"
    "kindergarten-garden"
    "largest-series-product"
    "perfect-numbers"
    "phone-number"
    "pig-latin"
    "protein-translation"
    "queen-attack"
    "rail-fence-cipher"
    "roman-numerals"
    "rotational-cipher"
    "saddle-points"
    "say"
    "scrabble-score"
    "secret-handshake"
    "sieve"
    "simple-linked-list"
    "spiral-matrix"
    "sublist"
    "variable-length-quantity"
    "wordy"
    "yacht"
)

cd /home/murr2k/projects/exercism/exercism-workspace/rust

submitted=0
failed=0
already=0

for exercise in "${exercises[@]}"; do
    if [ -d "$exercise" ] && [ -f "$exercise/src/lib.rs" ]; then
        echo "[$((submitted + failed + already + 1))/${#exercises[@]}] Checking $exercise..."
        
        # First check if tests pass
        cd "$exercise"
        if cargo test --lib 2>&1 | grep -q "test result: ok"; then
            echo "  ✓ Tests pass, submitting..."
            
            # Submit the exercise
            output=$(exercism submit src/lib.rs 2>&1)
            
            if echo "$output" | grep -q "Your solution has been submitted successfully"; then
                echo "  ✅ Submitted successfully!"
                ((submitted++))
                # Extract and show the URL
                url=$(echo "$output" | grep -o "https://[^ ]*" | head -1)
                [ -n "$url" ] && echo "     $url"
            elif echo "$output" | grep -q "No files you submitted have changed"; then
                echo "  📋 Already submitted"
                ((already++))
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
                ((failed++))
            fi
        else
            echo "  ⚠️ Tests don't pass, skipping"
        fi
        
        cd ..
        
        # Wait between submissions to avoid rate limiting
        echo "  Waiting 12 seconds..."
        sleep 12
    fi
done

echo ""
echo "======================================="
echo "SUBMISSION SUMMARY"
echo "======================================="
echo "✅ Newly submitted: $submitted"
echo "📋 Already submitted: $already"
echo "❌ Failed: $failed"
echo "Total processed: $((submitted + already + failed))/${#exercises[@]}"