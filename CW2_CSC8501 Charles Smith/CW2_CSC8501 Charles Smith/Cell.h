#pragma once

const int CellCOUNT = 6;
const char CELLCHARS[CellCOUNT] = { ' ','#','F','E','P','?' };

enum class Cell { Empty = 0, Wall = 1, Finish = 2, Entrance = 3, Player = 4, Invalid = 5 };

Cell CharToCell(char _char);