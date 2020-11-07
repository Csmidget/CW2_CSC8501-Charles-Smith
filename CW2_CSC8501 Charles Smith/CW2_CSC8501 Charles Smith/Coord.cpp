#include "Coord.h"

Coord& Coord::operator+=(const Coord& _rhs)
{
	x += _rhs.x;
	y += _rhs.y;
	return *this;
}

Coord& Coord::operator-=(const Coord& _rhs)
{
	x -= _rhs.x;
	y -= _rhs.y;
	return *this;
}

bool operator==(const Coord& _lhs, const Coord& _rhs)
{
	return (_lhs.x == _rhs.x && _lhs.y == _rhs.y);
}

bool operator!=(const Coord& _lhs, const Coord& _rhs)
{
	return !(_lhs == _rhs);
}

bool operator<(const Coord& _lhs, const Coord& _rhs)
{
	return _lhs.x == _rhs.x ? _lhs.y < _rhs.y : _lhs.x < _rhs.x;
}

Coord operator+(Coord _lhs, const Coord& _rhs)
{
	return _lhs += _rhs;
}

Coord operator-(Coord _lhs, const Coord& _rhs)
{
	return _lhs -= _rhs;
}

std::ostream& operator<<(std::ostream& _outStream, const Coord& _coord)
{
	return _outStream << _coord.x << "," << _coord.y;
}