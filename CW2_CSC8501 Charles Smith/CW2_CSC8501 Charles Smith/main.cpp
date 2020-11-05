#include <iostream>
#include <time.h>
#include "Maze.h"
#include "Helpers.h"



void NewMaze()
{
    int width{};
    int height{};
    int exits{};

    std::cout << "Enter maze width (Odd number 5 to 201): ";
    std::cin >> width;
    while (width % 2 == 0 || width < 5 || width > 201)
    {
        std::cout << "Invalid width. Please enter an odd number from 5 to 201: ";
        std::cin >> width;
        ClearCin();
    }

    std::cout << "Enter maze height (Odd number 5 to 201): ";
    std::cin >> height;
    while (height % 2 == 0 || height < 5 || height > 201)
    {
        std::cout << "Invalid height. Please enter an odd number from 5 to 201: ";
        std::cin >> height;
        ClearCin();
    }

    int maxExits = width + height - 2;

    std::cout << "Enter number of exits (0 to " << maxExits << "): ";
    std::cin >> exits;
    while (exits < 0 || exits > maxExits)
    {
        std::cout << "Invalid exit count. Please enter a number from 0 to " << maxExits << ":";
        std::cin >> exits;
        ClearCin();
    }

    std::cout << "\n";

    Maze maze(width, height, exits, true);

    std::cout << maze;
    maze.DisplayInfo();

    if (maze.IsSolvable() && ReceiveYN("Solve maze? (y/n): "))
        maze.RunSolution();

    if (ReceiveYN("Save maze to file? (y/n): "))
    {
        std::string fileName;

        bool validFile = false;

        while (!validFile)
        {
            fileName = ReceiveFileName();

            validFile = FileExists(fileName) ? ReceiveYN("File already exists. Overwrite? (y/n): ") : true;
        }

        WriteMazeToFile(maze, fileName, false);
    }
}

void LoadMaze()
{
    std::string fileName;

    do
        fileName = ReceiveFileName();
    while (!FileExists(fileName));

    Maze maze;
    try
    {
         maze = ReadMazeFromFile(fileName);
    }
    catch(std::exception e)
    {
        std::cout << e.what() << "\n" << "Enter any key to return to main menu.";
        std::getchar();
        ClearCin();
        return;
    }

    std::cout << maze;
    maze.DisplayInfo();

    if (maze.IsSolvable() && ReceiveYN("Solve maze? (y/n): "))  
        maze.RunSolution();

    if (ReceiveYN("Save maze to file? (y/n): "))
    {
        std::string fileName;

        bool validFile = false;

        while (!validFile)
        {
            fileName = ReceiveFileName();

            validFile = FileExists(fileName) ? ReceiveYN("File already exists. Overwrite? (y/n): ") : true;
        }

        WriteMazeToFile(maze, fileName, false);
    }
}

void MazeAnalysis()
{
    Maze mazes[100]{};

    for (size_t i = 0; i < 100; i++)
    {
        int width = (rand() % 98) * 2 + 5;
        int height = (rand() % 98) * 2 + 5;
        int exits = rand() % (width + height - 2);

        std::cout << i << ": " << width << "x" << height << ", " << exits << " exits.\n";
        mazes[i] = Maze(width, height, exits, true);


    }
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
    srand(time(NULL));

    char input{};
    bool end{};
    while (!end)
    {
        DisplayMainMenu();
        std::cin >> input;
        ClearCin();

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
            end = true;
            break;
        }
    }
}
