#pragma once
#include <ostream>

// HEADER SUMMARY
// This is essentially a 2d vector with some basic operator overloads to allow
// easy storage and manipulation of maze co-ordinates.

struct Coord
{
	int x;
	int y;

	Coord& operator+=(const Coord& _rhs);
	Coord& operator-=(const Coord& _rhs);
};

bool operator==(const Coord& _lhs, const Coord& _rhs);
bool operator!=(const Coord& _lhs, const Coord& _rhs);
bool operator<(const Coord& _lhs, const Coord& _rhs);
Coord operator+(Coord _lhs, const Coord& _rhs);
Coord operator-(Coord _lhs, const Coord& _rhs);
std::ostream& operator<<(std::ostream& os, const Coord& dt);