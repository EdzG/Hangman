#pragma once
#include <sqlite3.h>
#include <string>

// Opens (creating and seeding it first, if it doesn't exist yet) the
// SQLite database at `path`. Returns nullptr on failure.
sqlite3* db_open(const std::string& path);
