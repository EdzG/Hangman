#pragma once

// Small cross-platform console shim. This is the only place in the codebase
// that needs to know what platform it's on.

void console_init();    // enables ANSI escape sequence support (Windows only; no-op elsewhere)
void clear_screen();    // clears the terminal
void pause_console();   // "Press Enter to continue..." and waits for input
