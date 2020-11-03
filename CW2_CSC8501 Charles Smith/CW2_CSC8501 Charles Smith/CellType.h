#pragma once

const int CELLTYPECOUNT = 6;
const char CELLCHARS[CELLTYPECOUNT] = { ' ','#','F','E','P','?' };

enum class CellType { Empty = 0, Wall = 1, Finish = 2, Entrance = 3, Player = 4, Invalid = 5 };

CellType CharToCellType(char _char);