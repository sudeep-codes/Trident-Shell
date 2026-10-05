# GRAMMAR — nsh command syntax

## 1. Lexical rules
- Whitespace (space/tab) separates words; newline ends the line.
- Operators: `|`  `<`  `>`  `>>`  `&`  (recognized even without surrounding spaces: `a|b` is 3 tokens).
- `'...'`  single quotes: everything literal, no expansion.
- `"..."`  double quotes: spaces preserved, `$VAR` and `$?` expanded, `\"` and `\\` escapes.
- `\c`     outside quotes: next character literal.
- `#`      at start of a word begins a comment to end of line.
- Unterminated quote -> syntax error `nsh: unterminated quote`.

## 2. Grammar (EBNF)
```
line      := [ pipeline [ "&" ] ]
pipeline  := command { "|" command }
command   := { redirect } WORD { WORD | redirect }
redirect  := ( "<" | ">" | ">>" ) WORD
```
- `&` is only valid at the end of a line.
- Redirections may appear anywhere in a command.
- Empty command in a pipeline (`a | | b`, `| a`, `a |`) -> `nsh: syntax error near '|'`.

## 3. Expansion (after lexing, before parsing)
| Form | Result |
|---|---|
| `$NAME` | environment value, or empty string if unset |
| `$?` | exit status of last foreground pipeline |
| `$$` | NOT supported (treated literally) |
| inside `'...'` | never expanded |

Name characters: `[A-Za-z_][A-Za-z0-9_]*`.

## 4. Worked examples
| Input | Parse result |
|---|---|
| `ls -l` | 1 cmd, argv=[ls,-l] |
| `cat < in.txt \| sort > out.txt` | 2 cmds; cmd0 redir IN in.txt; cmd1 redir OUT out.txt |
| `sleep 10 &` | 1 cmd, background=1 |
| `echo "a  b" 'c $HOME'` | argv=[echo, "a  b", "c $HOME"] |
| `a \| b \| c >> log &` | 3 cmds, cmd2 redir APPEND log, background=1 |
| `echo hi >` | error: missing redirection target |

## 5. Error messages (match bash style where practical)
`nsh: syntax error near 'X'`, `nsh: unterminated quote`, `nsh: <cmd>: command not found` (exit 127), `nsh: <file>: No such file or directory`.
