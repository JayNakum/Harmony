#include "Logger.h"

#include <iostream>

#ifdef SOKOL

#include <sokol_log.h>

#elif DX12

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <string>
#include <sstream>

#endif


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
	if (mDescription.level >= LogLevel::eDEBUG && type == LogType::eDEBUG)
	{
		consoleLog(LABEL_DEBUG, message, _src);
	}

	else if (mDescription.level >= LogLevel::eINFO && type == LogType::eINFO)
	{
		consoleLog(LABEL_INFO, message, _src);
	}

	else if (mDescription.level >= LogLevel::eWARNING && type == LogType::eWARNING)
	{
		consoleLog(LABEL_WARNING, message, _src);
	}

	else if (mDescription.level >= LogLevel::eERROR && type == LogType::eERROR)
	{
		consoleLog(LABEL_ERROR, message, _src);
	}

	else if (mDescription.level >= LogLevel::eCRITICAL && type == LogType::eCRITICAL)
	{
		consoleLog(LABEL_CRITICAL, message, _src);
	}
}

void Logger::softAssert(bool condition, const char* message, const std::source_location& _src) const
{
	if (!condition)
	{
		consoleLog(LABEL_WARNING, message, _src);
	}
}

void Logger::hardAssert(bool condition, const char* message, const std::source_location& _src) const
{
	if (!condition)
	{
		consoleLog(LABEL_ASSERT, message, _src);
	}
}

void Logger::consoleLog(const char* label, const char* message, const std::source_location& _src) const
{
#ifdef SOKOL
	slog_func(label, mDescription.level, 0, message, _src.line(), _src.file_name(), nullptr);

#elif DX12

	std::ostringstream oss;
	oss << "["
		<< label
		<< "]\t"
		<< timenow() << " | "
		<< _src.file_name() << " | "
		<< _src.function_name() << ":"
		<< _src.line() << " | "
		<< message
		<< END_LOG << std::endl;

	OutputDebugString(oss.str().c_str());
	oss.clear();

#else

	std::cout
		<< label
		<< timenow() << " | "
		<< _src.file_name() << " | "
		<< _src.function_name() << ":"
		<< _src.line() << " | "
		<< message
		<< END_LOG << std::endl;

#endif
}
