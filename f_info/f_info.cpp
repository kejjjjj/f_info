#include "pch.hpp"

int main(int argc, char** argv)
{
    if (argc != 3) {
        puts("usage: <file_path> <argument>");
        system("pause");
        return 0;
    }

    
    try {
        
        if (!file_exists(argv[1])) {
            throw std::exception(std::format("{} is not a valid file!", argv[1]).c_str());
        }

        auto it = tool_features.find(argv[2]);

        if (it == tool_features.end())
            throw std::exception(std::format("\"{}\" is not a valid argument", argv[1]).c_str());

        if(const auto v = it->second(std::string(argv[1])))
            CopyToClipboard(v.value());

    }
    catch (std::exception& ex) {
        
        MessageBox(NULL, ex.what(), "Error!", MB_ICONERROR);
        return 0;
    }

    return 1;
}

