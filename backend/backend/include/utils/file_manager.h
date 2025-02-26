#pragma once
#include <fstream>
#include <string>

class file_manager {
private:
    std::string file_path;
    std::fstream file;

public:
    file_manager(const std::string& path);
    ~file_manager();

    bool is_open() const;
    bool write(const std::string& data);
    std::string read();
    bool clear();
    bool exists() const;
    bool remove();
};
