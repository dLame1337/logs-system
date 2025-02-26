#pragma once
#include "logs/logger.h"

void logs_test() {
	logs::logsdir dir;

	logs::logger log(dir);

	log.add(logs::D, "this is an debug log");
	log.add(logs::I, "this is an info log");
	log.add(logs::W, "this is an warning log");
	log.add(logs::E, "this is an error log");

	dir.send_logs();

	log.add(logs::E, "this is an error log");

	dir.send_logs();
}