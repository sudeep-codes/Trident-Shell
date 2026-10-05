# RULES — coding conventions and constraints

## Build
- `gcc -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror -g`
- `make`, `make test`, `make valgrind`, `make bench`, `make clean` must all work.

## Syscalls and safety
1. Check the return value of EVERY syscall (`fork`, `pipe`, `dup2`, `open`, `execvp`, `waitpid`, `setpgid`, `tcsetpgrp`). Use `perror`/`strerror`.
2. After `fork()`, the child that fails `execvp` must call `_exit(127)`, never `exit()`.
3. Close every unused pipe end in every process. Leaked write ends cause hangs.
4. Retry `EINTR` on `read`/`waitpid` where relevant.
5. Use `sigaction`, never `signal`.
6. Signal handlers may only call async-signal-safe functions (`write`, `waitpid`, `_exit`, set `volatile sig_atomic_t` flags). No `printf`/`malloc` in handlers.
7. Block SIGCHLD around job-table updates.
8. No `system()`, `popen()`, or shelling out to another shell. Raw syscalls only.
9. Call `setpgid` in BOTH parent and child after fork (avoids a race).

## Memory
- Every `malloc` has a matching `free`; wrap with `xmalloc/xrealloc` that abort on failure.
- `valgrind --leak-check=full` must be clean on the test suite before merge.
- Parser/lexer outputs have explicit `*_free()` functions.

## Style
- One module = one `.c/.h` pair. Headers have include guards.
- Functions under ~50 lines where practical; no global state outside `jobs.c`, `signals.c`, `trace.c`.
- Names: `snake_case` for functions/variables, `UPPER_CASE` for macros, `CamelCase` for types.
- Comment WHY, not what. Document each public function in its header.

## Git workflow
- Branch per feature: `p1/lexer`, `p2/pipes`, `p3/jobs`.
- PR required to merge into `main`; at least one teammate reviews.
- `main` must always build and pass `make test`.
- Commit messages: imperative, short (`Add >> redirection`).
- Changing a shared interface in ARCHITECTURE.md requires an entry in MEMORY.md and notifying everyone.

## AI-assistant rules
- Read PRD.md (scope), ARCHITECTURE.md (interfaces), GRAMMAR.md, and this file before generating code.
- Never add features from the Out of Scope list.
- Never change shared struct/function signatures without being asked.
- Explain any syscall-ordering choice in a comment (these are the subtle bugs).
