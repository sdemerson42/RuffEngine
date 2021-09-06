#include "pch.h"
#include "Logger.h"

#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>

namespace util
{
	const char* Logger::logFileName = "RuntimeLog.txt";

	// TODO: Allow for multiple logs
	Logger::Logger()
	{
		
	}

	void Logger::Log(const std::string& msg)
	{
		const auto chronoTime = std::chrono::system_clock::now();
		const auto cTime = std::chrono::system_clock::to_time_t(chronoTime);
		const std::string formatMsg = ": " + msg + '\n';

		//std::ofstream logStream{ logFileName, std::ios_base::app };
		//logStream << formatMsg;
		std::cout << formatMsg;

		//logStream.close();
	}
}