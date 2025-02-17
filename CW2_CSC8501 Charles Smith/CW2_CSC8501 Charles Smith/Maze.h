#pragma once

#include "MazePathFinder.h"
#include "Coord.h"
#include "Cell.h"

#include <string>
#include <vector>

// HEADER SUMMARY
// The core of the program. Manages maze state and controls
// maze generation, pathfinding and display.

enum class TurnResult {Finished, Blocked, Moved, Invalid};

class Maze
{
private:

	friend Maze ReadMazeFromFile(const std::string& fileName);

	struct Player
	{
		Coord pos;
		std::vector<Coord> path;
		Cell occupiedCell;
	};

	int width;
	int height;
	int playerCount;
	Coord finish;
	MazePathfinder pathfinder;

	Cell* map;
	std::vector<Coord> entrances;
	std::vector<Player> activePlayers;

	void GenerateMaze();
	void GenerateEntrances(size_t _count);
	TurnResult ProcessPlayerTurn(Player& _player);
	void GeneratePlayerPath(Player& _player);
	Cell* operator[](size_t _index) { return &map[_index * height]; }
	Cell& operator[](Coord _xy) { return map[_xy.x * height + _xy.y]; }

public:
	Maze();
	Maze(int _width, int _height, int _entrances, bool _generate);
	Maze(const Maze& _maze); //Copy constructor
	Maze(Maze&& _maze) noexcept; //Move constructor
	~Maze();

	Maze& operator=(const Maze& _maze); //Copy Assignment
	Maze& operator=(Maze&& _maze) noexcept; //Move Assignment

	int Height() const { return height; }
	int Width() const { return width; }
	Coord Finish() const { return finish; }
	Cell At(size_t _x, size_t _y) const { return map[_x * height + _y]; }
	Cell At(Coord _xy) const { return map[_xy.x * height + _xy.y]; }

	int PathfindingReusedNodes() const { return pathfinder.reusedNodeCount; }
	bool IsSolvable() const { return activePlayers.size() > 0; }
	bool IsPartiallySolvable() const { return activePlayers.size() > 0 && activePlayers.size() != playerCount; }
	bool IsFullySolvable() const { return activePlayers.size() > 0 && activePlayers.size() == playerCount;; }
	bool InBounds(Coord _xy) const { return _xy.x >= 0 && _xy.x < width && _xy.y >= 0 && _xy.y < height; }
	bool OnBoundary(int _x, int _y) const { return _x == 0 || _x == width - 1 || _y == 0 || _y == height - 1; }
	void DisplayInfo() const;
	float AverageStepsToSolve() const;

	//This is the only public function that can change the mazes state (Except for constructors and assignment operators)
	void RunSolution();
};

std::ostream& operator<<(std::ostream& _stream, const Maze& _maze);

void WriteMazeToFile(const Maze& _maze,const std::string& _fileName, bool _append);
Maze ReadMazeFromFile(const std::string& _fileName);