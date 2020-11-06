#pragma once

#include <vector>
#include <string>
#include "Coord.h"
#include "Player.h"
#include "Cell.h"
#include "MazePathfinder.h"

class Maze
{
	friend Maze ReadMazeFromFile(std::string fileName);

private:
	int width;
	int height;
	int playerCount;
	Coord finish;
	MazePathfinder pathfinder;

	Cell* map;
	std::vector<Coord> entrances;
	std::vector<Player> activePlayers;

	void GenerateMaze();
	void GenerateEntrances(int _count);
	bool ProcessPlayerTurn(Player& _player);
	void GeneratePlayerPath(Player& _player);
	Cell* operator[](int _index) { return &map[_index * height]; }

public:
	Maze();
	Maze(int _width, int _height, int _entrances, bool _generate);
	Maze(const Maze& _maze); //Copy constructor
	Maze(Maze&& _maze) noexcept; //Move constructor
	~Maze();

	Maze& operator=(const Maze& _maze); //copy Assignment
	Maze& operator=(Maze&& _maze) noexcept; //move Assignment

	int Height() const { return height; }
	int Width() const { return width; }
	Coord Finish() const { return finish; }
	Cell At(int _x, int _y) const { return map[_x * height + _y]; }

	bool IsSolvable() const { return activePlayers.size() > 0; }
	bool IsPartiallySolvable() const { return activePlayers.size() > 0 && activePlayers.size() != playerCount; }
	bool IsFullySolvable() const { return activePlayers.size() > 0 && activePlayers.size() == playerCount;; }
	bool InBounds(int _x, int _y) const { return _x >= 0 && _x < width && _y >= 0 && _y < height; }
	bool OnBoundary(int _x, int _y) const { return _x == 0 || _x == width - 1 || _y == 0 || _y == height - 1; }
	void DisplayInfo() const;
	float AverageStepsToSolve() const;

	//This is the only public function that can change the mazes state.
	void RunSolution();
};

std::ostream& operator<<(std::ostream& _stream, const Maze& _maze);

void WriteMazeToFile(const Maze& _maze, std::string _fileName, bool _append);
Maze ReadMazeFromFile(const std::string fileName);