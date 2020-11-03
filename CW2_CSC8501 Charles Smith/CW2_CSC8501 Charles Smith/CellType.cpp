#include "CellType.h"

CellType CharToCellType(char _char)
{
	for (int i = 0; i < CELLTYPECOUNT; i++)
		if (CELLCHARS[i] == _char)
			return (CellType)i;

	return CellType::Invalid;
}