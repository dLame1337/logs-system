#include <iostream>
#include <chrono>
#include <iomanip>
#include <string>
#include <filesystem>
#include <sstream>
#include <thread>
#include "network/tcp.h"
#include "utils/file_manager.h"
#include "log.h"

namespace fs = std::filesystem;

std::string get_current_date() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm buf{};
    localtime_s(&buf, &in_time_t);

    std::ostringstream oss;
    oss << std::put_time(&buf, "%Y-%m-%d");
    return oss.str();
}

void save_logs(const std::string& logs) {
    std::string log_dir = "./logs";
    if (!fs::exists(log_dir)) {
        fs::create_directory(log_dir);
    }

    std::string filename = log_dir + "/" + get_current_date() + ".log";

    file_manager fm(filename);
    if (fm.is_open()) {
        fm.write(logs);
        LOGD("logs saved to " << filename);
    }
    else {
        LOGE("Failed to open log file: " << filename);
    }
}

int main() {
    network::tcp_server server(8080);

    if (!server.listen()) {
        LOGE("failed to start listening");
        return -1;
    }

    while (true) {
        SOCKET sock_fd = server.accept();
        LOGD("new client connected");
        
        std::thread([=] {
            std::string logs;
            while (true) {
                constexpr int buffer_size = 1024;
                char buffer[buffer_size] = { 0 };
                int result = recv(sock_fd, buffer, buffer_size - 1, 0);
                if (result < 1) break;
                logs.append(buffer, result);
            }
            LOGD("logs from client:\n" << logs);
            save_logs(logs);
        }).detach();
    }

    return 0;
}
