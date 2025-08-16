#pragma once

#include "thxsoft_database_sqlite/msqlite/result.hpp"
#include "thxsoft_database_sqlite/msqlite/pipes/context.hpp"
#include "thxsoft_database_sqlite/msqlite/pipes/detail/reset.hpp"
#include "thxsoft_database_sqlite/msqlite/stmt.hpp"

#include "thxsoft_database_sqlite/msqlite/detail/reset.hpp"

namespace msqlite {

//Resets a prepared statement 'stmt'
inline result<void> reset(stmt& stmt) noexcept
{ return detail::reset(stmt); }

//** Below is the API to use pipe operators to chain functions **

//Constructs a lazy reset to be chained with a pipe operator. 
inline detail::reset_wrapper reset() noexcept
{ return {}; }

//Chains a prepared statement to a lazy reset
//
//This overload matches an expression like 'result<stmt> | reset()'. The
//left hand side operand should be a result<stmt> and the right hand
//side operand should be `reset`. If there is a prepared statement
//into 'result<stmt>', the statement is reseted. If there is an error
//in the 'result<stmt>', it is propagated to next element in the chain
//until someone catch it. Example:
//
//  auto r = open("person.db")
//  | step("select name, salary from person")
//  | reset();
//  if(!r) std::cout << r.error() << std::endl;
//
inline auto operator|(Context auto&& ctx, detail::reset_wrapper o) noexcept
{ return detail::pipe(std::forward<decltype(ctx)>(ctx), o); }

}
