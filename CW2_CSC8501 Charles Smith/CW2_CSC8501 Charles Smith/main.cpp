#include "Helpers.h"
#include "Maze.h"

#include <iostream>
#include <time.h>

void NewMaze()
{
    //I probably wouldn't normally use a template function to receive input as below, but it is a nice
    //opportunity to show off both templates and lambda expressions.
    int width{ Helpers::ReceiveValue<int>("Enter maze width (Odd number 5 to 201): ",
                                          "Invalid width. Please enter an odd number from 5 to 201: ",
                                          [](int val) {return val % 2 != 0 && val >= 5 && val <= 201; }) };


    int height{ Helpers::ReceiveValue<int>("Enter maze height (Odd number 5 to 201): ",
                                           "Invalid height. Please enter an odd number from 5 to 201: ",
                                           [](int val) {return val % 2 != 0 && val >= 5 && val <= 201; }) };

    int maxPlayers{ width + height - 2 };
    auto maxPlayersStr{ std::to_string(maxPlayers) };
    int players{ Helpers::ReceiveValue<int>("Enter number of players (2 to " + maxPlayersStr + ", more than 5 is not recommended): ",
                                            "Invalid player count. Please enter a number from 2 to " + maxPlayersStr + ": ",
                                            [maxPlayers](int val) {return val >= 2 && val <= maxPlayers; }) };
    
    if (players > 5 && !Helpers::ReceiveYN("WARNING: Mazes with a large number of players can take A LOT of turns to solve. Continue? (y/n): "))
        return;

    std::cout << "\n";

    Maze maze{ width, height, players, true };

    std::cout << maze;
    maze.DisplayInfo();

    if (maze.IsSolvable() && Helpers::ReceiveYN("Solve maze? (y/n): "))
        maze.RunSolution();

    if (Helpers::ReceiveYN("Save maze to file? (y/n): "))
        WriteMazeToFile(maze, Helpers::ReceiveFileNameForWrite("File already exists. Overwrite? (y/n): "), false);
}

void LoadMaze()
{
    std::string fileName{};

    do
        fileName = Helpers::ReceiveFileName();
    while (!Helpers::FileExists(fileName));

    std::cout << "\n";

    Maze maze{};

    try
    {
         maze = ReadMazeFromFile(fileName);
    }
    catch(std::exception e)
    {
        std::cout <<"ERROR: " << e.what() << "\n" << "Enter any key to return to main menu.";
        std::cin;
        Helpers::ClearCin();
        return;
    }

    std::cout << maze;
    maze.DisplayInfo();

    if (maze.IsSolvable() && Helpers::ReceiveYN("Solve maze? (y/n): "))
        maze.RunSolution();

    if (Helpers::ReceiveYN("Save maze to file? (y/n): "))
        WriteMazeToFile(maze, Helpers::ReceiveFileNameForWrite("File already exists. Overwrite? (y/n): "), false);
}

