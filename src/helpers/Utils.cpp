//
// Created by Ned on 23/03/2026.
//
#include "Utils.h"
#include <iostream>

std::string get_filename(std::string path)
{
    bool nameFound = false;

    std::string filename;

    for (int i = 0; i < 2; i++)
    {
        constexpr char delimiters[] = {
            '/',
            '\\'
        };
        // if last character is a delimiter, get rid of it
        if (path[path.length() - 1] == delimiters[i])
        {
            path = path.substr(0, path.length() - 1);
        }
        // find last delimiter, everything after that is the file name
        size_t pos = path.find_last_of(delimiters[i]);
        if (pos == std::string::npos)
        {
            continue;
        }
        // get the substring of the file name
        filename = path.substr(pos+1, path.length());
        nameFound = true;
    }

    if (!nameFound)
    {
        std::cerr << "Failed to parse file name, is the path valid?" << std::endl;
    }

    return filename;
}
