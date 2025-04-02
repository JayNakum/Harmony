#pragma once

#include "Utility.h"

#include "win.h"

#if !defined(RELEASE) // TODO: Remove the ifdef once file logging is implemented because project will support file logging in RELEASE as well.

#define _LOG_FORMAT "[%s] %s | %s | %s:%d | "
#define _LOG_ARGS utility::timenow().c_str(), utility::removeBasePath(__FILE__).c_str(), __FUNCTION__, __LINE__

// unnecessary helper macro to print a simple message to logs
#define DEBUGPRINT(message, ...) utility::printF(message "\n", ##__VA_ARGS__)

// prints a formatted message to logs "[category] time | fileName | function:line | messsage"
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

// BREAK IF conditoin is TRUE
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

// BREAK and DEBUGLOG_ONCE IF condition is FALSE
#define HASSERT_SOFT_ONCE(category, message, ...) \
{ \
static bool sTriggeredLog = false; \
if (!sTriggeredLog) \
{ \
    sTriggeredLog = true; \
    HASSERT_SOFT(category, message, ##__VA_ARGS__); \
} \
}

// HASSERT_SOFT if SUCCEEDED(hResult)
#define HASSERT_SOFT_HR(hResult, message, ...) HASSERT_SOFT(SUCCEEDED((HRESULT)(hResult)), message, ##__VA_ARGS__); if(FAILED((HRESULT)(hResult))) DEBUGLOG("ASSERT", "%s", utility::translateErrorCode(hResult).get())

// HASSERT_SOFT_ONCE if SUCCEEDED(hResult)
#define HASSERT_SOFT_HR_ONCE(hResult, message, ...) HASSERT_SOFT_ONCE(SUCCEEDED((HRESULT)(hResult)), message, ##__VA_ARGS__); if(FAILED((HRESULT)(hResult))) DEBUGLOG("ASSERT", "%s", utility::translateErrorCode(hResult).get())

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

// HASSERT if SUCCEEDED(hResult)
#define HASSERT_HR(hResult, message, ...) HASSERT(SUCCEEDED((HRESULT)(hResult)), message, ##__VA_ARGS__); if(FAILED((HRESULT)(hResult))) DEBUGLOG("ASSERT", "%s", utility::translateErrorCode(hResult).get())

#else

#define DEBUGPRINT(message, ...) (void)(message)
#define DEBUGLOG(category, message, ...) (void)(category); (void)(message)
#define DEBUGLOG_ONCE(category, message, ...) (void)(category); (void)(message)
#define DEBUGLOG_IF(condition, category, message, ...) (void)(condition); (void)(category); (void)(message)
#define DEBUGLOG_ONCE_IF(condition, category, message, ...) (void)(condition); (void)(category); (void)(message)

#define BREAK
#define BREAK_IF(condition) (void)condition

#define HASSERT_SOFT(condition, message, ...) (void)(condition); (void)(message)
#define HASSERT_SOFT_HR(hResult, message, ...) (void)(hResult); (void)(message)

#define HASSERT(condition, message, ...) (void)(condition); (void)(message); if (!(bool)(condition)) std::abort()
#define HASSERT_HR(hResult, message, ...) (void)(hResult); (void)(message); if (FAILED((HRESULT)(hResult))) std::abort()

#endif // !defined(RELEASE)
