# SIGNALS AND JOBS — the high-risk subsystem

## Concepts
- **Process group:** set of related processes; signals can be sent to the whole group (`kill(-pgid, sig)`).
- **Session / controlling terminal:** the terminal delivers Ctrl+C (SIGINT) and Ctrl+Z (SIGTSTP) to the **foreground process group** only.
- **Foreground group:** set with `tcsetpgrp(STDIN_FILENO, pgid)`.

## Shell startup
1. Loop until the shell is in the foreground (`tcgetpgrp(fd) == getpgrp()`), else send SIGTTIN to self.
2. Ignore SIGINT, SIGTSTP, SIGQUIT, SIGTTIN, SIGTTOU.
3. `setpgid(0, 0)` to make shell its own group leader; `tcsetpgrp` to take the terminal.
4. Install SIGCHLD handler with `sigaction` (`SA_RESTART | SA_NOCLDSTOP` off, since we need stop notifications via waitpid).

## Launching a pipeline
```
for each stage i:
  pid = fork()
  child:
    setpgid(0, i == 0 ? 0 : pgid)       // first child becomes leader
    if foreground: tcsetpgrp(tty, pgid)  // also in parent (race)
    signals_reset_child()                // SIG_DFL for all
    dup2 pipes/redirects, close extras
    execvp(); _exit(127)
  parent:
    setpgid(pid, pgid)                   // duplicate call on purpose
```
Then: foreground -> `job_put_fg`; background -> print `[id] pgid` and return to prompt.

## job_put_fg(job, cont)
1. `tcsetpgrp(tty, job->pgid)`
2. if `cont`: `kill(-pgid, SIGCONT)`
3. `waitpid(-pgid, &status, WUNTRACED)` for every process in the job
4. `tcsetpgrp(tty, shell_pgid)` (shell reclaims terminal)
5. if stopped: mark `JOB_STOPPED`, print `[id]+ Stopped  cmd`; if exited: remove job, set `$?`
6. restore shell terminal modes if saved

## Who handles what
| Event | Handler |
|---|---|
| Ctrl+C at prompt | shell ignores; reprint prompt |
| Ctrl+C in fg job | kernel -> fg group; shell unaffected |
| Ctrl+Z in fg job | kernel SIGTSTP -> group stops; `waitpid` returns WIFSTOPPED; shell marks job Stopped |
| bg job finishes | SIGCHLD -> handler sets flag; main loop reaps with `WNOHANG` and prints `Done` |
| bg job reads terminal | kernel sends SIGTTIN; job stops; user can `fg` it |

## Pitfalls checklist
- [ ] Forgetting `setpgid` in parent -> race, `tcsetpgrp` fails with EPERM
- [ ] Not ignoring SIGTTOU in shell -> shell stops when reclaiming the terminal
- [ ] Not resetting signals to SIG_DFL in child -> children inherit ignored Ctrl+C
- [ ] `waitpid(-1)` in the handler stealing the fg job's status from `job_put_fg`
- [ ] Unclosed pipe ends -> reader never sees EOF, pipeline hangs
- [ ] Zombies from background jobs when SIGCHLD is coalesced (loop `waitpid` until 0)
- [ ] Job table modified inside handler while main code reads it (block SIGCHLD or defer via flag)
