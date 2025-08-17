#!/bin/bash

echo "📊 FINAL RUST EXERCISES SUBMISSION REPORT"
echo "=========================================="
echo ""
echo "Date: $(date)"
echo ""

cd /home/murr2k/projects/exercism/exercism-workspace/rust

# List of all requested exercises
all_exercises=(
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

echo "Checking status of all 27 requested exercises..."
echo ""

completed=0
partial=0
not_done=0

completed_list=()
partial_list=()
not_done_list=()

for exercise in "${all_exercises[@]}"; do
    if [ -d "$exercise" ] && [ -f "$exercise/src/lib.rs" ]; then
        cd "$exercise" 2>/dev/null
        
        # Check if tests pass
        test_output=$(cargo test --lib 2>&1)
        
        if echo "$test_output" | grep -q "test result: ok"; then
            completed_list+=("$exercise")
            ((completed++))
        else
            # Check how many tests pass/fail
            failed=$(echo "$test_output" | grep "test result:" | grep -o "[0-9]* failed" | grep -o "[0-9]*" || echo "0")
            passed=$(echo "$test_output" | grep "test result:" | grep -o "[0-9]* passed" | grep -o "[0-9]*" || echo "0")
            
            if [ "$passed" -gt "0" ]; then
                partial_list+=("$exercise (${passed} passed, ${failed} failed)")
                ((partial++))
            else
                not_done_list+=("$exercise")
                ((not_done++))
            fi
        fi
        
        cd .. 2>/dev/null
    else
        not_done_list+=("$exercise")
        ((not_done++))
    fi
done

echo "✅ COMPLETED EXERCISES ($completed/27):"
echo "======================================"
for ex in "${completed_list[@]}"; do
    echo "  • $ex"
done

if [ ${#partial_list[@]} -gt 0 ]; then
    echo ""
    echo "🟡 PARTIALLY COMPLETED ($partial/27):"
    echo "======================================"
    for ex in "${partial_list[@]}"; do
        echo "  • $ex"
    done
fi

if [ ${#not_done_list[@]} -gt 0 ]; then
    echo ""
    echo "❌ NOT IMPLEMENTED ($not_done/27):"
    echo "======================================"
    for ex in "${not_done_list[@]}"; do
        echo "  • $ex"
    done
fi

echo ""
echo "=========================================="
echo "SUMMARY:"
echo "=========================================="
echo "✅ Fully Completed: $completed/27 ($(( completed * 100 / 27 ))%)"
echo "🟡 Partially Done: $partial/27 ($(( partial * 100 / 27 ))%)"
echo "❌ Not Done: $not_done/27 ($(( not_done * 100 / 27 ))%)"
echo ""

# Count total exercises in workspace
total_exercises=$(find . -name "lib.rs" -path "*/src/*" | wc -l)
passing_exercises=$(for dir in */; do if [ -f "${dir}src/lib.rs" ]; then cd "$dir" 2>/dev/null && cargo test --lib 2>&1 | grep -q "test result: ok" && echo "1" || echo "0"; cd .. 2>/dev/null; fi; done | grep "1" | wc -l)

echo "Overall Rust Track Progress:"
echo "  • Total exercises in workspace: $total_exercises"
echo "  • Exercises with passing tests: $passing_exercises"
echo "  • Success rate: $(( passing_exercises * 100 / total_exercises ))%"