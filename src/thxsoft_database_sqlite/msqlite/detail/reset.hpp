#pragma once

#include "thxsoft_database_sqlite/msqlite/result.hpp"
#include "thxsoft_database_sqlite/msqlite/stmt.hpp"

#include <sqlite3.h>

namespace msqlite::detail {

inline result<void> reset(stmt& stmt) {
    if(auto rc = sqlite3_reset(stmt.get()))
        return std::error_code{error{rc}};
    return {};
}

}
