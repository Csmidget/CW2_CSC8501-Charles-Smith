#include <iostream>
#include <fstream>

//Clear the Cin stream in case a bunch of junk was entered.
void ClearCin()
{
    std::cin.clear();
    std::cin.ignore(INT_MAX, '\n');
}

bool ReceiveYN(std::string _prompt)
{
	char input{};
	while (input != 'y' && input != 'n')
	{
		std::cout << _prompt;
		std::cin >> input;
		ClearCin();
	}
	return input == 'y';
}

bool FileExists(std::string _fileName)
{
	std::ifstream file (_fileName);
	return file.good();
}

std::string ReceiveFileName()
{
	std::string fileName{};
	std::cout << "Enter file name, including extension: ";
	std::cin >> fileName;

	while (std::cin.fail())
	{
		ClearCin();

		std::cout << "Invalid name entered, try again: ";
		std::cin >> fileName;
	}

	ClearCin();
	return fileName;
}


