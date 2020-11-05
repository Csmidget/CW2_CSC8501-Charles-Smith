#include "Cell.h"

Cell CharToCell(char _char)
{
	for (int i = 0; i < CellCOUNT; i++)
		if (CELLCHARS[i] == _char)
			return (Cell)i;

	return Cell::Invalid;
}