#pragma once

#include <source_location>

namespace harmony
{
	class Logger
	{
	public:
		enum LogType
		{
			eASSERT = 0,
			eCRITICAL = 1,
			eERROR = 2,
			eSOFT_ASSERT = 3,
			eWARNING = 3,
			eINFO = 4,
			eDEBUG = 5,
			eNO_LOG
		};
		using LogLevel = LogType;

		enum LogMode
		{
			eCONSOLE,
			eFILE,
			eNETWORK
		};

	public:
		struct Description
		{
			LogLevel	level	= LogLevel::eDEBUG;
			LogMode		mode	= LogMode::eCONSOLE;
		};

		virtual ~Logger() = default;

	public:
		virtual void setLogLevel(const LogLevel& level) = 0;

		virtual void initialize() = 0;
		virtual void deinitialize() = 0;

		virtual void log(const char* message, const LogType& type = LogType::eDEBUG, const std::source_location& _src = std::source_location::current()) const = 0;
		virtual void softAssert(bool condition, const char* message, const std::source_location& _src = std::source_location::current()) const = 0;
		virtual void hardAssert(bool condition, const char* message, const std::source_location& _src = std::source_location::current()) const = 0;
	};
}
