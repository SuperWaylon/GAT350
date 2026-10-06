#pragma once
#include <string>
#include <iostream>

namespace nu
{
	inline std::string ToLower(const std::string& str)
	{
		std::string lower = str;
		for (auto& c : lower)
		{
			c = std::tolower(c);
		}

		return lower;
	}

	inline std::string ToUpper(const std::string& str)
	{
		std::string lower = str;
		for (auto& c : lower)
		{
			c = std::toupper(c);
		}

		return lower;
	}

	inline bool EqualsIgnoreCase(const std::string& str1, const std::string& str2)
	{
		if (str1.size() != str2.size()) return false;

		return ToLower(str1) == ToLower(str2);
	}

}