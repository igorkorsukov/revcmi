# AGENTS.md

Guidance for coding agents working in this repository. VCMI-specific guidelines live in [`vcmi/AGENTS.md`](vcmi/AGENTS.md).

## Git

- Never run `git commit`, `git push`, `git reset`, `git rebase`, `git stash` or any other command that modifies git history or the index on your own initiative.
- Leave all changes uncommitted; the user reviews and commits them.
- Run state-changing git commands only when the user explicitly asks for it in the current message.
- Read-only commands (`git status`, `git diff`, `git log`, `git show`) are allowed.
