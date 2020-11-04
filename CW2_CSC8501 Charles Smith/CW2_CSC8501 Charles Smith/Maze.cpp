#include <iostream>
#include <algorithm>
#include <queue>
#include <fstream>
#include "Maze.h"
#include "Helpers.h"
#include <string>

void Maze::GeneratePlayerPath(Player& _player, const Coord& _finish)
{
	_player.path = {};

	struct AStarNode
	{
		Coord pos;
		AStarNode* cameFrom;
		int fromStart;
		int fromEnd;
		int totalCost;
	};

	struct AStarCompare
	{
		bool operator()(AStarNode*& lhs, AStarNode*& rhs) { return lhs->totalCost > rhs->totalCost; }
	};

	std::priority_queue<AStarNode*, std::vector<AStarNode*>, AStarCompare> openNodes{};
	std::vector<AStarNode*> closedNodes{};

	AStarNode* endNode{ nullptr };

	openNodes.push(new AStarNode{ _player.pos,nullptr,0,0,0 });

	while (openNodes.size() > 0)
	{
		AStarNode* curr = openNodes.top();
		openNodes.pop();

		if (std::find_if(closedNodes.begin(), closedNodes.end(), [curr](const AStarNode* val) { return val->pos == curr->pos; }) != closedNodes.end())
		{
			delete curr;
			continue;
		}

		closedNodes.push_back(curr);

		//Reusing existing paths saves on wasting time retreading the same ground
		if (curr->pos == _finish)
		{
			endNode = curr;
			break;
		}

		const Coord childModifiers[]{ {1,0} ,{0,1},{-1,0},{0,-1} };
		for (int i = 0; i < 4; i++)
		{
			Coord childPos = curr->pos + childModifiers[i];

			if (!InBounds(childPos.x, childPos.y) || (*this)[childPos.x][childPos.y] == CellType::Wall)
				continue;

			int fromStart = curr->fromStart + 1;
			int fromEnd = abs(childPos.x - finish.x) + abs(childPos.y - finish.y);
			openNodes.push(new AStarNode{ childPos,curr,fromStart,fromEnd,fromStart + fromEnd });
		}
	}

	if (endNode != nullptr)
		while (endNode->cameFrom != nullptr)
		{
			_player.path.push_back(endNode->pos);

			endNode = endNode->cameFrom;
		}
	else
		std::cout << "No valid path to finish for player at: " << _player.pos << "\n";

	while (openNodes.size() > 0)
	{
		delete openNodes.top();
		openNodes.pop();
	}

	for (int i = 0; i < closedNodes.size(); i++)
		delete closedNodes[i];
}

bool Maze::ProcessPlayerTurn(Player& _player)
{
	Coord next = _player.path.back();

	switch ((*this)[next.x][next.y])
	{
	case CellType::Finish:
		(*this)[_player.pos.x][_player.pos.y] = _player.occupiedCellType;
		return true;
	case::CellType::Empty:
		(*this)[_player.pos.x][_player.pos.y] = _player.occupiedCellType;
		_player.pos = next;
		_player.path.pop_back();
		_player.occupiedCellType = (*this)[_player.pos.x][_player.pos.y];
		(*this)[_player.pos.x][_player.pos.y] = CellType::Player;
		break;
	}

	return false;
}

void Maze::RunSolution()
{
	int currPlayer = activePlayers.size() - 1;
	
	bool autosolve = ReceiveYN("Run every turn without prompting input? Warning, this may take a while. (y/n): ");

	do 
	{
		std::cout << "\n";

		if (ProcessPlayerTurn(activePlayers[currPlayer]))
			activePlayers.erase(activePlayers.begin() + currPlayer);

		PrintMaze(*this);

		currPlayer > 0 ? currPlayer-- : currPlayer = activePlayers.size() - 1;

	} while (activePlayers.size() > 0 && (autosolve || ReceiveYN("Continue? (y/n): ")));

	std::cout << "COMPLETE\n\n";
}

void Maze::GenerateEntrances(int _count)
{
	//This solution ensures that no exits will overlap with one another and is of known complexity ( O(n) )
	std::vector<Coord> possibleEntrances;
	possibleEntrances.reserve(width + height - 2);

	for (int i = 1; i < width / 2 + 1; i++)
		possibleEntrances.insert(possibleEntrances.end(), { {i * 2 - 1,0} , {i * 2 - 1,height - 1} });
	for (int i = 1; i < height / 2 + 1; i++)
		possibleEntrances.insert(possibleEntrances.end(), { {0,i * 2 - 1} , {width - 1 ,i * 2 - 1} });

	std::random_shuffle(possibleEntrances.begin(), possibleEntrances.end());

	int entranceCount = std::min(_count, (int)possibleEntrances.size());
	entrances.reserve(entranceCount);

	for (int i = 0; i < entranceCount; i++)
	{
		entrances.push_back(possibleEntrances[i]);

		Player player = { entrances[i],{},CellType::Entrance };
		GeneratePlayerPath(player, finish);

		if (player.path.size() > 0)
			activePlayers.push_back(player);
			
		(*this)[possibleEntrances[i].x][possibleEntrances[i].y] = CellType::Player;
	}
}

