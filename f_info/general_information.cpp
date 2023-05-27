#include "pch.hpp"

std::string general_get_info(const std::string& filepath)
{
	HANDLE handle = handle_for_file(filepath, GENERIC_READ);

	if (handle == INVALID_HANDLE_VALUE) {
		throw std::exception(std::format("an error occurred when opening the file:\n{}\n{:X}", get_last_error(), GetLastError()).c_str());
	}

	file_data_s data(1, 1, 1);
	data.where = filepath;
	data.name = full_path_to_name(filepath);
	data.dwSize = GetFileSize(handle, 0);

	if (GetFileTime(handle, data.lpCreationTime.get(), data.lpLastAccessTime.get(), data.lpLastWriteTime.get()) == NULL) {
		throw std::exception(std::format("an error occurred when trying to access the file data:\n{}", get_last_error()).c_str());
	}

	data.owner = get_file_owner(handle);
	data.common_attributes = general_get_attributes(handle);

	CloseHandle(handle);

	return general_write_buffer(&data);


}
std::string general_write_buffer(const file_data_s* data)
{
	if (!data)
		throw std::exception("general_write_buffer(): invalid data");

	

	const auto time_created = filetime_to_text(data->lpCreationTime.get());
	//const auto last_access = filetime_to_text(data->lpLastAccessTime.get());
	const auto last_written = filetime_to_text(data->lpLastWriteTime.get());
	const auto extension = file_extension(data->name);
	const auto real_name = data->name.substr(0, data->name.size() - (extension == "(no extension)" ? 0 : extension.size() + 1));

	std::stringstream cb;

	cb << "```\n";
	cb << "full path:        " << data->where << '\n';
	cb << "name:             " << real_name << '\n';
	cb << "extension:        " << extension << '\n';
	cb << "owner:            " << data->owner << '\n';
	cb << "size:             " << formatBytes(data->dwSize) << '\n';
	cb << "time created:     " << time_created << '\n';
	//cb << "last access:      " << last_access << '\n';
	cb << "last written:     " << last_written << '\n';
	cb << "attributes:	  " << data->common_attributes << '\n';

	cb << "```";

	return cb.str();
}
std::string general_get_attributes(HANDLE fHandle)
{
	BY_HANDLE_FILE_INFORMATION basicInfo;

	if (GetFileInformationByHandle(fHandle, &basicInfo) == NULL) {
		throw std::exception(std::format("failed to get file attributes:\n{}", get_last_error()).c_str());
		return "";
	}

	std::string out;
	DWORD a = basicInfo.dwFileAttributes;


	if (a & FILE_ATTRIBUTE_READONLY)
		out += ("read-only ");

	if (a & FILE_ATTRIBUTE_HIDDEN)
		out += ("hidden ");

	if (a & FILE_ATTRIBUTE_SYSTEM)
		out += ("system ");

	if (out.empty())
		return "<no common attributes>";

	return out;

}
std::string get_file_owner(HANDLE fHandle)
{
	PSID pSidOwner = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	LPSTR AcctName = NULL;
	LPSTR DomainName = NULL;
	DWORD dwAcctName = 128;
	DWORD dwDomainName = 128;
	SID_NAME_USE eUse = SidTypeUnknown;

	

	if (GetSecurityInfo(fHandle, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, &pSidOwner, NULL, NULL, NULL, &pSD) != ERROR_SUCCESS) {
		throw std::exception(std::format("unable to get file owner:\n{}", get_last_error()).c_str());
		return "";
	}

	//obtain buffer sizes
	if (LookupAccountSidA(NULL, pSidOwner, AcctName, (LPDWORD)&dwAcctName, DomainName, (LPDWORD)&dwDomainName, &eUse) == NULL) {
		throw std::exception(std::format("unable to look up owner:\n{}", get_last_error()).c_str());
		return "";
	}

	AcctName = (char*)GlobalAlloc(GMEM_FIXED, dwAcctName * sizeof(char));
	DomainName = (char*)GlobalAlloc(GMEM_FIXED, dwDomainName * sizeof(char));

	if (AcctName == NULL || DomainName == NULL) {
		throw std::exception(std::format("unable allocate in get_file_owner():\n{}", get_last_error()).c_str());
		return "";
	}
	dwAcctName = 128;
	dwDomainName = 128;
	if (LookupAccountSidA(NULL, pSidOwner, AcctName, (LPDWORD)&dwAcctName, DomainName, (LPDWORD)&dwDomainName, &eUse) == NULL) {

		if (GetLastError() == ERROR_NONE_MAPPED)
			throw std::exception("<unknown owner>");
		else
			throw std::exception(std::format("#2 unable to look up owner:\n{}", get_last_error()).c_str());
		return "";
	}

	return AcctName;

}