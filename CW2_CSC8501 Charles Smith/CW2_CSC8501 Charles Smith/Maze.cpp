#include <iostream>
#include <algorithm>
#include <queue>
#include <fstream>
#include "Maze.h"
#include "Helpers.h"
#include "MazePathfinder.h"

//Blank maze
Maze::Maze() : pathfinder(this)
{
	height = 0;
	width = 0;
	playerCount = 0;
	finish = {};
	map = nullptr;
}

Maze::Maze(int _width, int _height, int _entrances, bool _generate) : pathfinder(this)
{
	height = _height;
	width = _width;
	playerCount = 0;

	finish = { width / 2,height / 2 };
	map = new Cell[width * height]{};


	if (_generate)
	{
		GenerateMaze();
		GenerateEntrances(_entrances);
	}
}

Maze::Maze(const Maze& _maze) : pathfinder(this)
{
	height = _maze.height;
	width = _maze.width;
	finish = _maze.finish;
	playerCount = _maze.playerCount;


	activePlayers = _maze.activePlayers;
	entrances = _maze.entrances;

	map = new Cell[width * height];

	memcpy(map, _maze.map, sizeof(Cell) * width * height);
}

//Move constructor
Maze::Maze(Maze&& _maze) noexcept : pathfinder(this)
{
	(*this) = std::move(_maze);
}

Maze::~Maze()
{
	delete[] map;
}

Maze& Maze::operator=(const Maze& _maze)
{
	delete[] map;

	height = _maze.height;
	width = _maze.width;
	finish = _maze.finish;
	playerCount = _maze.playerCount;

	activePlayers = _maze.activePlayers;
	entrances = _maze.entrances;

	map = new Cell[width * height];

	memcpy(map, _maze.map, sizeof(Cell) * width * height);

	return *this;
}

Maze& Maze::operator=(Maze&& _maze) noexcept
{
	width = _maze.width;
	height = _maze.height;
	finish = _maze.finish;
	playerCount = _maze.playerCount;

	Cell* oldMap = map;
	map = _maze.map;
	activePlayers = _maze.activePlayers;
	entrances = _maze.entrances;

	_maze.height = 0;
	_maze.width = 0;
	_maze.map = oldMap;

	return *this;
}

std::ostream& operator<<(std::ostream& _stream, const Maze& _maze)
{
	for (int y = 0; y < _maze.Height(); y++)
	{
		for (int x = 0; x < _maze.Width(); x++)
			_stream << (CELLCHARS[(int)_maze.At(x, y)]);
		_stream << "\n";
	}
	_stream << "\n";

	return _stream;
}

void Maze::GeneratePlayerPath(Player& _player)
{
	_player.path = pathfinder.Solve(_player.pos);

	if (_player.path.size() > 0)
		activePlayers.push_back(_player);
	else
		std::cout << "No valid path for player at: " << _player.pos;
}

//Returns true if player reaches the goal
bool Maze::ProcessPlayerTurn(Player& _player)
{
	Coord next = _player.path.front();

	switch ((*this)[next.x][next.y])
	{
	case Cell::Finish:
		(*this)[_player.pos.x][_player.pos.y] = _player.occupiedCell;
		return true;
	case::Cell::Empty:
		(*this)[_player.pos.x][_player.pos.y] = _player.occupiedCell;
		_player.pos = next;
		_player.path.erase(_player.path.begin());
		_player.occupiedCell = (*this)[_player.pos.x][_player.pos.y];
		(*this)[_player.pos.x][_player.pos.y] = Cell::Player;
		break;
	case::Cell::Player:
		std::cout << "Player at " << _player.pos << " is currently blocked, skipping turn.\n";
		break;
	}

	return false;
}

