#pragma once
#include <string>
#include <vector>
#include "constants.h"

namespace tools
{
	float GetInput(const std::string& prompt);

	std::vector<std::string> split(const std::string& str, const char* delim);
	
	template<class T>
	void printArray(T* a, size_t s, bool printEndline = true)
	{
		if (!a)
			return;

		std::cout << "[ ";
		for (size_t i = 0; i < s; ++i)
		{
			std::cout << a[i];
			if (i + 1 < s)
				std::cout << ", ";

		}
		std::cout << " ]";
		if (printEndline)
			std::cout << std::endl;
	};

	template<class T>
	bool isEqual(T* a, T* b, size_t s)
	{
		if (!a || !b)
			return false;
		for (size_t i = 0; i < s; ++i)
		{
			if(std::abs(a[i] - b[i]) > MAX_DIFF)
				return false;
		}
		return true;
	}
}