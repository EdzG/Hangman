# Hangman

A cross-platform console Hangman game in C++ with a SQLite-backed word bank.
Words and their definitions are stored in a database and organized into sets
("word sets"); players pick a set, guess letters against a randomly chosen
word from it, and can create their own word sets from the game menu.

Originally built as the final project for the Programming II course at
National Dong Hwa University (freshman year, second semester). The full
project report is in [`docs/final-project-report.pdf`](docs/final-project-report.pdf).

## Features

- Full interactive terminal UI: arrow-key/Enter menu navigation, live letter
  keypresses during gameplay (no Enter needed), color-coded alphabet
  (green = correct, red = wrong) and a stage-tinted hangman drawing
  (white → yellow → red as guesses run out)
- Words pulled at random (without repeats) from a SQLite database
- Each word comes with a definition shown as a hint
- Multiple word sets, selectable from the start menu
- In-game wizard for creating a new word set and populating it with words
- Zero setup for the database: it's created and seeded automatically on
  first run

## Tech stack

- C++17
- [FTXUI](https://github.com/ArthurSonzogni/ftxui) (fetched via CMake
  FetchContent) for the terminal UI
- [SQLite](https://www.sqlite.org/) (vendored amalgamation, no external DB server)
- CMake

## Project structure

```
src/                        Game source (functions.h/.cpp, hangman.cpp, db.h/.cpp)
sql/schema.sql               Database schema + seed data (reference copy; embedded in src/db.cpp)
sql/schema_diagram.dbml      DBML schema diagram (view at dbdiagram.io)
docs/                        Original project report
third_party/sqlite3/         Vendored SQLite amalgamation
CMakeLists.txt                Build definition
```

## Building

Requires a C++17 compiler and CMake 3.16+. No database server to install —
but the **first** `cmake -B build` needs network access, since it fetches
FTXUI via CMake's `FetchContent` (subsequent builds are fully offline).

```sh
cmake -B build
cmake --build build
./build/hangman        # Windows: build\Debug\hangman.exe (or Release, per generator)
```

The first run creates `hangman.db` in the working directory and seeds it
with a starter word set; subsequent runs reuse it.

## Playing

- **Menus** — arrow keys (or `j`/`k`) to move, Enter to select.
- **START** — pick a word set, then type letters directly to guess (no
  Enter needed); six wrong guesses ends the game.
- **ABOUT** — shows the game rules; Enter to go back.
- **CREATE WORD SET** — a short wizard: set name, word count, then a
  word + definition for each one.
- **QUIT** — exit.

## License

MIT — see [LICENSE.md](LICENSE.md). The vendored SQLite amalgamation
(`third_party/sqlite3/`) is public domain — see
[`third_party/sqlite3/README.md`](third_party/sqlite3/README.md).