void Maze::RunSolution()
{
	int currPlayer = activePlayers.size() - 1;

	bool toFile = ReceiveYN("Write solution to file as it progresses? (y/n): ");

	std::string fileName;

	if (toFile)
	{
		bool validFile = false;

		while (!validFile)
		{
			fileName = ReceiveFileName();

			validFile = FileExists(fileName) ? ReceiveYN("File already exists. Append? (y/n): ") : true;
		}
	}

	bool autosolve = ReceiveYN("Run every turn without prompting input? Warning, this may take a while. (y/n): ");

	bool  go{ true };
	while (activePlayers.size() > 0 && go)
	{
		std::cout << "\n";

		if (ProcessPlayerTurn(activePlayers[currPlayer]))
			activePlayers.erase(activePlayers.begin() + currPlayer);

		std::cout << *this;

		if (toFile)
			WriteMazeToFile(*this, fileName, true);

		currPlayer > 0 ? currPlayer-- : currPlayer = activePlayers.size() - 1;

		go = autosolve || ReceiveYN("Continue? (y/n): ");
	}

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

		Player player = { entrances[i],{},Cell::Entrance };
		GeneratePlayerPath(player);
		playerCount++;

		(*this)[possibleEntrances[i].x][possibleEntrances[i].y] = Cell::Player;
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
			(*this)[x][y] = Cell::Wall;
	(*this)[1][1] = Cell::Empty;

	std::vector<FrontierCell> frontierCells{};

	frontierCells.push_back({ {3,1},{2,1} });
	frontierCells.push_back({ {1,3},{1,2} });

	while (frontierCells.size() > 0)
	{
		int pos = rand() % frontierCells.size();

		FrontierCell currCell = frontierCells[pos];
		frontierCells.erase(frontierCells.begin() + pos);

		if ((*this)[currCell.loc.x][currCell.loc.y] != Cell::Wall) continue;

		Coord loc = currCell.loc;

		(*this)[loc.x][loc.y] = Cell::Empty;
		(*this)[currCell.connector.x][currCell.connector.y] = Cell::Empty;

		if (loc.x + 2 < width - 1 && (*this)[loc.x + 2][loc.y] == Cell::Wall)
			frontierCells.push_back({ {loc.x + 2, loc.y},{loc.x + 1, loc.y} });

		if (loc.x - 2 > 0 && (*this)[loc.x - 2][loc.y] == Cell::Wall)
			frontierCells.push_back({ {loc.x - 2, loc.y},{loc.x - 1, loc.y} });

		if (loc.y + 2 < height - 1 && (*this)[loc.x][loc.y + 2] == Cell::Wall)
			frontierCells.push_back({ {loc.x, loc.y + 2},{loc.x, loc.y + 1} });

		if (loc.y - 2 > 0 && (*this)[loc.x][loc.y - 2] == Cell::Wall)
			frontierCells.push_back({ {loc.x, loc.y - 2},{loc.x, loc.y - 1} });
	}

	for (int x = -1; x <= 1; x++)
		for (int y = -1; y <= 1; y++)
			(*this)[finish.x + x][finish.y + y] = Cell::Empty;
	(*this)[finish.x][finish.y] = Cell::Finish;
}

void WriteMazeToFile(const Maze& _maze, const std::string _fileName, bool _append)
{
	auto base = _append ? std::ios_base::app : std::ios_base::out;

	std::ofstream file;
	file.open(_fileName, base);

	file << _maze;

	file.close();
}

//Factory function to generate maze from file.
Maze ReadMazeFromFile(std::string _fileName)
{
	std::ifstream file;
	std::string line;
	file.open(_fileName);

	if (file.fail())
		throw std::exception("Unable to open file.");

	getline(file, line);

	int width = line.size();
	int height{ 1 };

	while (!file.eof())
	{
		std::getline(file, line);
		if (line.size() > 0)
			height++;
	}
	Maze maze = Maze(width, height, 0, false);

	file.clear();
	file.seekg(0);

	char inChar;
	int currLine{ 0 };
	int currChar{ 0 };

	//We hold on to players for later to generate their paths.
	std::vector<Player> players;

	for (int y = 0; y < height; y++)
	{
		getline(file, line);

		if (line.size() == 0)
			throw std::exception("Error, provided file is not valid maze.");

		for (int x = 0; x < line.size(); x++)
		{
			Cell cell = CharToCell(line[x]);
			maze[x][y] = cell;

			switch (cell)
			{
			case Cell::Entrance:
				maze.entrances.push_back({ x,y });
				break;
			case Cell::Player: 
				maze.playerCount++;
				players.push_back({ {x,y},{},maze.OnBoundary(x,y) ? Cell::Entrance : Cell::Empty });
				break;
			case Cell::Finish:
				maze.finish = { x,y };
				break;
			}
		}
	}

	//Calculate player paths AFTER the maze is fully generated.
	for (int i = 0; i < players.size(); i++)
		maze.GeneratePlayerPath(players[i]);

	file.close();

	return maze;
}

void Maze::DisplayInfo() const
{
	if (playerCount > 0)
		if (activePlayers.size() == 0)
			std::cout << "Maze is unsolvable.\n";
		else
			std::cout << (activePlayers.size() == playerCount ? "Maze is fully solvable\n" : "Maze is partially solvable\n");
	else
		std::cout << "This maze contains no players.\n";

	std::cout << "Average steps to solve: " << AverageStepsToSolve() << "\n";

	std::cout << "\n";
}

float Maze::AverageStepsToSolve() const
{
	int steps{};

	for (int i = 0; i < activePlayers.size(); i++)
		steps += activePlayers[i].path.size();

	activePlayers.empty() ? steps = 0 : steps /= activePlayers.size();

	return steps;
}