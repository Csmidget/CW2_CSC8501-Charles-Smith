#pragma once

#include <vector>
#include "Coord.h"                                                                                   
#include "Cell.h"

struct Player
{
	Coord pos;
	std::vector<Coord> path;
	Cell occupiedCell;
};