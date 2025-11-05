# Simple OS Lab Project: File Explorer (C++)

An interactive, menu-based C++ File Explorer that lets you navigate directories and preview text files.

Works on Windows (and most platforms) using only the C++17 standard library (`std::filesystem`).

## How to run (Code Runner)

- Open `main.cpp` in VS Code.
- Use Code Runner to run the active file.
- If prompted for a compiler standard, choose C++17 or newer.

When it starts, it launches the File Explorer directly.

## Features

- Lists current directory contents with indexes.
- Open a directory by its number.
- View a file (prints up to 200 lines; skips likely-binary files).
- Go up to parent directory.
- Change directory by typing a path.

## Notes
- Requires a C++17-capable compiler.
- Uses only standard C++ headers; no external dependencies.
- Binary files are not printed (a small heuristic suppresses output if the file looks binary).

## File to run
Run `main.cpp`.
