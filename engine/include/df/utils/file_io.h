#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace DF::File
{
    std::string read(const std::string& path)
    {
        std::ifstream file{ path };
        std::stringstream fileStream{};

        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            if (!file) throw std::runtime_error("Cannot open file " + path);

            fileStream << file.rdbuf();
            file.close();
        }
        catch (std::runtime_error& error)
        {
            std::cout << "Error while reading a file: " << path << " *** " << error.what() << "***\n";
        }
        catch (...)
        {
            std::cout << "Unknown error while reading a file " << path << '\n';
        }

        return fileStream.str();
    }
}
