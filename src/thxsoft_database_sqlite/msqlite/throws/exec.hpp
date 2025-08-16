#pragma once

#include "thxsoft_database_sqlite/msqlite/exec.hpp"
#include "thxsoft_database_sqlite/msqlite/throws/error.hpp"

namespace msqlite::throws {

//Take a look at 'msqlite/exec.hpp' to see more details.
//
//These are alternatives to msqlite::exec that throws
//std::system_error instead of returning result<T>.

template<typename... Values>
inline void exec(const db& db, std::string_view stmt, Values&&... values)
{ return value_or_throw(::msqlite::exec(db, stmt, std::forward<Values>(values)...)); }

template<typename... Values>
inline void exec(const db* db, std::string_view stmt, Values&&... values)
{ return value_or_throw(::msqlite::exec(db, stmt, std::forward<Values>(values)...)); }

template<typename... Values>
inline void exec(stmt& stmt, Values&&... values) {
    return value_or_throw(
        ::msqlite::exec(stmt, std::forward<Values>(values)...));
}

template<typename... Values>
inline void exec(cache::db& db, std::string_view stmt, Values&&... values) {
    return value_or_throw(
        ::msqlite::exec(db, stmt, std::forward<Values>(values)...));
}

}
