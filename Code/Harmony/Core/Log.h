#pragma once

#include "common.h"
#include "Utility.h"

#if defined(DEBUG)

#define _LOG_FORMAT "[%s] %s | %s | %s:%d | "
#define _LOG_ARGS utility::timenow().c_str(), utility::removeBasePath(__FILE__).c_str(), __FUNCTION__, __LINE__

// unnecessary helper macro to print a simple message to logs
#define DEBUGPRINT(message, ...) utility::printF(message "\n", ##__VA_ARGS__)

// prints a formatted message to logs "[category] time | fileName | function:line | message"
#define DEBUGLOG(category, message, ...) utility::printF(_LOG_FORMAT message "\n", category, _LOG_ARGS, ##__VA_ARGS__)

// DEBUGLOG ONCE
#define DEBUGLOG_ONCE(category, message, ...) \
{ \
static bool sTriggeredLog = false; \
if (!sTriggeredLog) \
{ \
    sTriggeredLog = true; \
    DEBUGLOG(category, message, ##__VA_ARGS__); \
} \
}

// DEBUGLOG IF condition is TRUE
#define DEBUGLOG_IF(condition, category, message, ...) \
{ \
if ((bool)(condition)) \
{ \
    DEBUGLOG(category, message, ##__VA_ARGS__); \
} \
}

// DEBUGLOG ONCE IF condition is TRUE
#define DEBUGLOG_ONCE_IF(condition, category, message, ...) \
{ \
if ((bool)(condition)) \
{ \
    DEBUGLOG_ONCE(category, message, ##__VA_ARGS__); \
} \
}


// triggers a breakpoint that stops execution and enter the debugger
#define BREAK __debugbreak()

// BREAK IF condition is TRUE
#define BREAK_IF(condition) if((bool)(condition)) BREAK


// BREAK and DEBUGLOG IF condition is FALSE
#define HASSERT_SOFT(condition, message, ...) \
{ \
if (!(bool)(condition)) \
{ \
    DEBUGLOG("SOFT ASSERT", message, ##__VA_ARGS__); \
    BREAK; \
} \
}

// BREAK and DEBUGLOG ONCE IF condition is FALSE
#define HASSERT_SOFT_ONCE(condition, message, ...) \
{ \
static bool sTriggeredLog = false; \
if (!(bool)(condition) && !sTriggeredLog) \
{ \
    sTriggeredLog = true; \
    DEBUGLOG("SOFT ASSERT", message, ##__VA_ARGS__); \
    BREAK; \
} \
}

// BREAK, DEBUGLOG and ABORT IF condition is FALSE
#define HASSERT(condition, message, ...) \
{ \
if (!(bool)(condition)) \
{ \
    DEBUGLOG("ASSERT", message, ##__VA_ARGS__); \
    BREAK; \
    std::abort(); \
} \
}

#else // defined(DEBUG)

#define DEBUGPRINT(message, ...) (void)(message)
#define DEBUGLOG(category, message, ...) (void)(category); (void)(message)
#define DEBUGLOG_ONCE(category, message, ...) (void)(category); (void)(message)
#define DEBUGLOG_IF(condition, category, message, ...) (void)(condition); (void)(category); (void)(message)
#define DEBUGLOG_ONCE_IF(condition, category, message, ...) (void)(condition); (void)(category); (void)(message)

#define BREAK
#define BREAK_IF(condition) (void)condition

#define HASSERT_SOFT(condition, message, ...) (void)(condition); (void)(message)
#define HASSERT_SOFT_ONCE(category, message, ...) (void)(condition); (void)(message)
#define HASSERT(condition, message, ...) (void)(condition); (void)(message); if (!(bool)(condition)) std::abort()

#endif // defined(DEBUG)
