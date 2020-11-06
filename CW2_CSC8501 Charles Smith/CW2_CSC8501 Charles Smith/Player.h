#pragma once

struct Player
{
	Coord pos;
	std::vector<Coord> path;
	Cell occupiedCell;
};