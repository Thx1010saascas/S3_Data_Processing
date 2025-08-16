#pragma once

#include <functional>
#include <optional>
#include <sqlite3.h>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

namespace msqlite::detail
{
    template <typename Tuple, typename F, std::size_t... Idx>
    inline constexpr void for_each_impl(Tuple&& t, F&& f, std::index_sequence<Idx...> idx)
    {
        (std::invoke(std::forward<F>(f), std::get<Idx>(std::forward<Tuple>(t))), ...);
    }

    template <typename Tuple, typename F>
    inline constexpr void for_each(Tuple&& t, F&& f)
    {
        for_each_impl(std::forward<Tuple>(t),
                      std::forward<F>(f),
                      std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
    }

    inline void bind(sqlite3_stmt* stmt, std::size_t idx, std::string_view s)
    {
        if (s.empty())
            sqlite3_bind_text(stmt, idx, nullptr, 0, SQLITE_STATIC);
        else
            sqlite3_bind_text(stmt, idx, s.data(), s.size(), SQLITE_STATIC);
    }

    template <typename T> requires(std::is_same_v<T, std::string>)
    inline void bind(sqlite3_stmt* stmt, std::size_t idx, T s)
    {
        if (s.empty())
            sqlite3_bind_text(stmt, idx, nullptr, 0, SQLITE_TRANSIENT);
        else
            sqlite3_bind_text(stmt, idx, s.c_str(), s.size(), SQLITE_TRANSIENT);
    }

    inline void bind(sqlite3_stmt* stmt, std::size_t idx, std::string& s)
    {
        if (s.empty())
            sqlite3_bind_text(stmt, idx, nullptr, 0, SQLITE_STATIC);
        else
            sqlite3_bind_text(stmt, idx, s.c_str(), s.size(), SQLITE_STATIC);
    }

    inline void bind(sqlite3_stmt* stmt, std::size_t idx, std::nullptr_t)
    {
        sqlite3_bind_null(stmt, idx);
    }

    inline void bind(sqlite3_stmt* stmt, std::size_t idx, bool v)
    {
        sqlite3_bind_int(stmt, idx, v);
    }

    inline void bind(sqlite3_stmt* stmt, std::size_t idx, int v)
    {
        sqlite3_bind_int(stmt, idx, v);
    }
    inline void bind(sqlite3_stmt* stmt, std::size_t idx, std::optional<int> v)
    {
        if(v.has_value())
            sqlite3_bind_int(stmt, idx, v.value());
        else
            sqlite3_bind_null(stmt, idx);
   }

    inline void bind(sqlite3_stmt* stmt, std::size_t idx, long long v)
    {
        sqlite3_bind_int64(stmt, idx, v);
    }
    inline void bind(sqlite3_stmt* stmt, std::size_t idx, std::optional<long long> v)
    {
        if(v.has_value())
            sqlite3_bind_int64(stmt, idx, v.value());
        else
            sqlite3_bind_null(stmt, idx);
    }


    inline void bind(sqlite3_stmt* stmt, std::size_t idx, float v)
    {
        sqlite3_bind_double(stmt, idx, v);
    }
    inline void bind(sqlite3_stmt* stmt, std::size_t idx, std::optional<float> v)
    {
        if(v.has_value())
            sqlite3_bind_double(stmt, idx, v.value());
        else
            sqlite3_bind_null(stmt, idx);
    }


    inline void bind(sqlite3_stmt* stmt, std::size_t idx, double v)
    {
        sqlite3_bind_double(stmt, idx, v);
    }
    inline void bind(sqlite3_stmt* stmt, std::size_t idx, std::optional<double> v)
    {
        if(v.has_value())
            sqlite3_bind_double(stmt, idx, v.value());
        else
            sqlite3_bind_null(stmt, idx);
    }


    template <typename Binds>
    inline void bind_params(sqlite3_stmt* stmt, Binds&& binds)
    {
        for_each(std::forward<Binds>(binds),
                 [=, idx = std::size_t{0}](auto&& e) mutable
                 {
                     bind(stmt, ++idx, std::forward<decltype(e)>(e));
                 });
    }
}
