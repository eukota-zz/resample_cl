#pragma once
#include <string>
#include <vector>

namespace tools
{
	float GetInput(const std::string& prompt);

	std::vector<std::string> split(const std::string& str, const char* delim);
}