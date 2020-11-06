#include "Cell.h"

Cell CharToCell(char _char)
{
	for (size_t i{}; i < CELLCOUNT; i++)
		if (CELLCHARS[i] == _char)
			return (Cell)i;

	return Cell::Invalid;
}