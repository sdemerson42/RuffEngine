#pragma once

#include <string>

namespace util
{
	// Default to logging console and file for now
	class Logger
	{
	public:
		Logger();
		static void Log(const std::string& msg);
	private:
		static const char* logFileName;
	};
}
