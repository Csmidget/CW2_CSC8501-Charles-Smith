#pragma once

#include <vector>
#include <string>
#include "Coord.h"
#include "Player.h"
#include "Cell.h"

class Maze
{
	friend Maze ReadMazeFromFile(std::string fileName);

private:
	int width;
	int height;
	int playerCount;
	Coord finish;

	Cell* map;
	std::vector<Coord> entrances;
	std::vector<Player> activePlayers;

	void GenerateMaze();
	void GenerateEntrances(int _count);
	bool ProcessPlayerTurn(Player& _player);
	void GeneratePlayerPath(Player& _player, const Coord& _finish);
	Cell* operator[](int _index) { return &map[_index * height]; }

public:
	Maze();
	Maze(int _width, int _height, int _entrances, bool _generate);
	Maze(const Maze& _maze); //Copy constructor
	Maze(Maze&& _maze) noexcept; //Move constructor
	~Maze();

	Maze& operator=(const Maze& _maze); //copy assignment
	Maze& operator=(Maze&& _maze) noexcept; //move assignment

	int Height() const { return height; }
	int Width() const { return width; }
	Cell At(int _x, int _y) const { return map[_x * height + _y]; }

	bool IsSolvable() const { return activePlayers.size() > 0; }
	bool InBounds(int _x, int _y) const { return _x >= 0 && _x < width&& _y >= 0 && _y < height; }
	bool OnBoundary(int _x, int _y) const { return _x == 0 || _x == width - 1 || _y == 0 || _y == height - 1; }
	void DisplayInfo() const;
	void RunSolution();
	float AverageStepsToSolve() const;
};

std::ostream& operator<<(std::ostream& _stream, const Maze& _maze);

void WriteMazeToFile(const Maze& _maze, const std::string _fileName, bool _append);
Maze ReadMazeFromFile(std::string fileName);