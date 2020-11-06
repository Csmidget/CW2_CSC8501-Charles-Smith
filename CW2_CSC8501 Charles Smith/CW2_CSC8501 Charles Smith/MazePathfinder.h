#pragma once

#include <vector>
#include <map>
#include <queue>
#include "Coord.h"

class Maze;

class MazePathfinder
{
	friend class Maze;

	//This is private so that only the Maze class can construct the pathfinder class.
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

	std::vector<Coord> ConstructPathToFinish(const Coord& _from);
	void AddChildrenToOpenList(AStarNode* _node);
	void Cleanup();
	void AddSolvedPath(const AStarNode* _startNode);

	std::map<Coord,AStarNode*> closedList;
	std::priority_queue<AStarNode*, std::vector<AStarNode*>, AStarWeightCompare> openList;

public:
	std::vector<Coord> Solve(Coord _start);
};