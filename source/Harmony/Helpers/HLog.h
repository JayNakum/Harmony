#pragma once

#include "Harmony.h"

static const harmony::Logger* logger = harmony::createLogger();

#define HLOG_DEBUG(message)			logger->log(message, harmony::Logger::LogType::eDEBUG)
#define HLOG_INFO(message)			logger->log(message, harmony::Logger::LogType::eINFO)
#define HLOG_WARNING(message)		logger->log(message, harmony::Logger::LogType::eWARNING)
#define HLOG_ERROR(message)			logger->log(message, harmony::Logger::LogType::eERROR)
#define HLOG_CRITICAL(message)		logger->log(message, harmony::Logger::LogType::eCRITICAL)
#define HASSERT(condition, message)	logger->softAssert(condition, message)