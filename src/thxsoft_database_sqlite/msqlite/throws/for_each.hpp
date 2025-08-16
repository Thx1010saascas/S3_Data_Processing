#pragma once

#include "thxsoft_database_sqlite/msqlite/for_each.hpp"
#include "thxsoft_database_sqlite/msqlite/throws/value_or_throw.hpp"

namespace msqlite::throws {

template<typename F>
inline void for_each(stmt& stmt, F&& f)
{ return value_or_throw(::msqlite::for_each(stmt, std::forward<F>(f))); }

}
