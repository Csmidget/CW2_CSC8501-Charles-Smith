#pragma once

// HEADER SUMMARY
// This header allows quick manipulation of how mazes are displayed and saved.
// Changing the characters in the CELLCHARS array will automatically update
// how the program reads and writes mazes.

const int CELLCOUNT = 6;
const char CELLCHARS[CELLCOUNT] = { ' ','#','F','E','P','?' };

enum class Cell { Empty = 0, Wall = 1, Finish = 2, Entrance = 3, Player = 4, Invalid = 5 };

Cell CharToCell(char _char);