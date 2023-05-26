#pragma once

#ifndef reg_hle
#define reg_hle

#include <windows.h>
#include <shlobj.h>
#include <string>
#include <optional>

HKEY NewRegistryKey(HKEY src, const std::wstring& keyName);
void RegistryShellAction(HKEY key, const std::wstring& keyIdentifier, const std::wstring& MUIVerb, const std::wstring& action);
void NewRegistryString(HKEY key, const std::wstring& identifier, const std::wstring& value);
void EditRegistryStringDefault(HKEY key, const std::wstring& value);
#endif