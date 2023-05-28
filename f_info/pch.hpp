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
#include <fstream>

namespace fs = std::filesystem;

#include "md5.hpp"

#include "file_utilities.hpp"
#include "general_information.hpp"
#include "md5_hash.hpp"

#include "f_info.hpp"
#endif