#!/usr/bin/env bash
# Spawn latency: run /bin/true N times in each shell. Report wall time.
N=${1:-5000}
cd "$(dirname "$0")/.." || exit 1
script=$(mktemp); for _ in $(seq "$N"); do echo /bin/true; done > "$script"
for sh in "bash --norc" "dash" "./nsh"; do
    s=$(date +%s.%N); $sh < "$script" > /dev/null 2>&1; e=$(date +%s.%N)
    printf '%-12s %s s\n' "$sh" "$(echo "$e - $s" | bc)"
done
rm -f "$script"
