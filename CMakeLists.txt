cmake_minimum_required(VERSION 3.16)
project(HangmanGame CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(hangman
    src/hangman.cpp
    src/functions.cpp
    src/db.cpp
    src/console.cpp
    third_party/sqlite3/sqlite3.c
)

target_include_directories(hangman PRIVATE src third_party/sqlite3)

if(UNIX)
    target_link_libraries(hangman PRIVATE pthread dl)
endif()
