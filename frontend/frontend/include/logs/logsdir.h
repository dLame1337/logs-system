#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <nstd/list.h>

namespace logs {
	class logsdir {
	private:
		struct log {
			std::string time_stamp_;
			std::string log_;
		};

		nstd::list<int, log> logs_;

		int log_index = 0;
	public:
		logsdir();
		~logsdir();
		void add(const std::string& log);
		bool send_logs();
	};
}
