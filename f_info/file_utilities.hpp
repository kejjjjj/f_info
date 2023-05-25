#pragma once

#ifndef f_ile
#define f_ile

#include "pch.hpp"

struct file_data_s
{
	file_data_s() = delete;
	file_data_s(bool creationTime=0, bool lastAccess=0, bool lastWrite=0) {

		if (creationTime)
			lpCreationTime = std::shared_ptr<FILETIME>(new FILETIME);

		if (lastAccess)
			lpLastAccessTime = std::shared_ptr<FILETIME>(new FILETIME);

		if (lastWrite)
			lpLastWriteTime = std::shared_ptr<FILETIME>(new FILETIME);
	}

	std::string name;
	std::string where;

	DWORD dwSize;
	std::shared_ptr<FILETIME> lpCreationTime = 0;
	std::shared_ptr<FILETIME> lpLastAccessTime = 0;
	std::shared_ptr<FILETIME> lpLastWriteTime = 0;

};

bool file_exists(const std::string_view& file);
std::string full_path_to_name(const std::string_view& path);
std::string file_extension(const std::string_view& file);
HANDLE handle_for_file(const std::string& filepath, const DWORD access_level);
std::string get_last_error();

std::string SysTimeDifference(SYSTEMTIME* systime1, SYSTEMTIME* systime2);
std::string SystemTimeToString(SYSTEMTIME* systime);

std::string formatBytes(DWORD kilobytes);

std::string filetime_to_text(const LPFILETIME p);

std::optional<file_data_s> getfile(const std::string& filepath);


void CopyToClipboard(const std::string& text);

#endif