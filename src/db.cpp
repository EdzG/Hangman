#include "db.h"
#include <filesystem>
#include <iostream>

// Mirrors sql/schema.sql — see that file for the human-readable copy.
static const char* SCHEMA_SQL = R"SQL(
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
)SQL";

sqlite3* db_open(const std::string& path) {
	bool needsSeed = !std::filesystem::exists(path);

	sqlite3* db = nullptr;
	if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
		std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return nullptr;
	}

	sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

	if (needsSeed) {
		char* errMsg = nullptr;
		if (sqlite3_exec(db, SCHEMA_SQL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
			std::cerr << "Failed to initialize database: " << errMsg << std::endl;
			sqlite3_free(errMsg);
			sqlite3_close(db);
			return nullptr;
		}
	}

	return db;
}
