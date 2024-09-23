#pragma once

#include "Harmony.h"

#include <memory>

static const harmony::Logger* logger = HARMONY->createNewLogger();

// Helper function to create a formatted string
template<typename ... Args>
static std::unique_ptr<char[]> formatMessage(const char* format, Args ... args)
{
	int size = snprintf(nullptr, 0, format, args ...) + 1; // +1 for null terminator

	std::unique_ptr<char[]> buffer(new char[size]);

	snprintf(buffer.get(), size, format, args ...);

	return buffer;
}

#define HMESSAGE(format, ...) (formatMessage(format, ##__VA_ARGS__).get())

#define HLOG_DEBUG(format, ...)					logger->log(HMESSAGE(format, ##__VA_ARGS__), harmony::Logger::LogType::eDEBUG)
#define HLOG_INFO(format, ...)					logger->log(HMESSAGE(format, ##__VA_ARGS__), harmony::Logger::LogType::eINFO)
#define HLOG_WARNING(format, ...)				logger->log(HMESSAGE(format, ##__VA_ARGS__), harmony::Logger::LogType::eWARNING)
#define HLOG_ERROR(format, ...)					logger->log(HMESSAGE(format, ##__VA_ARGS__), harmony::Logger::LogType::eERROR)
#define HLOG_CRITICAL(format, ...)				logger->log(HMESSAGE(format, ##__VA_ARGS__), harmony::Logger::LogType::eCRITICAL)
#ifdef _DEBUG
	#define HSOFT_ASSERT(condition, format, ...)	logger->softAssert(condition, HMESSAGE(format, ##__VA_ARGS__)); if (!condition) __debugbreak()
	#define HASSERT(condition, format, ...)			logger->hardAssert(condition, HMESSAGE(format, ##__VA_ARGS__)); if (!condition) __debugbreak(); if (!condition) exit(-1)
#else
	#define HSOFT_ASSERT(condition, format, ...)	logger->softAssert(condition, HMESSAGE(format, ##__VA_ARGS__))
	#define HASSERT(condition, format, ...)			logger->hardAssert(condition, HMESSAGE(format, ##__VA_ARGS__)); if (!condition) exit(-1)
#endif // _DEBUG
