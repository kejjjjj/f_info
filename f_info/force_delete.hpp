
#pragma once

#ifndef _fdel_
#define _fdel_

#include "pch.hpp"

DWORD get_file_host_pid(const std::string& filepath);
std::string get_file_host_name(DWORD pid);
void force_delete_file(const std::string& filepath);
#endif