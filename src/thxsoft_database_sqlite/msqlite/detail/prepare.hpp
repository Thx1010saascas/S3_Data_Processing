#pragma once

#include "thxsoft_database_sqlite/msqlite/db.hpp"
#include "thxsoft_database_sqlite/msqlite/cache/db.hpp"
#include "thxsoft_database_sqlite/msqlite/detail/prepare_impl.hpp"
#include "thxsoft_database_sqlite/msqlite/result.hpp"
#include "thxsoft_database_sqlite/msqlite/stmt.hpp"

#include <sqlite3.h>

namespace msqlite::detail {

template<typename... Binds>
inline result<stmt> prepare(const db& conn, std::string_view s, std::tuple<Binds...> binds)
{ return prepare_impl(conn, s, std::move(binds)); }

template<typename... Binds>
inline result<stmt> prepare(const db* conn, std::string_view s, std::tuple<Binds...> binds)
{ return prepare_impl(conn, s, std::move(binds)); }

template<typename... Binds>
inline result<stmt*> prepare(cache::db& db_, std::string_view stmt, std::tuple<Binds...> binds)
{ return db_.prepare(stmt, std::move(binds)); }

}
