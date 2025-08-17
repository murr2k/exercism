#!/bin/bash

echo "📊 Final Rust Exercise Status Report"
echo "===================================="
echo ""

cd /home/murr2k/projects/exercism/exercism-workspace/rust

# Count exercises
total_dirs=$(find . -maxdepth 1 -type d | wc -l)
total_dirs=$((total_dirs - 1))  # Subtract current directory

echo "📁 Total exercise directories: $total_dirs"

# Count exercises with src/lib.rs
with_lib=$(find . -name "lib.rs" -path "*/src/*" | wc -l)
echo "📝 Exercises with src/lib.rs: $with_lib"

# Count complete implementations
complete=0
incomplete=0
incomplete_list=()

for dir in */; do
    exercise="${dir%/}"
    if [ -f "$exercise/src/lib.rs" ]; then
        if grep -q "todo!()\|unimplemented!()" "$exercise/src/lib.rs" 2>/dev/null; then
            incomplete=$((incomplete + 1))
            incomplete_list+=("$exercise")
        else
            # Check if tests pass
            cd "$exercise" 2>/dev/null
            if cargo test --lib 2>&1 | grep -q "test result: ok"; then
                complete=$((complete + 1))
            else
                incomplete=$((incomplete + 1))
                incomplete_list+=("$exercise")
            fi
            cd .. 2>/dev/null
        fi
    fi
done

echo "✅ Exercises with passing tests: $complete"
echo "❌ Exercises needing work: $incomplete"

echo ""
echo "Exercises that need implementation or fixes:"
for ex in "${incomplete_list[@]}"; do
    echo "  • $ex"
done

echo ""
echo "Based on user feedback, there should be:"
echo "  • 18 exercises in progress on Exercism"
echo "  • 1 (Bowling) with failed iteration"
echo "  • 17 with no iterations"
echo ""

echo "Checking submission status of recently worked exercises:"
recent_exercises=(
    "pascals-triangle"
    "robot-simulator"
    "palindrome-products"
    "diamond"
    "two-bucket"
)

for ex in "${recent_exercises[@]}"; do
    if [ -f "$ex/src/lib.rs" ]; then
        echo -n "  $ex: "
        cd "$ex" 2>/dev/null
        if cargo test --lib 2>&1 | grep -q "test result: ok"; then
            echo "✓ Tests pass locally"
        else
            failed_count=$(cargo test --lib 2>&1 | grep "test result:" | grep -o "[0-9]* failed" | grep -o "[0-9]*")
            echo "✗ $failed_count tests failing"
        fi
        cd .. 2>/dev/null
    fi
done

echo ""
echo "==============================================="
echo "Summary:"
echo "  • Total exercises in workspace: $total_dirs"
echo "  • Exercises with implementations: $with_lib"
echo "  • Exercises with all tests passing: $complete"
echo "  • Exercises needing work: $incomplete"
echo ""
echo "The 18 in-progress exercises on Exercism likely include:"
echo "  1. Bowling (failed iteration)"
echo "  2-18. Some combination of:"
echo "     - Newly implemented exercises not yet marked complete"
echo "     - Exercises with partial implementations"
echo "     - Exercises started on web but not downloaded"