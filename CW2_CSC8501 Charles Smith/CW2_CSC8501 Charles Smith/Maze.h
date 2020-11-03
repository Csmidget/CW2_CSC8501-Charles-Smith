#pragma once

#include <vector>
#include "Coord.h"
#include "Player.h"
#include "CellType.h"

class Maze
{
private:
	int width;
	int height;
	Coord finish;

	CellType* map;
	std::vector<Coord> entrances;
	std::vector<Player> activePlayers;

	void GenerateMaze();
	void GenerateEntrances(int _count);
	bool ProcessPlayerTurn(Player& _player);
	std::vector<Coord> GeneratePath(const Coord& _start, const Coord& _finish);
	CellType* operator[](int _index) { return &map[_index * width]; }

	friend Maze ReadMazeFromFile();

public:
	Maze(int _width, int _height, int _entrances, bool _generate);
	Maze(const Maze&); // Copy constructor
	Maze(Maze&&) noexcept; //Move constructor
	~Maze();

	int Height() const { return height; }
	int Width() const { return width; }
	CellType At(int _x, int _y) const { return map[_x * width + _y]; }

	bool InBounds(int _x, int _y) const { return _x >= 0 && _x < width&& _y >= 0 && _y < height; }
	bool OnBoundary(int _x, int _y) const { return _x == 0 || _x == width - 1 || _y == 0 || _y == height - 1; }
	void RunSolution();
};

void PrintMaze(const Maze& _maze);
void WriteMazeToFile(const Maze& _maze);
Maze ReadMazeFromFile();