void Maze::GenerateMaze()
{
	struct FrontierCell
	{
		Coord loc;
		Coord connector;
	};

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			(*this)[x][y] = CellType::Wall;
	(*this)[1][1] = CellType::Empty;

	std::vector<FrontierCell> frontierCells{};

	frontierCells.push_back({ {3,1},{2,1} });
	frontierCells.push_back({ {1,3},{1,2} });

	while (frontierCells.size() > 0)
	{
		int pos = rand() % frontierCells.size();

		FrontierCell currCell = frontierCells[pos];
		frontierCells.erase(frontierCells.begin() + pos);

		if ((*this)[currCell.loc.x][currCell.loc.y] != CellType::Wall) continue;

		Coord loc = currCell.loc;

		(*this)[loc.x][loc.y] = CellType::Empty;
		(*this)[currCell.connector.x][currCell.connector.y] = CellType::Empty;

		if (loc.x + 2 < width - 1 && (*this)[loc.x + 2][loc.y] == CellType::Wall)
			frontierCells.push_back({ {loc.x + 2, loc.y},{loc.x + 1, loc.y} });

		if (loc.x - 2 > 0 && (*this)[loc.x - 2][loc.y] == CellType::Wall)
			frontierCells.push_back({ {loc.x - 2, loc.y},{loc.x - 1, loc.y} });

		if (loc.y + 2 < height - 1 && (*this)[loc.x][loc.y + 2] == CellType::Wall)
			frontierCells.push_back({ {loc.x, loc.y + 2},{loc.x, loc.y + 1} });

		if (loc.y - 2 > 0 && (*this)[loc.x][loc.y - 2] == CellType::Wall)
			frontierCells.push_back({ {loc.x, loc.y - 2},{loc.x, loc.y - 1} });
	}

	for (int x = -1; x <= 1; x++)
		for (int y = -1; y <= 1; y++)
			(*this)[finish.x + x][finish.y + y] = CellType::Empty;
	(*this)[finish.x][finish.y] = CellType::Finish;
}

Maze::Maze(int _width, int _height, int _entrances, bool _generate)
{

	height = _height;
	width = _width;

	finish = { width / 2,height / 2 };
	map = new CellType [width * height];

	if (_generate)
	{
		GenerateMaze();
		GenerateEntrances(_entrances);
	}
}

//Copy Constructor
Maze::Maze(const Maze& _maze)
{
	height = _maze.height;
	width = _maze.width;
	finish = _maze.finish;

	activePlayers = _maze.activePlayers;
	entrances = _maze.entrances;

	map = new CellType [width * height];

    memcpy(map, _maze.map, sizeof(CellType) * width * height);
}

//Move constructor
Maze::Maze(Maze&& _maze) noexcept
{
	width = _maze.width;
	height = _maze.height;
	finish = _maze.finish;

	map = _maze.map;
	activePlayers = _maze.activePlayers;
	entrances = _maze.entrances;

	_maze.height = 0;
	_maze.width = 0;
	_maze.map = nullptr;
}

Maze::~Maze()
{
	delete[] map;
}

void PrintMaze(const Maze& _maze)
{
	for (int y = 0; y < _maze.Height(); y++)
	{
		for (int x = 0; x < _maze.Width(); x++)
			std::cout << (CELLCHARS[(int)_maze.At(x, y)]);
		std::cout << "\n";
	}
	std::cout << "\n";
}

void WriteMazeToFile(const Maze& _maze)
{
	std::string fileName;

	bool validFile = false;

	while (!validFile)
	{
		fileName=  ReceiveFileName();

		validFile = FileExists(fileName) ? ReceiveYN("File already exists. Overwrite? (y/n): ") : true;
	}

	std::ofstream file;
	file.open(fileName);
	file.clear();

	for (int x = 0; x < _maze.Width(); x++)
		file << CELLCHARS[(int)_maze.At(x, 0)];

	for (int y = 1; y < _maze.Height(); y++)
	{
		file << "\n";
		for (int x = 0; x < _maze.Width(); x++)
			file << CELLCHARS[(int)_maze.At(x, y)];
	}

	file.close();
}

//Factory function to generate maze from file.
Maze ReadMazeFromFile()
{
	std::string fileName;
	
	do
		fileName = ReceiveFileName();
	while (!FileExists(fileName));

		
	std::ifstream file;
	std::string line;
	file.open(fileName);

	if (file.fail())
		throw std::exception("Unable to open file.");

	getline(file, line);

	int width = line.size();
	int height{ 1 };

	while (!file.eof())
	{
		std::getline(file, line);
		height++;
	}
	Maze maze = Maze(width, height, 0, false);

	file.clear();
	file.seekg(0);

	char inChar;
	int currLine{ 0 };
	int currChar{ 0 };

	std::vector<Player> players;

	for (int y = 0; y < height; y++)
	{
		getline(file, line);

		for (int x = 0; x < line.size(); x++)
		{
			CellType cell = CharToCellType(line[x]);
			maze[x][y] = cell;

			switch (cell)
			{
			case CellType::Entrance:
				maze.entrances.push_back({ x,y });
				break;
			case CellType::Player:
				players.push_back({ {x,y},{},maze.OnBoundary(x,y) ? CellType::Entrance : CellType::Empty });
				break;
			case CellType::Finish:
				maze.finish = { x,y };
				break;
			}
		}
	}

	for (int i = 0; i < players.size(); i++)
	{
		maze.GeneratePlayerPath(players[i], maze.finish);

		if (players[i].path.size() > 0)
			maze.activePlayers.push_back(players[i]);
	}

	file.close();

	return maze;
}