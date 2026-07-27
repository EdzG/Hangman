cmake_minimum_required(VERSION 3.16)
project(HangmanGame CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG v5.0.0)
FetchContent_MakeAvailable(ftxui)

add_executable(hangman
    src/hangman.cpp
    src/functions.cpp
    src/db.cpp
    third_party/sqlite3/sqlite3.c
)

target_include_directories(hangman PRIVATE src third_party/sqlite3)

target_link_libraries(hangman
    PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
)

if(UNIX)
    target_link_libraries(hangman PRIVATE pthread dl)
endif()
