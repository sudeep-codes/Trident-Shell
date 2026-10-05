# nsh

A Unix shell with job control and built-in process observability, written in C.

## Build and run
```
make        # builds ./nsh
./nsh
make test   # compares against bash
make valgrind
make bench
```
Requires Linux (native, VM, or WSL2), gcc, make. Optional: valgrind, dash, bc.

## Docs
See `docs/` - start with `PRD.md`, then `ARCHITECTURE.md`.

## Team
| Member | Area |
|---|---|
| Person 1 | lexer, parser, built-ins, expansion, trace/stats, benchmarks |
| Person 2 | pipes, redirection, test harness |
| Person 3 | job control, signals, process groups |
