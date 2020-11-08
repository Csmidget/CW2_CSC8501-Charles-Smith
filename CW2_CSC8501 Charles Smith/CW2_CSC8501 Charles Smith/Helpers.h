#pragma once
#include <string>
#include <iostream>

// HEADER SUMMARY
// A set of helper functions that are used throughout the program, cleaning
// up code that is heavily reused (Especially Receiving use input)

namespace Helpers
{
	// Return cin to default state ready for future requests, this prevents bad input completely messing up the program.
	void ClearCin();

	// Receives input from user. Returns true if users enters 'y', false if user enter 'n'
	bool ReceiveYN(std::string _prompt);

	std::string ReceiveFileName();
	std::string ReceiveFileNameForWrite(std::string _existsPrompt);

	bool FileExists(std::string _fileName);

	//Template function needs to be implemented in header. Receives and
	//validates a value. Looping until a valid value is entered.
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
}
