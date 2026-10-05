# MEMORY — running log (update continuously; newest entries on top)

## Current status
- Week: 1
- Done: _nothing yet_
- In progress: _fill in_
- Blocked: _none_

## Decisions (append only)
| Date | Decision | Why | Who |
|---|---|---|---|
| wk1 | Use `sigaction`, not `signal` | portable, defined semantics | all |
| wk1 | One process group per pipeline | Ctrl+C/Z must hit whole pipeline | P3 |
| wk1 | `wait4` for reaping | gives rusage for `stats` for free | P1 |
| wk1 | Out-of-scope list frozen (see PRD.md) | prevent scope creep | all |

## Gotchas / bugs found
| Date | Symptom | Cause | Fix |
|---|---|---|---|
| _example_ | pipeline hangs on `cat` | write end of pipe left open in parent | close in parent after fork |

## Interface changes
| Date | Change | Affected people |
|---|---|---|

## Open questions
- Should `fg` with no args use the most recent job or the most recently stopped? (default: most recent)
