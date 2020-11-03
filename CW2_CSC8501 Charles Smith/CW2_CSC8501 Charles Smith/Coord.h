#pragma once
#include <ostream>

struct Coord
{
	int x;
	int y;

	Coord& operator+=(const Coord& _rhs);
};

bool operator==(const Coord& _lhs, const Coord& _rhs);
bool operator!=(const Coord& _lhs, const Coord& _rhs);
Coord operator+(Coord _lhs, const Coord& _rhs);
std::ostream& operator<<(std::ostream& os, const Coord& dt);