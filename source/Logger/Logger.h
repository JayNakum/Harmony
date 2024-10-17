#pragma once

#include "HLogger.h"

#include <ctime>

class Logger : public harmony::Logger
{
public:
	Logger(const harmony::Logger::Description mDescription);
	~Logger();

	void setLogLevel(const LogLevel& level) override;

	void initialize() override;
	void deinitialize() override;

	void log(const char* message, const LogType& type = LogType::eDEBUG, const std::source_location& _src = std::source_location::current()) const override;
	void softAssert(bool condition, const char* message, const std::source_location& _src = std::source_location::current()) const override;
	void hardAssert(bool condition, const char* message, const std::source_location& _src = std::source_location::current()) const override;

private:
	void consoleLog(const char* label, const char* message, const std::source_location& _src) const;

	static inline char* timenow()
	{
		static char buffer[64];

		time_t m_time = time(NULL);
		struct tm now = {};

		localtime_s(&now, &m_time);

		strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", &now);

		return buffer;
	}

private:
	harmony::Logger::Description mDescription;
	

#ifdef SOKOL
	const char* LABEL_DEBUG		= "DEBUG";
	const char* LABEL_INFO		= "INFO";
	const char* LABEL_WARNING	= "WARNING";
	const char* LABEL_ERROR		= "ERROR";
	const char* LABEL_CRITICAL	= "CRITICAL";
	const char* LABEL_SOFTASSERT= "SOFT ASSERT";
	const char* LABEL_ASSERT	= "ASSERT";
	const char* END_LOG			= "";
#elif DX12
	const char* LABEL_DEBUG		= "DEBUG";
	const char* LABEL_INFO		= "INFO";
	const char* LABEL_WARNING	= "WARNING";
	const char* LABEL_ERROR		= "ERROR";
	const char* LABEL_CRITICAL	= "CRITICAL";
	const char* LABEL_SOFTASSERT= "SOFT ASSERT";
	const char* LABEL_ASSERT	= "ASSERT";
	const char* END_LOG			= "";
#endif // SOKOL


	//const char* COLOR_DEBUG		= "\x1B[37m";
	//const char* COLOR_INFO		= "\x1B[32m";
	//const char* COLOR_WARNING	= "\x1B[33m";
	//const char* COLOR_ERROR		= "\x1B[31m";
	//const char* COLOR_CRITICAL	= "\x1B[43m\x1B[30m";
	//const char* COLOR_ASSERT	= "\x1B[41m\x1B[30m";
	//const char* END_LOG			= "\x1B[00m\n";
};
