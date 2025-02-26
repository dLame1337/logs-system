#include "logs/logsdir.h"
#include "network/tcp.h"
#include "log.h"
namespace logs {
	logsdir::logsdir()
	{
	}

	logsdir::~logsdir()
	{
	}

	void logsdir::add(const std::string& log)
	{
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);

		std::ostringstream oss;
		oss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");

		logs_.insert(log_index, { oss.str(), log });
		log_index++;
	}

	bool logsdir::send_logs()
	{
		std::string result = "";

		for (const auto& [key_, value_] : logs_) {
			result = value_.time_stamp_ + " " + value_.log_ + "\n" + result;
		}

		LOGI("sending logs: \n" << result);

		network::tcp_client client("127.0.0.1", 8080);
		if (!client.connect()) {
			LOGE("failed connect to server");
			return false;
		}

		if (client.send(result.c_str(), result.length() + 1) != (result.length() + 1)) {
			LOGE("failed to sent data to server");
			return false;
		}

		logs_.clear();
		log_index = 0;
		return true;
	}
}