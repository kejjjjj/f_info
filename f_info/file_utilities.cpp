#include "pch.hpp"


bool file_exists(const std::string_view& file)
{
	return fs::exists(file);
}
std::string full_path_to_name(const std::string_view& path)
{
	const auto p = path.find_last_of('\\');

	return std::string(path.substr(p+1));
}
std::string file_extension(const std::string_view& file)
{
	auto ext = file.find_last_of('.');

	if (ext == std::string::npos)
		return "(no extension)";

	return std::string(file.substr(ext + 1));
}
std::string get_last_error()
{
	const DWORD errorMessageID = ::GetLastError();
	char* messageBuffer = nullptr;

	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&messageBuffer, 0, NULL);

	std::string output = std::string(messageBuffer, size);

	LocalFree(messageBuffer);
	return output;
}
HANDLE handle_for_file(const std::string& filepath, const DWORD access_level)
{	
	return CreateFileA(filepath.c_str(), access_level, NULL, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
}
std::string SysTimeDifference(SYSTEMTIME* systime1, SYSTEMTIME* systime2)
{
	FILETIME file_time1;
	SystemTimeToFileTime(systime1, &file_time1);

	FILETIME file_time2;
	SystemTimeToFileTime(systime2, &file_time2);

	ULARGE_INTEGER ularge_int1 = { 0 };
	ularge_int1.LowPart = file_time1.dwLowDateTime;
	ularge_int1.HighPart = static_cast<LONG>(file_time1.dwHighDateTime);

	ULARGE_INTEGER ularge_int2 = { 0 };
	ularge_int2.LowPart = file_time2.dwLowDateTime;
	ularge_int2.HighPart = static_cast<LONG>(file_time2.dwHighDateTime);

	LONGLONG diff_in_100ns_ticks =
		(static_cast<LONGLONG>(ularge_int2.QuadPart) - static_cast<LONGLONG>(ularge_int1.QuadPart));

	long long SECONDS_IN_100NS_TICKS = 10'000'000LL;

	std::stringstream ss;
	std::string suffix;

	decltype(SECONDS_IN_100NS_TICKS) time = 0;

	if (!time) {
		time = diff_in_100ns_ticks / (SECONDS_IN_100NS_TICKS * 60 * 60 * 24);
		suffix = "days";
	}
	if (!time) {
		time = diff_in_100ns_ticks / (SECONDS_IN_100NS_TICKS * 60 * 60);
		suffix = "hours";
	}
	if (!time) {
		time = diff_in_100ns_ticks / (SECONDS_IN_100NS_TICKS * 60);
		suffix = "minutes";
	}
	if (!time) {
		time = diff_in_100ns_ticks / (SECONDS_IN_100NS_TICKS);
		suffix = "seconds";
	}

	
	ss << time;

	return ss.str() + ' ' + suffix + " ago";
}
std::string SystemTimeToString(SYSTEMTIME* systime)
{
	// convert systemtime to filetime
	FILETIME filetime;
	SystemTimeToFileTime(systime, &filetime);


	std::stringstream ss;

	SYSTEMTIME nowsystime;
	GetSystemTime(&nowsystime);

	auto month	= (systime->wMonth < 10 ? "0" : "") + std::to_string(systime->wMonth);
	auto day	= (systime->wDay < 10 ? "0" : "") + std::to_string(systime->wDay);
	auto hour	= (systime->wHour < 10 ? "0" : "") + std::to_string(systime->wHour);
	auto minute = (systime->wMinute < 10 ? "0" : "") + std::to_string(systime->wMinute);
	auto second = (systime->wSecond < 10 ? "0" : "") + std::to_string(systime->wSecond);


	// format date and time
	ss << std::to_string(systime->wYear) << "-" <<
		month << "-" <<
		day << " " << 
		hour << ":" <<
		minute << ":" <<
		second << " - " <<
		"(" << SysTimeDifference(systime, &nowsystime) << ")";

	return ss.str();



}
std::string formatBytes(DWORD kilobytes) {
	const std::size_t numSuffixes = 6;
	const char* suffixes[numSuffixes] = { "B", "KB", "MB", "GB", "TB", "PB" };

	double size = static_cast<double>(kilobytes);
	int suffixIndex = 0;

	while (size >= 1024 && suffixIndex < numSuffixes - 1) {
		size /= 1024;
		++suffixIndex;
	}

	char buffer[32];
#ifdef _MSC_VER
#pragma warning(suppress : 4996)
#endif
	sprintf_s(buffer, "%.2f%s", size, suffixes[suffixIndex]);

	return std::string(buffer);
}
std::string filetime_to_text(const LPFILETIME p)
{
	SYSTEMTIME systime;

	if (!p) {
		throw std::exception("filetime_to_text(): passed an invalid filetime argument");
		return "";
	}

	if (FileTimeToSystemTime(p, &systime) == 0) {
		throw std::exception(std::format("failed to get systemtime, error code: {}", get_last_error()).c_str());
		return "";
	}

	return SystemTimeToString(&systime);

}
std::optional<file_data_s> getfile(const std::string& filepath)
{
	if (!file_exists(filepath)) {
		throw std::exception(std::format("{} is not a valid file!", filepath).c_str());
		return std::nullopt;
	}

	HANDLE handle = handle_for_file(filepath, GENERIC_READ);

	if (handle == INVALID_HANDLE_VALUE) {
		throw std::exception(std::format("an error occurred when opening the file:\n{}", get_last_error()).c_str());
		return std::nullopt;
	}

	file_data_s data(1, 1, 1);
	data.where = filepath;
	data.name = full_path_to_name(filepath);
	data.dwSize = GetFileSize(handle, 0);

	if (GetFileTime(handle, data.lpCreationTime.get(), data.lpLastAccessTime.get(), data.lpLastWriteTime.get()) == NULL) {
		throw std::exception(std::format("an error occurred when trying to access the file data:\n{}", get_last_error()).c_str());
		return std::nullopt;
	}

	CloseHandle(handle);

	return data;
}


void CopyToClipboard(const std::string& text)
{
	if (OpenClipboard(nullptr)) {
		EmptyClipboard();

		HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE, (text.length() + 1) * sizeof(wchar_t));
		if (hText != nullptr) {
			char* pText = static_cast<char*>(GlobalLock(hText));
			strcpy_s(pText, text.length() + 1, text.c_str());
			GlobalUnlock(hText);

			SetClipboardData(CF_TEXT, hText);
		}

		CloseClipboard();
	}
}