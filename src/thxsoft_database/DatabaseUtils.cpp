#include "DatabaseUtils.h"
#include "Thx.h"

namespace thxsoft::database
{
    std::string DatabaseUtils::makeDbColumnValue(const std::string& value)
    {
        if(value.empty())
            return "null";

        return "'" + Thx::escapeSingleQuote(value) + "'";
    }
}