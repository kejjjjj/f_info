#pragma once

#ifndef f_ile
#define f_ile

#include "pch.hpp"

bool file_exists(const std::string_view& file);
std::string full_path_to_name(const std::string_view& path);
std::string file_extension(const std::string_view& file);
HANDLE handle_for_file(const std::string& filepath, const DWORD access_level);
std::string get_last_error();

std::string SysTimeDifference(SYSTEMTIME* systime1, SYSTEMTIME* systime2);
std::string SystemTimeToString(SYSTEMTIME* systime);

std::string formatBytes(DWORD kilobytes);

std::string filetime_to_text(const LPFILETIME p);



void CopyToClipboard(const std::string& text);

#endif