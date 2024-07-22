#include "Logger.h"

#include <iostream>

Logger::Logger(const harmony::Logger::Description description)
	: mDescription(description) {}

Logger::~Logger()
{
	mDescription = {};
}

void Logger::setLogLevel(const LogLevel& level)
{
	mDescription.level = level;
}

void Logger::initialize()
{}

void Logger::deinitialize()
{}

void Logger::log(const char* message, const LogType & type, const std::source_location& _src) const
{
	if (mDescription.level <= LogLevel::eDEBUG && type == LogType::eDEBUG)
	{
		consoleLog(COLOR_DEBUG, message, _src);
	}

	else if (mDescription.level <= LogLevel::eINFO && type == LogType::eINFO)
	{
		consoleLog(COLOR_INFO, message, _src);
	}

	else if (mDescription.level <= LogLevel::eWARNING && type == LogType::eWARNING)
	{
		consoleLog(COLOR_WARNING, message, _src);
	}

	else if (mDescription.level <= LogLevel::eERROR && type == LogType::eERROR)
	{
		consoleLog(COLOR_ERROR, message, _src);
	}

	else if (mDescription.level <= LogLevel::eCRITICAL && type == LogType::eCRITICAL)
	{
		consoleLog(COLOR_CRITICAL, message, _src);
	}
}

void Logger::softAssert(bool condition, const char* message, const std::source_location& _src) const
{
	if (!condition)
	{
		consoleLog(COLOR_WARNING, message, _src);
	}
}

void Logger::hardAssert(bool condition, const char* message, const std::source_location& _src) const
{
	if (!condition)
	{
		consoleLog(COLOR_ASSERT, message, _src);
	}
}

void Logger::consoleLog(const char* color, const char* message, const std::source_location& _src) const
{
	std::cout
		<< color
		<< timenow() << " | "
		<< _src.file_name() << " | "
		<< _src.function_name() << ":"
		<< _src.line() << " | "
		<< message
		<< END_LOG << std::endl;
}
