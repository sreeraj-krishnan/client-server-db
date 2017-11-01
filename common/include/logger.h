#pragma once

#include <spdlog/spdlog.h>

class Logger
{
	public:

	static std::shared_ptr<spdlog::logger>& log();

	private:

	Logger();
	
};
