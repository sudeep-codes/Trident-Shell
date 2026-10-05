#!/usr/bin/env bash
# Runs every tests/cases/*.sh through nsh and bash, diffs stdout.
# Set NSH_WRAPPER to run nsh under valgrind.
cd "$(dirname "$0")/.." || exit 1
pass=0; fail=0
for f in tests/cases/*.sh; do
    [ -e "$f" ] || continue
    exp=$(bash --norc < "$f" 2>/dev/null)
    got=$($NSH_WRAPPER ./nsh < "$f" 2>/dev/null | sed 's/^nsh> //')
    if [ "$exp" == "$got" ]; then pass=$((pass+1)); else fail=$((fail+1)); echo "FAIL: $f"; fi
done
echo "passed=$pass failed=$fail"
[ "$fail" -eq 0 ]
