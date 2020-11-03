#pragma once

#include <vector>
#include "Coord.h"                                                                                   
#include "CellType.h"

struct Player
{
	Coord pos;
	std::vector<Coord> path;
	CellType occupiedCellType;
};