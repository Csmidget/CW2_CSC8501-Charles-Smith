#pragma once

#include "Coord.h"

#include <map>
#include <vector>
#include <queue>

class Maze;

class MazePathfinder
{
	friend class Maze;
	
	//Private so only Maze can construct this class.
	MazePathfinder(Maze* _maze);

	struct AStarNode
	{
		Coord pos;
		AStarNode* cameFrom;
		int fromStart;
		int fromEnd;
		int totalCost;
	};

	//Functor for comparing a star node's values.
	struct AStarWeightCompare { bool operator()(AStarNode*& _lhs, AStarNode*& _rhs) { return _lhs->totalCost > _rhs->totalCost; } };

	//The maze that owns this pathfinder.
	Maze* maze;

	//Nodes with known path to finish
	std::map<Coord, Coord> solvedPaths;

	void Cleanup();
	void AddChildrenToOpenList(AStarNode* _node);
	void AddSolvedPath(const AStarNode* _startNode);
	std::vector<Coord> ConstructPathToFinish(const Coord& _from);

	std::map<Coord,AStarNode*> closedList;
	std::priority_queue<AStarNode*, std::vector<AStarNode*>, AStarWeightCompare> openList;

public:
	std::vector<Coord> operator()(Coord _start);

	//No default constructor
	MazePathfinder() = delete;
};