#pragma once
#include <string>
#include <iostream>

void ClearCin();
bool ReceiveYN(std::string _prompt);
bool FileExists(std::string _fileName);
std::string ReceiveFileName();

//Template function needs to be implemented in header.
template<typename T, class Validator>
T ReceiveValue(std::string _prompt, std::string _invalidPrompt, Validator _validator)
{
	T value;
	std::cout << _prompt;
	std::cin >> value;
	while (!_validator(value))
	{
		std::cout << _invalidPrompt;
		std::cin >> value;
		ClearCin();
	}
	return value;
}
