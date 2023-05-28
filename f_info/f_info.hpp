#ifndef _finfohp
#define _finfohp

#include "pch.hpp"

inline std::unordered_map<std::string, std::function<std::optional<std::string>(const std::string&)>> tool_features =
{
	{"-default", [](const std::string& a) -> std::optional<std::string>
	{
		return general_get_info(a);
	}},
	{"-md5", [](const std::string& a) -> std::optional<std::string>
	{
		return get_md5(a);
	}},


};

#endif