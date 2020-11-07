#include "Maze.h"

#include "Helpers.h"

#include <iostream>
#include <fstream>

//Blank maze
Maze::Maze() : pathfinder{this}
{
	height = 0;
	width = 0;
	playerCount = 0;
	finish = {};
	map = new Cell[0];
}

Maze::Maze(int _width, int _height, int _entrances, bool _generate) : pathfinder{this}
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

Maze::Maze(const Maze& _maze) : pathfinder{this}
{
	map = nullptr;
	*this = _maze;
}

//Move constructor
Maze::Maze(Maze&& _maze) noexcept : pathfinder{ this }
{
	map = nullptr;
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
	pathfinder = _maze.pathfinder;
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
	pathfinder = _maze.pathfinder;
	Cell* oldMap = map;
	map = _maze.map;

	//Move this mazes old map to the other maze to make sure it is properly deleted in the other mazes deconstructor.
	_maze.map = oldMap;

	activePlayers = std::move(_maze.activePlayers);
	entrances = std::move(_maze.entrances);
	return *this;
}

std::ostream& operator<<(std::ostream& _stream, const Maze& _maze)
{
	for (int y{}; y < _maze.Height(); y++)
	{
		for (int x{}; x < _maze.Width(); x++)
			_stream << (CELLCHARS[(int)_maze.At(x, y)]);
		_stream << "\n";
	}
	_stream << "\n";

	return _stream;
}

void Maze::GeneratePlayerPath(Player& _player)
{
	_player.path = pathfinder(_player.pos);

	if (_player.path.size() > 0)
	{
		int pos{};
		//Placing players in path size order will reduce congestion in heavily populated mazes by ensuring players with
		//fewer steps to the finish move first. (Player turns are processed from the end of the vector to the beginning)
		while (pos != activePlayers.size() && activePlayers[pos].path.size() > _player.path.size())
			pos++;
		activePlayers.insert(activePlayers.begin() + pos, _player);
	}
		
	else
		std::cout << "No valid path for player at: " << _player.pos << ".\n";
}

//Returns true if player reaches the goal
TurnResult Maze::ProcessPlayerTurn(Player& _player)
{
	Coord next{ _player.path.front() };

	switch ((*this)[next])
	{
	case Cell::Finish:
		(*this)[_player.pos] = _player.occupiedCell;
		return TurnResult::Finished;
	case::Cell::Player:
		return TurnResult::Blocked;
	case::Cell::Entrance:
	case::Cell::Empty:
		(*this)[_player.pos] = _player.occupiedCell;
		_player.pos = next;
		_player.path.erase(_player.path.begin());
		_player.occupiedCell = (*this)[_player.pos];
		(*this)[_player.pos] = Cell::Player;
		return TurnResult::Moved;
	}
	
	return TurnResult::Invalid;
}

void Maze::RunSolution()
{
	bool toFile{ ReceiveYN("Write solution to file instead of console? (y/n): ") };

	std::string fileName{};

	if (toFile)
		fileName = ReceiveFileNameForWrite("File already exists. Append? (y/n): ");

	//Always autosolve if we're outputting to file.
	bool autosolve{ toFile || ReceiveYN("Run every turn without prompting input? Warning, this may take a while. (y/n): ") };

	bool  go{ true };
	size_t currPlayer{ activePlayers.size() - 1 };
	while (activePlayers.size() != 0 && go)
	{
		Player& currentPlayer = activePlayers[currPlayer];
		switch (ProcessPlayerTurn(currentPlayer))
		{
		case TurnResult::Finished:
			activePlayers.erase(activePlayers.begin() + currPlayer);
			std::cout << "A player has reached the goal. " << activePlayers.size() << " remain.\n";
		case TurnResult::Moved:
			toFile ? WriteMazeToFile(*this, fileName, true) : (void)(std::cout << "\n" << *this);
			break;
		case TurnResult::Blocked:
			std::cout << "Player at " << currentPlayer.pos << " is currently blocked, skipping turn.\n";
			break;
		case TurnResult::Invalid:
			std::cout << "An invalid move was attempted! Player at " << currentPlayer.pos << " attempted to move to " << *currentPlayer.path.begin() << ". Solve stopped.";
			return;
			break;
		}

		currPlayer == 0 ? currPlayer = activePlayers.size() - 1 : currPlayer--;

		go = autosolve || ReceiveYN("Continue? (y/n): ");
	}

	std::cout << "\nCOMPLETE\n\n";
}

