#include "pch.hpp"

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3) {
        puts("usage: <file_path> [optional]<param>");
        return 0;
    }
    std::optional<file_data_s> data;
    
    try {
        data = getfile(argv[1]);

        if (!data.has_value())
            return 0;



    }
    catch (std::exception& ex) {
        
        MessageBox(NULL, ex.what(), "Error!", MB_ICONERROR);
        return 0;
    }

    auto& v = data.value();

    const auto time_created = filetime_to_text(v.lpCreationTime.get());
    const auto last_access = filetime_to_text(v.lpLastAccessTime.get());
    const auto last_written = filetime_to_text(v.lpLastWriteTime.get());
    const auto extension = file_extension(v.name);
    const auto real_name = v.name.substr(0, v.name.size() - (extension == "(no extension)" ? 0 : extension.size()+1));

    std::stringstream cb;

    cb << "```\n";
    cb << "full path:        " << v.where << '\n';
    cb << "name:             " << real_name << '\n';
    cb << "extension:        " << extension << '\n';
    cb << "size:             " << formatBytes(v.dwSize) << '\n';
    cb << "time created:     " << time_created<< '\n';
    cb << "last access:      " << last_access << '\n';
    cb << "last written:     " << last_written<< '\n';
    cb << "```";


    CopyToClipboard(cb.str());

    return 1;
}

