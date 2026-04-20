# minishell

A POSIX-compliant Unix shell implementation written in C, as part of the 42 School curriculum.

> Authors: 
- Charlotte Hénique — charlotte.1.hq@gmail.com — github.com/chheniqu
- Ophelia Marboeuf — m.opheliaa@gmail.com — github.com/opheliamarboeuf

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Built-in Commands](#built-in-commands)
- [Installation](#installation)
- [Usage](#usage)
- [Implementation Details](#implementation-details)

---

## Overview

Minishell recreates the core behaviour of a Unix shell — reading user input, tokenizing and parsing command lines, expanding variables, executing external programs and built-in commands, managing pipes and redirections, and handling signals interactively.

---

## Features

- Built using **libft** (custom C library)
- Interactive prompt with command history (via `readline`)
- Single and double quote handling with POSIX semantics
- Environment variable expansion (`$VAR`, `$?`)
- Pipes (`|`) with arbitrary depth
- Redirections: `<`, `>`, `>>`, `<<` (heredoc)
- Signal handling (`Ctrl-C`, `Ctrl-D`, `Ctrl-\`, `Ctrl-Z`)
- Built-in commands
- Exit status propagation (`$?`) matching bash behaviour
- Exit codes:
  - `0` — Success
  - `1` — General error
  - `2` — Syntax error (invalid token, unclosed quotes)
  - `126` — Command not executable (permission denied)
  - `127` — Command not found
  - `130` — Interrupted by `Ctrl-C` (readline or heredoc)
  - `131` — Quit by `Ctrl-\`
  - `128 + n` — Process terminated by signal `n` (e.g., 130 for SIGINT, 131 for SIGQUIT)

---

## Architecture

The shell processes each input line through five sequential phases:

```
readline input
    │
    ├─ tokenize()          → t_token linked list (with quote tracking per character)
    ├─ check_quotes()      → unclosed quote validation
    ├─ has_syntax_error()  → token-level syntax validation
    ├─ expand_variable()   → $VAR / $? substitution (quote-aware)
    ├─ has_syntax_error()  → post-expansion re-check
    ├─ parser()            → t_shell_cmd + t_redir linked lists
    └─ ft_pipex()
         ├─ handle_heredoc()    → fills heredoc fds before forking
         ├─ init_pipes()        → allocates pipe fd matrix
         └─ for each command:
              ├─ [single builtin] → run in parent
              └─ [piped/compound] → fork and execute
         └─ wait_for_children() → update exit status
```
---

## Built-in Commands

| Command | Behaviour |
|---|---|
| `echo [-n]` | Print arguments to stdout; `-n` suppresses the trailing newline |
| `cd [path]` | Change directory; no argument goes to `$HOME`; updates `PWD` and `OLDPWD` |
| `pwd` | Print the current working directory |
| `export [KEY=VALUE]` | Set/update an environment variable; no argument prints all exported variables |
| `unset KEY` | Remove an environment variable |
| `env` | Print all environment variables |
| `exit [n]` | Exit the shell with status `n % 256`; errors on non-numeric or too many arguments |

---

## Installation

**Dependencies:** `gcc`, `make`, `readline`

```bash
# Clone the repository
git clone git@github.com:opheliamarboeuf/minishell.git minishell
cd minishell

# Build
make
```

| Make target | Effect |
|---|---|
| `make` / `make all` | Build `libft` and link the `minishell` binary |
| `make clean` | Remove all object files |
| `make fclean` | Remove object files + binary + libft |
| `make re` | `fclean` then full rebuild |

---

## Usage

```bash
./minishell
```

Launch minishell and interact with it like a regular shell:

```bash
minishell$ echo "Hello, World!"
Hello, World!

minishell$ export MY_VAR=42
minishell$ echo $MY_VAR
42

minishell$ ls -la | grep ".c" | wc -l
42

minishell$ cat << EOF
> heredoc content
> EOF

minishell$ exit
```
