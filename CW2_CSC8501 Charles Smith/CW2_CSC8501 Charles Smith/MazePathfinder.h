#pragma once

#include "Coord.h"

#include <map>
#include <vector>
#include <queue>

class Maze;

// HEADER SUMMARY
// This functor runs A Star pathfinding to calculate routes to a mazes finishing point.
// As it always calculates the shortest route from a point to the finish, it caches any
// routes it calculates for future use.
// 
// As all players are moving to the same point in the maze, it is impossible for deadlock
// situations to occur where players endlessly block one another.
// 
// If it is impossible for a player to reach the finish, no route will be created and the
// player will remain static.
//
//Can only be instantiated by the Maze class and is stored as a member of the Maze, ensuring  
//the pointer back to the maze is always valid (Outside of some hackery).

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

	//Purely used for analysis purposes. Slows pathfinding down slightly.
	int reusedNodeCount;
	void CountReusedNodes(Coord _start);

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

	//No default constructors
	MazePathfinder() = delete;
	MazePathfinder(MazePathfinder&) = delete;
	MazePathfinder(MazePathfinder&&) = delete;

	MazePathfinder& operator=(const MazePathfinder& _maze); //Copy Assignment
	MazePathfinder& operator=(MazePathfinder&& _maze) noexcept; //Move Assignment
};