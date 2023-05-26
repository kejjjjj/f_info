#ifndef geninfo_
#define geninfo_

#include "pch.hpp"


struct file_data_s
{
	file_data_s() = delete;
	file_data_s(bool creationTime = 0, bool lastAccess = 0, bool lastWrite = 0) {

		if (creationTime)
			lpCreationTime = std::shared_ptr<FILETIME>(new FILETIME);

		if (lastAccess)
			lpLastAccessTime = std::shared_ptr<FILETIME>(new FILETIME);

		if (lastWrite)
			lpLastWriteTime = std::shared_ptr<FILETIME>(new FILETIME);
	}

	std::string name;
	std::string where;
	std::string owner;

	std::string common_attributes;

	DWORD dwSize;
	std::shared_ptr<FILETIME> lpCreationTime = 0;
	std::shared_ptr<FILETIME> lpLastAccessTime = 0;
	std::shared_ptr<FILETIME> lpLastWriteTime = 0;

};

std::string general_get_info(const std::string& filepath);
std::string general_write_buffer(const file_data_s* filepath);
std::string general_get_attributes(HANDLE fHandle);

std::string get_file_owner(HANDLE fHandle);

#endif