void MazeAnalysis()
{
    int maxWidth{ Helpers::ReceiveValue<int>("Enter maximum width (Odd number 5 to 201): ",
                                          "Invalid width. Please enter an odd number from 5 to 201: ",
                                          [](int val) {return val % 2 != 0 && val >= 5 && val <= 201; }) };


    int maxHeight{ Helpers::ReceiveValue<int>("Enter maximum height (Odd number 5 to 201): ",
                                           "Invalid height. Please enter an odd number from 5 to 201: ",
                                           [](int val) {return val % 2 != 0 && val >= 5 && val <= 201; }) };

    int maxPlayers{ maxWidth + maxHeight - 2 };
    auto maxPlayersStr{ std::to_string(maxPlayers) };
    maxPlayers = Helpers::ReceiveValue<int>("Enter maximum number of players (2 to " + maxPlayersStr + "): ",
                                            "Invalid player count. Please enter a number from 2 to " + maxPlayersStr + ": ",
                                            [maxPlayers](int val) {return val >= 2 && val <= maxPlayers; });

    const int MAZECOUNT{100};

    Maze mazes[MAZECOUNT]{};
    float averageStepsTotal{};
    float averageWidth{};
    float averageHeight{};
    float averageExits{};
    int averageReusedNodesPerPlayer{};

    for (size_t i{}; i < MAZECOUNT; i++)
    {
        //Generate random width and height that are odd numbers 5 - maxSize.
        int width{ (rand() % (maxWidth - 4) / 2) * 2 + 5 };
        int height{ (rand() % (maxHeight - 4) / 2) * 2 + 5 };
        int players{ rand() % (maxPlayers - 1) + 2 };

        std::cout << i << ": " << width << "x" << height << ", " << players << " players.\n";
        mazes[i] = Maze(width, height, players, true);

        //Extract information about maze
        float averageSteps{ mazes[i].AverageStepsToSolve() };
        averageStepsTotal += (averageSteps / width + averageSteps / height);
        averageWidth += width;
        averageHeight += height;
        averageExits += players;
        averageReusedNodesPerPlayer += mazes[i].PathfindingReusedNodes() / players;
    }

    //Get average values across all mazes.
    averageStepsTotal /= MAZECOUNT;
    averageWidth /= MAZECOUNT;
    averageHeight /= MAZECOUNT;
    averageExits /= MAZECOUNT;
    averageReusedNodesPerPlayer /= MAZECOUNT;

    //Get average between width and height.
    averageStepsTotal /= 2;

    std::cout << "\nFor the above mazes:\n\n";
    std::cout << "The average width was: " << averageWidth << ".\n";
    std::cout << "The average height was: " << averageHeight << ".\n";
    std::cout << "The average number of players was: " << averageExits << ".\n";
    std::cout << "On average, the pathfinder was able save time by reusing " << averageReusedNodesPerPlayer << " path nodes per player.\n";
    std::cout << "Average increase in steps required to solve maze per unit of width or height: " << averageStepsTotal << " steps.\n\n";

    std::cout << "General Findings:\n";
    std::cout << "As the size of a maze increases the number of steps required to solve it increases exponentially.\n";
    std::cout << "Increasing the number of players exponentially increases the time it takes to solve a maze, due to players blocking one another.\n\n";

    if (Helpers::ReceiveYN("Save mazes to file? (y/n): "))
    {
        std::string fileName{ Helpers::ReceiveFileNameForWrite("File already exists. Append? (y/n): ") };
        for (size_t i{}; i < MAZECOUNT; i++)
            WriteMazeToFile(mazes[i], fileName, true);
        std::cout << "Saved.\n";
    }
    std::cout << "Enter any key to return to menu.\n";
    std::cin;
    Helpers::ClearCin();
}

void DisplayMainMenu()
{
    system("cls");
    std::cout << "###################################\n";
    std::cout << "#                                 #\n";
    std::cout << "# Maze Generator by Charles Smith #\n";
    std::cout << "#                                 #\n";
    std::cout << "# Commands:                       #\n";
    std::cout << "#                                 #\n";
    std::cout << "# n - generate a brand new maze.  #\n";
    std::cout << "# l - load a maze from file.      #\n";
    std::cout << "# a - maze analysis.              #\n";
    std::cout << "# q - quit.                       #\n";
    std::cout << "#                                 #\n";
    std::cout << "###################################\n\n";
    std::cout << "Command: ";
}

int main()
{
    srand((unsigned int)time(NULL));

    char input{};
    bool go{true};
    while (go)
    {
        DisplayMainMenu();
        std::cin >> input;
        Helpers::ClearCin();

        switch (input)
        {
        case 'n':
        case 'N':
            NewMaze();
            break;
        case 'l':
        case 'L':
            LoadMaze();
            break;
        case 'a':
        case 'A':
            MazeAnalysis();
            break;
        case 'q':
        case'Q':
            go = false;
            break;
        }
    }
}
