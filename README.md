## Unix Shell (`osh`)
A simple Unix-like shell written in C for educational purposes. This project replicates essential shell features such as command execution, input/output redirection, piping, background processes, and command history.

---

## 📂 Repository Structure

```
Unix_shell/
├── src/
│   └── osh.c              # Main shell source code
├── bin/
│   └── osh                # Compiled binary (after make)
├── unix_shell_report.tex  # Project report in LaTeX
├── Makefile               # Build instructions
```

---

## How to Compile

1. Clone the repository:
```bash
git clone https://github.com/<your-username>/Unix_shell.git
cd Unix_shell
```

2. Compile using the provided Makefile:
```bash
make
```

The compiled shell binary will be located in `bin/osh`.

---

## How to Run

```bash
./bin/osh
```

You’ll be greeted with:
```
osh>
```

---

## Features Implemented

- ✅ Command execution (`ls`, `cat`, `echo`, etc.)
- ✅ Built-in commands: `cd`, `exit`
- ✅ Input `<` and output `>` redirection
- ✅ Piping with `|`
- ✅ Background execution using `&`
- ✅ Command history recall using `!!`

---

## Example Commands

```bash
osh> ls -l
osh> echo hello > file.txt
osh> cat < file.txt
osh> ls | grep .c
osh> sleep 5 &
osh> !!
osh> cd src
osh> exit
```

---

## Notes

- Only one redirection (`<` or `>`) is supported per command.
- Only a single pipe (`|`) is supported per command line.
- `cd` and `exit` are handled as built-in commands (not through `execvp`).
- `!!` recalls and executes the most recent non-empty command.
- `&` executes a command in the background (without waiting for it to finish).

---

## Report

The full LaTeX report is provided in `unix_shell_report.tex`, including implementation details, pseudocode, analysis, and screenshots demonstrating functionality.

