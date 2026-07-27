# Hangman

A cross-platform console Hangman game in C++ with a SQLite-backed word bank.
Words and their definitions are stored in a database and organized into sets
("word sets"); players pick a set, guess letters against a randomly chosen
word from it, and can create their own word sets from the game menu.

Originally built as the final project for the Programming II course at
National Dong Hwa University (freshman year, second semester). The full
project report is in [`docs/final-project-report.pdf`](docs/final-project-report.pdf).

## Features

- Classic Hangman gameplay with ASCII-art stages drawn to the console
- Words pulled at random (without repeats) from a SQLite database
- Each word comes with a definition shown as a hint
- Multiple word sets, selectable from the start menu
- In-game flow for creating a new word set and populating it with words
- Zero setup: the database is created and seeded automatically on first run

## Tech stack

- C++17
- [SQLite](https://www.sqlite.org/) (vendored amalgamation, no external DB server)
- CMake

## Project structure

```
src/                        Game source (functions.h/.cpp, hangman.cpp, db.h/.cpp, console.h/.cpp)
sql/schema.sql               Database schema + seed data (reference copy; embedded in src/db.cpp)
sql/schema_diagram.dbml      DBML schema diagram (view at dbdiagram.io)
docs/                        Original project report
third_party/sqlite3/         Vendored SQLite amalgamation
CMakeLists.txt                Build definition
```

## Building

Requires a C++17 compiler and CMake 3.16+ — no external dependencies, no
database server to install.

```sh
cmake -B build
cmake --build build
./build/hangman        # Windows: build\Debug\hangman.exe (or Release, per generator)
```

The first run creates `hangman.db` in the working directory and seeds it
with a starter word set; subsequent runs reuse it.

## Playing

- **START** — pick a word set, then guess letters one at a time; six wrong
  guesses ends the game.
- **ABOUT** — shows the game rules.
- **CREATE WORD SET** — add a new set of words and definitions to the
  database from inside the game.
- **QUIT** — exit.

## License

MIT — see [LICENSE.md](LICENSE.md). The vendored SQLite amalgamation
(`third_party/sqlite3/`) is public domain — see
[`third_party/sqlite3/README.md`](third_party/sqlite3/README.md).
