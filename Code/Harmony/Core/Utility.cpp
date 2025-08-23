#include "Utility.h"

#include "common.h"

namespace utility
{
    std::string toLower(const std::string& str)
    {
        std::string lower_case = str;
        std::locale loc;
        for (char& s : lower_case)
            s = std::tolower(s, loc);
        return lower_case;
    }

    std::string timenow() noexcept
    {
        time_t m_time = time(nullptr);
        struct tm now = {};

        localtime_s(&now, &m_time);

        char buffer[64];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &now);

        return std::string(buffer);
    }

}
