# ARCHITECTURE

## 1. Data flow
```
input line
  -> [lexer]    tokens
  -> [expander] tokens with $VAR / $? resolved
  -> [parser]   Pipeline (commands + redirections + background flag)
  -> [executor] builtin in-process, or fork per stage
  -> [jobs]     job table, fg/bg, reaping
  -> [signals]  SIGINT/SIGTSTP/SIGCHLD policy
  -> [trace/stats] observability hooks (called from executor + jobs)
```

## 2. File layout
```
nsh/
  Makefile
  src/
    main.c        REPL loop, prompt, shell init
    lexer.c/h     tokenizer (quotes, escapes, operators)
    expand.c/h    $VAR and $? expansion
    parser.c/h    tokens -> Pipeline
    executor.c/h  fork/exec, pipes, redirection
    builtins.c/h  cd, exit, export, jobs, fg, bg, trace, stats
    jobs.c/h      job table + state transitions
    signals.c/h   sigaction setup, handlers
    trace.c/h     event logging
    stats.c/h     rusage collection / reporting
    util.c/h      xmalloc, error helpers, string buffers
  tests/          see TESTING.md
  bench/          benchmark scripts
  docs/           these files
```

## 3. Shared interfaces (AGREE IN WEEK 1, change only via PR + MEMORY.md note)

```c
/* ---- lexer.h ---- */
typedef enum { TOK_WORD, TOK_PIPE, TOK_REDIR_IN, TOK_REDIR_OUT,
               TOK_REDIR_APPEND, TOK_BG, TOK_EOF } TokenType;
typedef struct { TokenType type; char *text; } Token;
typedef struct { Token *items; size_t len; } TokenList;
int  lex(const char *line, TokenList *out);   /* 0 ok, -1 syntax error (msg on stderr) */
void tokenlist_free(TokenList *t);

/* ---- parser.h ---- */
typedef enum { REDIR_IN, REDIR_OUT, REDIR_APPEND } RedirType;
typedef struct { RedirType type; char *path; } Redir;
typedef struct {
    char  **argv;          /* NULL-terminated */
    Redir  *redirs; size_t nredirs;
} Command;
typedef struct {
    Command *cmds; size_t ncmds;
    int background;        /* 1 if trailing & */
    char *raw;             /* original line, for jobs listing */
} Pipeline;
int  parse(const TokenList *t, Pipeline *out);  /* 0 ok, -1 error */
void pipeline_free(Pipeline *p);

/* ---- executor.h ---- */
int  execute(Pipeline *p);   /* returns exit status of last stage (fg) or 0 (bg) */
int  is_builtin(const char *name);
int  run_builtin(Command *c);

/* ---- jobs.h ---- */
typedef enum { JOB_RUNNING, JOB_STOPPED, JOB_DONE } JobState;
typedef struct {
    int      id;           /* %1, %2 ... */
    pid_t    pgid;
    pid_t   *pids; size_t npids;
    JobState state;
    char    *cmdline;
    int      background;
    struct timespec start; /* for stats */
} Job;
Job *job_add(pid_t pgid, pid_t *pids, size_t n, const char *cmd, int bg);
Job *job_find_by_id(int id);
Job *job_find_by_pgid(pid_t pgid);
void job_remove(Job *j);
void jobs_print(void);
int  job_put_fg(Job *j, int cont);   /* give terminal, SIGCONT if cont, wait, reclaim */
int  job_put_bg(Job *j, int cont);

/* ---- signals.h ---- */
void signals_init(void);       /* shell ignores SIGINT/SIGTSTP/SIGTTOU/SIGTTIN; installs SIGCHLD */
void signals_reset_child(void);/* restore SIG_DFL in forked child before exec */

/* ---- trace.h / stats.h ---- */
extern int g_trace;
void trace_event(const char *fmt, ...);        /* no-op when g_trace == 0 */
void stats_record(pid_t pid, const struct rusage *ru, double wall_sec);
void stats_print(void);
```

## 4. Ownership
| Module | Owner |
|---|---|
| lexer, expand, parser, builtins (cd/exit/export), trace, stats, benchmarks | Person 1 |
| executor pipes/redirection, test harness | Person 2 |
| jobs, signals, process groups, fg/bg | Person 3 |
| Basic fork/exec in executor | Person 1 (week 2), then handed to Person 2 |

## 5. Key design decisions
- **One process group per pipeline**; pgid = pid of first stage.
- **Shell ignores** SIGINT/SIGTSTP/SIGTTOU/SIGTTIN; children reset to default before exec.
- **Reaping** with `waitpid(-1, ..., WNOHANG | WUNTRACED | WCONTINUED)`.
- **Resource stats** gathered via `wait4()` so rusage comes with each reaped child.
- **Built-ins** run in the shell process unless part of a multi-stage pipeline.
