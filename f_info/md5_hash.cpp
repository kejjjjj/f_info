#include "pch.hpp"

std::string get_md5(const std::string& filepath)
{
	std::ifstream file(filepath, std::ios::binary);

	if (!file.is_open()) {
		throw std::exception("unable to open the file for binary operation");
		return "";
	}

	MD5 md5;

	constexpr size_t bufferSize = 4096;
	char buffer[bufferSize];

	while (file.read(buffer, bufferSize)) {
		md5.add(buffer, bufferSize);
	}
	
	file.close();

	return md5.getHash();

}