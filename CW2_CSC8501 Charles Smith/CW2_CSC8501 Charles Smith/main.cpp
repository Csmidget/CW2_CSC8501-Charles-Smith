#include <iostream>
#include <time.h>
#include "Maze.h"

//Clear the Cin stream in case a bunch of junk was entered.
void ClearCin()
{
    std::cin.clear();
    std::cin.ignore(INT_MAX, '\n');
}

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

    maze.RunSolution();

    char input{};
    while (input != 'n' && input != 'y')
    {
        std::cout << "Save maze to file? (y/n): ";
        std::cin >> input;
        ClearCin();
    }

    if (input == 'y')
        WriteMazeToFile(maze);
}

void LoadMaze()
{
    Maze maze = ReadMazeFromFile();

    maze.RunSolution();

    char input{};
    while (input != 'n' && input != 'y')
    {
        std::cout << "Save maze to file? (y/n): ";
        std::cin >> input;
    }

    if (input == 'y')
        WriteMazeToFile(maze);
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
        case 'q':
        case'Q':
            end = true;
            break;
        }
    }
}
