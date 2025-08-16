#pragma once

#include "thxsoft_database_sqlite/msqlite/step.hpp"
#include "thxsoft_database_sqlite/msqlite/throws/value_or_throw.hpp"

namespace msqlite::throws {

template<typename F,
         typename T = detail::result_of_F<F> >
inline T step(stmt& stmt, F&& f)
{ return value_or_throw(::msqlite::step(stmt, std::forward<F>(f))); }

}
