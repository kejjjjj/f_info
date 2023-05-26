#pragma once

#ifndef PCH_H
#define PCH_H

#include <Windows.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <string_view>
#include <filesystem>
#include <optional>
#include <fileapi.h>
#include <memory>
#include <locale>
#include <unordered_map>
#include <functional>
#include <aclapi.h>

namespace fs = std::filesystem;

#include "file_utilities.hpp"
#include "general_information.hpp"
#include "f_info.hpp"
#endif