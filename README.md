# Hangman

A console Hangman game for Windows, written in C++ with a MySQL-backed word
bank. Words and their definitions are stored in a database and organized
into sets ("word sets"); players pick a set, guess letters against a
randomly chosen word from it, and can create their own word sets from the
game menu.

Originally built as the final project for the Programming II course at
National Dong Hwa University (freshman year, second semester). The full
project report is in [`docs/final-project-report.pdf`](docs/final-project-report.pdf).

## Features

- Classic Hangman gameplay with ASCII-art stages drawn to the console
- Words pulled at random (without repeats) from a MySQL database
- Each word comes with a definition shown as a hint
- Multiple word sets, selectable from the start menu
- In-game flow for creating a new word set and populating it with words

## Tech stack

- C++ (Visual Studio / MSVC, Windows console app)
- MySQL, via [MySQL Connector/C++](https://dev.mysql.com/downloads/connector/cpp/) (JDBC-style API)

## Project structure

```
src/                  Game source (functions.h/.cpp, hangman.cpp, db.cpp)
src/db_config.h.example   Template for local DB credentials (copy to db_config.h)
sql/schema.sql        Database schema + seed data
sql/schema_diagram.dbml   DBML schema diagram (view at dbdiagram.io)
docs/                  Original project report
HangmanGame.sln/.vcxproj  Visual Studio project files
```

## Building

Requires Windows, Visual Studio 2022 (or later) with the "Desktop
development with C++" workload, a running MySQL server, and MySQL
Connector/C++ 8.4.

1. **Set up the database**
   ```sql
   -- in a MySQL client, run:
   source sql/schema.sql
   ```
2. **Configure credentials**
   ```
   copy src\db_config.h.example src\db_config.h
   ```
   Edit `src\db_config.h` with your MySQL host/user/password. This file is
   gitignored so your credentials never get committed.
3. **Point the project at MySQL Connector/C++**
   Open `HangmanGame.sln`, then either define a `MYSQL_CONNECTOR_CPP_DIR`
   user/environment variable pointing at your Connector/C++ install (e.g.
   `C:\mysql-connector-c++-8.4.0-winx64`), or edit the project's
   VC++ Directories / Additional Include Directories directly to match
   where you installed it.
4. Build and run (Release|x64 recommended).

## Playing

- **START** — pick a word set, then guess letters one at a time; six wrong
  guesses ends the game.
- **ABOUT** — shows the game rules.
- **CREATE WORD SET** — add a new set of words and definitions to the
  database from inside the game.
- **QUIT** — exit.

## License

MIT — see [LICENSE.md](LICENSE.md).
