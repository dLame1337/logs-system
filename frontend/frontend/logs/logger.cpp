#include "logs/logger.h"
namespace logs {
	logger::logger(logsdir& dir)
	{
		dir_ = &dir;
	}

	logger::~logger()
	{
	}

	void logger::add(const char* log_level, const std::string& log)
	{
		dir_->add(log_level + log);
	}
}