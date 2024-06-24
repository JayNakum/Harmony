#pragma once

#include "Harmony.h"

#include <memory>

static const harmony::Logger* logger = harmony::createLogger();

// Helper function to create a formatted string
template<typename ... Args>
static std::unique_ptr<char[]> formatMessage(const char* format, Args ... args)
{
	int size = snprintf(nullptr, 0, format, args ...) + 1; // +1 for null terminator

	std::unique_ptr<char[]> buffer(new char[size]);

	snprintf(buffer.get(), size, format, args ...);

	return buffer;
}

#define HLOG_DEBUG(format, ...)					logger->log(formatMessage(format, ##__VA_ARGS__).get(), harmony::Logger::LogType::eDEBUG)
#define HLOG_INFO(format, ...)					logger->log(formatMessage(format, ##__VA_ARGS__).get(), harmony::Logger::LogType::eINFO)
#define HLOG_WARNING(format, ...)				logger->log(formatMessage(format, ##__VA_ARGS__).get(), harmony::Logger::LogType::eWARNING)
#define HLOG_ERROR(format, ...)					logger->log(formatMessage(format, ##__VA_ARGS__).get(), harmony::Logger::LogType::eERROR)
#define HLOG_CRITICAL(format, ...)				logger->log(formatMessage(format, ##__VA_ARGS__).get(), harmony::Logger::LogType::eCRITICAL)
#define HSOFT_ASSERT(condition, format, ...)		logger->softAssert(condition, formatMessage(format, ##__VA_ARGS__).get())
#define HASSERT(condition, format, ...)			logger->hardAssert(condition, formatMessage(format, ##__VA_ARGS__).get())