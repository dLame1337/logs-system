#include "utils/file_manager.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

file_manager::file_manager(const std::string& path) : file_path(path) {
    file.open(file_path, std::ios::in | std::ios::out | std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Warning: Failed to open file: " << file_path << std::endl;
    }
}

file_manager::~file_manager() {
    if (file.is_open()) {
        file.close();
    }
}

bool file_manager::is_open() const {
    return file.is_open();
}

bool file_manager::write(const std::string& data) {
    if (!file.is_open()) return false;
    file.clear();
    file.seekp(0, std::ios::end);
    file << data << std::endl;
    return true;
}

std::string file_manager::read() {
    if (!file.is_open()) return "";

    file.clear();
    file.seekg(0, std::ios::beg);

    std::string content, line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }

    return content;
}

bool file_manager::clear() {
    if (file.is_open()) file.close();
    file.open(file_path, std::ios::out | std::ios::trunc);
    return file.is_open();
}

bool file_manager::exists() const {
    return fs::exists(file_path);
}

bool file_manager::remove() {
    if (exists()) {
        file.close();
        return fs::remove(file_path);
    }
    return false;
}
