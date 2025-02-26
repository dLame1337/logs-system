#pragma once
#include "logsdir.h"

namespace logs {
	static const char* D = " [D] ";
	static const char* I = " [I] ";
	static const char* W = " [W] ";
	static const char* E = " [E] ";

	class logger { // базовий клас для логування
	private:
		logsdir* dir_;

	public:
		logger(logsdir& dir);

		virtual ~logger();
		virtual void add(const char* log_level, const std::string& log);
	};
}