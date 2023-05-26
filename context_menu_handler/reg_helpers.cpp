#include "reg_helpers.hpp"


HKEY NewRegistryKey(HKEY src, const std::wstring& keyName)
{
	HKEY subkey;

	auto result = RegCreateKeyExW(src, keyName.c_str(), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &subkey, NULL);

	if (result != ERROR_SUCCESS) {
		throw std::exception("NewRegistryKey: failed to create a key");
	}
	return subkey;
}
void RegistryShellAction(HKEY key, const std::wstring& keyIdentifier, const std::wstring& MUIVerb, const std::wstring& action)
{

}

void NewRegistryString(HKEY key, const std::wstring& identifier, const std::wstring& value)
{
	auto result = RegSetValueExW(key, identifier.c_str(), 0, REG_SZ, (const BYTE*)value.c_str(), value.size()*2);

	if (result != ERROR_SUCCESS) {
		throw std::exception("NewRegistryString: failed to create a key");
	}
}
void EditRegistryStringDefault(HKEY key, const std::wstring& value)
{
	auto result = RegSetValueExW(key, NULL, 0, REG_SZ, (const BYTE*)value.c_str(), value.size()*2);

	if (result != ERROR_SUCCESS) {
		throw std::exception("EditRegistryStringDefault: failed to create a key");
	}
}