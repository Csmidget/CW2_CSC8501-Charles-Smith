#include "MazePathfinder.h"
#include "Maze.h"

MazePathfinder::MazePathfinder(Maze* _maze)
{
	maze = _maze;
}

//Return to default state ready for next request.
void MazePathfinder::Cleanup()
{
	while (openList.size() > 0)
	{
		delete openList.top();
		openList.pop();
	}

	for (auto i = closedList.begin(); i != closedList.end(); i++)
		delete i->second;
	closedList.clear();
}

void MazePathfinder::AddChildrenToOpenList(AStarNode* _node)
{
	const Coord childModifiers[]{ {1,0} ,{0,1},{-1,0},{0,-1} };
	for (int i = 0; i < 4; i++)
	{
		Coord childPos = _node->pos + childModifiers[i];

		if (!maze->InBounds(childPos.x, childPos.y) || maze->At(childPos.x, childPos.y) == Cell::Wall)
			continue;

		int fromStart = _node->fromStart + 1;
		int fromEnd = abs(childPos.x - maze->Finish().x) + abs(childPos.y - maze->Finish().y);
		openList.push(new AStarNode{ childPos,_node,fromStart,fromEnd,fromStart + fromEnd });
	}
}

std::vector<Coord> MazePathfinder::ConstructPathToFinish(const Coord& _from)
{
	Coord curr = _from;
	std::vector<Coord> path;

	if (solvedPaths.find(curr) != solvedPaths.end())
		while (curr != maze->Finish())
		{
			path.push_back(solvedPaths[curr]);
			curr = solvedPaths[curr];
		}

	return path;
}

void MazePathfinder::AddSolvedPath(const AStarNode* _startNode)
{
	auto currentNode = _startNode;
	while (currentNode->cameFrom != nullptr)
	{
		solvedPaths[currentNode->cameFrom->pos] = currentNode->pos;
		currentNode = currentNode->cameFrom;
	}
}

std::vector<Coord> MazePathfinder::Solve(Coord _from)
{
	openList.push(new AStarNode{ _from,nullptr,0,0,0 });

	while (openList.size() > 0)
	{
		AStarNode* curr = openList.top();
		openList.pop();
		
		if (closedList.find(curr->pos) != closedList.end())
		{
			delete curr;
			continue;
		}

		closedList[curr->pos] = curr;

		//Reusing existing paths saves on wasting time retreading the same ground
		if (curr->pos == maze->Finish() || solvedPaths.find(curr->pos) != solvedPaths.end())
		{
			AddSolvedPath(curr);
			break;
		}

		AddChildrenToOpenList(curr);
	}

	std::vector<Coord> path = ConstructPathToFinish(_from);

	Cleanup();

	return path;
}