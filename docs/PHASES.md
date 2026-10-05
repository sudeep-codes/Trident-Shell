# PHASES — Trident-Shell development plan

Work is organized by **phase**, not by calendar. A phase is finished only when its **exit criteria** are met; do not start the next phase's dependent work before that. Tasks within a phase run in parallel across the three people.

**Owners:** P1 = parser/built-ins/observability, P2 = pipes/redirection/testing, P3 = job control/signals.

## Phase overview
| Phase | Name | Outcome |
|---|---|---|
| 1 | Foundation & Design | Docs, repo, agreed interfaces, compiling stubs |
| 2 | Core Execution | Shell runs simple commands; survives Ctrl+C |
| 3 | Built-ins, Redirection, Background Launch | Usable single-command shell |
| 4 | Pipes & Job Commands | Pipelines and `jobs`/`fg`/`bg` working separately |
| 5 | Integration & Signals | All core features work together |
| 6 | Hardening (risk buffer) | Stable: no leaks, hangs, or zombies |
| 7 | Edge Cases & Benchmarks | Shell matches bash on edge cases; results measured |
| 8 | Documentation & Delivery | README, demo, report, final submission |

---

## Phase 1 — Foundation & Design
**Goal:** everyone works from the same plan and the same interfaces.
- **All:** commit `docs/`, agree on grammar and struct/function signatures, set up Git workflow and CI.
- **All:** confirm stub project builds with `make` on everyone's machine.
- **P3:** read `SIGNALS_AND_JOBS.md`; understand process groups before coding.

**Exit criteria**
- [ ] Repo created, branch rules agreed, CI green on stubs
- [ ] `ARCHITECTURE.md` interfaces signed off by all three
- [ ] Each person can build and run `./nsh`

## Phase 2 — Core Execution
**Goal:** the shell reads a line and runs a real program.
- **P1:** lexer (words, quotes, operators), parser for a single command, basic `fork` + `execvp` + `waitpid`.
- **P2:** test harness (`tests/run.sh`) with first ~10 cases; compares against bash.
- **P3:** throwaway spike: `setpgid`, `tcsetpgrp`, shell ignores SIGINT/SIGTSTP.

**Exit criteria**
- [ ] `ls`, `pwd`, `echo hi` run correctly; unknown command gives exit 127
- [ ] Ctrl+C at the prompt does not kill the shell
- [ ] Test harness runs and reports pass/fail

## Phase 3 — Built-ins, Redirection, Background Launch
**Goal:** a practically usable single-command shell.
- **P1:** `cd`, `exit`, `export`, `$VAR` and `$?` expansion, error messages.
- **P2:** `<`, `>`, `>>` redirection on single commands.
- **P3:** job table, `&` launches a background job, SIGCHLD reaping.

**Exit criteria**
- [ ] Built-ins and expansion pass their tests
- [ ] All three redirection operators pass, including error cases (missing file)
- [ ] `sleep 1 &` returns to the prompt immediately and leaves no zombie

## Phase 4 — Pipes & Job Commands
**Goal:** the two largest features each work on their own.
- **P2:** 2-stage pipes, then N-stage pipes; close all unused fds.
- **P3:** `jobs`, `fg`, `bg`, one process group per pipeline.
- **P1:** `trace` mode with hooks in executor and job manager.

**Exit criteria**
- [ ] `a | b | c` works, including `yes | head` (SIGPIPE) and large data
- [ ] `jobs` lists correct states; `fg`/`bg` resume the right job
- [ ] `trace on` prints fork/exec/setpgid events

## Phase 5 — Integration & Signals
**Goal:** pipelines, redirection, background jobs and signals all combine correctly.
- **P1 + P2:** merge parser, redirection and pipes into one executor path.
- **P3:** Ctrl+C / Ctrl+Z reach only the foreground group; stopped jobs tracked; terminal reclaimed.
- **P1:** `stats` command using `wait4` rusage.

**Exit criteria**
- [ ] `cat f | sort > out &` works
- [ ] Ctrl+Z on a pipeline stops all its stages; `fg` resumes them
- [ ] Ctrl+C kills only the foreground job; shell keeps running
- [ ] `stats` shows wall/user/sys time and max RSS per job

## Phase 6 — Hardening (risk buffer)
**Goal:** fix the subtle bugs. This is the highest-risk phase; process groups and terminal control are the hardest parts.
- **All:** work through the pitfalls checklist in `SIGNALS_AND_JOBS.md`.
- **P2:** full regression run, Valgrind clean, fd-leak and zombie checks.
- Use spare time here to absorb slips from earlier phases.

**Exit criteria**
- [ ] `make valgrind` clean
- [ ] No leaked fds, no zombies after stress tests (50 background jobs)
- [ ] Interactive (pty) tests pass for Ctrl+C, Ctrl+Z, fg, bg

## Phase 7 — Edge Cases & Benchmarks
**Goal:** behaviour matches bash on awkward input; performance is measured.
- **All:** quoting corner cases, empty pipelines, bad files, very long lines, EOF handling.
- **P1:** run benchmark experiments (spawn latency, pipeline throughput, job overhead, trace overhead) against bash and dash; produce charts.

**Exit criteria**
- [ ] Every row of the `TESTING.md` matrix passes
- [ ] At least two benchmark experiments completed with charts and recorded system info

## Phase 8 — Documentation & Delivery
**Goal:** everything needed for submission.
- **All:** README (build/run), architecture diagram, demo recording (asciinema or GIF), written report covering design decisions, challenges (esp. process groups) and each member's contribution.
- Final cleanup: remove dead code, tidy commit history, tag a release.

**Exit criteria**
- [ ] All items in the `PRD.md` deliverables checklist ticked
- [ ] Report and demo reviewed by all three members

---

## Go / no-go checkpoints
| After | Check |
|---|---|
| Phase 2 | `ls` runs and the shell survives Ctrl+C |
| Phase 4 | Pipelines and background jobs each work independently |
| Phase 5 | All core features integrated; if not, cut per the list below |

## Cut order if behind
1. Reduce benchmarks to one experiment
2. Drop `stats` (keep `trace`)
3. Drop `$VAR`/`$?` expansion (keep `export`)

**Never cut:** pipes, redirection, job control, signal handling.

## Dependency notes
- Phase 2's executor is the base for everyone; keep it stable before Phase 3 work depends on it.
- Phase 4 pipes and job control touch the same executor code; P2 and P3 should agree on the `fork`/`setpgid` ordering early (see `SIGNALS_AND_JOBS.md`).