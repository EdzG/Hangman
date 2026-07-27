-- Schema for the Hangman word bank (SQLite).
--
-- This file is the human-readable reference copy; the game embeds the same
-- statements in src/db.cpp and runs them automatically the first time it
-- starts (when hangman.db doesn't exist yet), so no manual setup is needed.

CREATE TABLE wordSet (
    setId      INTEGER PRIMARY KEY AUTOINCREMENT,
    setName    TEXT NOT NULL,
    numOfWords INTEGER NOT NULL DEFAULT 0
);

CREATE TABLE Words (
    wordId     INTEGER PRIMARY KEY AUTOINCREMENT,
    wordName   TEXT NOT NULL,
    definition TEXT NOT NULL,
    setId      INTEGER NOT NULL REFERENCES wordSet(setId)
);

INSERT INTO wordSet (setName, numOfWords) VALUES ('computer science', 4);

INSERT INTO Words (wordName, definition, setId) VALUES
    ('Computer Science', 'The scientific study of computation, applied to both hardware and software, covering both theoretical and practical concerns.', 1),
    ('complexity', 'The way that a solution to a problem scales as the size of the input increases, considering both the number of computational steps and the memory space required.', 1),
    ('Browser', 'A piece of software that enables a user to locate, retrieve and display information on the world wide web.', 1),
    ('Function', 'A small section of computational code that performs a specific operation. In particular, a function takes inputs, or arguments, and returns outputs, or results.', 1);