void Maze::GenerateEntrances(size_t _count)
{
	//This solution ensures that no exits will overlap with one another and is of known complexity ( O(n) )
	//List every possible entrance to the maze and shuffle.

	std::vector<Coord> possibleEntrances{};
	possibleEntrances.reserve(width + height - 2);

	for (int i{2}; i < width; i+=2)
		possibleEntrances.insert(possibleEntrances.end(), { {i-1,0} , {i-1,height-1} });
	for (int i{2}; i < height; i+=2)
		possibleEntrances.insert(possibleEntrances.end(), { {0,i-1} , {width-1 ,i-1} });

	std::random_shuffle(possibleEntrances.begin(), possibleEntrances.end());

	size_t entranceCount{ std::min(_count, possibleEntrances.size()) };
	entrances.reserve(entranceCount);

	for (size_t i{}; i < entranceCount; i++)
	{
		entrances.push_back(possibleEntrances[i]);

		Player player { entrances[i],{},Cell::Entrance };
		GeneratePlayerPath(player);
		playerCount++;

		(*this)[possibleEntrances[i]] = Cell::Player;
	}
}

//Randomized Prim's algorithm for maze generation.
void Maze::GenerateMaze()
{
	struct FrontierCell
	{
		Coord loc;
		Coord connector;
	};

	//Blank out maze with walls.
	for (int y{}; y < height; y++)
		for (int x{}; x < width; x++)
			(*this)[x][y] = Cell::Wall;

	(*this)[1][1] = Cell::Empty;

	std::vector<FrontierCell> frontierCells{};

	frontierCells.push_back({ {3,1},{2,1} });
	frontierCells.push_back({ {1,3},{1,2} });

	while (frontierCells.size() != 0)
	{
		int pos{ rand() % (int)frontierCells.size() };

		FrontierCell currCell{ frontierCells[pos] };
		frontierCells.erase(frontierCells.begin() + pos);

		if ((*this)[currCell.loc] != Cell::Wall) 
			continue;

		Coord loc{ currCell.loc };

		(*this)[loc] = Cell::Empty;
		(*this)[currCell.connector] = Cell::Empty;

		if (loc.x + 2 < width - 1 && (*this)[loc + Coord{2,0}] == Cell::Wall)
			frontierCells.push_back({ loc + Coord{2,0},loc + Coord{1,0} });

		if (loc.x - 2 > 0 && (*this)[loc - Coord{2,0}] == Cell::Wall)
			frontierCells.push_back({ loc - Coord{2,0},loc - Coord{1,0} });

		if (loc.y + 2 < height - 1 && (*this)[loc + Coord{0,2}] == Cell::Wall)
			frontierCells.push_back({ loc + Coord{0,2},loc + Coord{0,1}});

		if (loc.y - 2 > 0 && (*this)[loc - Coord{0,2}] == Cell::Wall)
			frontierCells.push_back({ loc - Coord{0,2},loc - Coord{0,1} });
	}

	for (int x{-1}; x <= 1; x++)
		for (int y{-1}; y <= 1; y++)
			(*this)[finish + Coord{ x,y }] = Cell::Empty;
	(*this)[finish] = Cell::Finish;
}

void WriteMazeToFile(const Maze& _maze, const std::string& _fileName, bool _append)
{
	auto base{ _append ? std::ios_base::app : std::ios_base::out };

	std::ofstream file{};
	file.open(_fileName, base);

	file << _maze;

	file.close();
}

//Factory function to generate maze from file. (friend of Maze class)
Maze ReadMazeFromFile(const std::string& _fileName)
{
	std::ifstream file{};
	std::string line{};
	file.open(_fileName);

	if (file.fail())
		throw std::exception("Unable to open file.");

	getline(file, line);

	int width{ (int)line.size() };
	int height{ 1 };

	while (!file.eof())
	{
		std::getline(file, line);
		if (line.size() != 0)
			height++;
	}

	Maze maze{ width, height, 0, false };

	file.clear();
	file.seekg(0);

	char inChar{};
	int currLine{ 0 };
	int currChar{ 0 };

	//We hold on to players for later to generate their paths.
	std::vector<Maze::Player> players{};

	for (int y{}; y < height; y++)
	{
		getline(file, line);

		if (line.size() == 0)
			throw std::exception("Provided file is not valid maze.");

		for (int x{}; x < (int)line.size(); x++)
		{
			maze[x][y] = { CharToCell(line[x]) };

			switch (maze[x][y])
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
	for (size_t i{}; i < players.size(); i++)
		maze.GeneratePlayerPath(players[i]);

	file.close();

	return maze;
}

void Maze::DisplayInfo() const
{
	//This is a messy if block, but turning these into conditionals makes it even less readable.
	if (playerCount > 0)
	{
		if (activePlayers.size() == 0)
			std::cout << "Maze is unsolvable.\n";
		else
			std::cout << (activePlayers.size() == playerCount ? "Maze is fully solvable\n" : "Maze is partially solvable\n");
	}
	else
		std::cout << "This maze contains no players.\n";

	std::cout << "Average steps to solve: " << AverageStepsToSolve() << "\n";
	std::cout << "The pathfinder reused " << pathfinder.reusedNodeCount / activePlayers.size() << " path nodes per player.\n\n";

}

float Maze::AverageStepsToSolve() const
{
	float steps{};

	for (size_t i{}; i < activePlayers.size(); i++)
		steps += activePlayers[i].path.size();

	activePlayers.empty() ? steps = 0 : steps /= activePlayers.size();

	return steps;
}