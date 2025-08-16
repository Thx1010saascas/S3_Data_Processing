#pragma once

#include <memory>

#include "thxsoft_database_sqlite/msqlite/stmt.hpp"

#include <variant>
#include <sqlite3.h>
#include <string_view>
#include <string>

namespace msqlite::detail
{
    struct param : std::variant<std::nullptr_t, bool, std::shared_ptr<int>, std::shared_ptr<long long>, std::shared_ptr<float>, std::shared_ptr<double>, int, long long, double, float, std::string_view>
    {
        using base = std::variant<std::nullptr_t, bool, std::shared_ptr<int>, std::shared_ptr<long long>, std::shared_ptr<float>, std::shared_ptr<double>, int, long long, double, float, std::string_view>;
        using base::base;

        //TODO: These function can't throw an exception that will not be
        //catched by value().
        operator bool() const
        {
            return static_cast<bool>(std::get<long long>(*this));
        }

        operator std::shared_ptr<int>() const
        {
            if(index() == 0)
                return nullptr;

            return std::make_shared<int>(static_cast<int>(std::get<long long>(*this)));
        }

        operator std::shared_ptr<double>() const
        {
            if(index() == 0)
                return nullptr;

            return std::make_shared<double>(std::get<double>(*this));
        }

        operator std::shared_ptr<float>() const
        {
            if(index() == 0)
                return nullptr;

            return std::make_shared<float>(std::get<float>(*this));
        }

        operator std::shared_ptr<long long>() const
        {
            if(index() == 0)
                return nullptr;

            return std::make_shared<long long>(std::get<long long>(*this));
        }

        operator int() const
        {
            return static_cast<int>(std::get<long long>(*this));
        }
        operator long long() const
        {
            return std::get<long long>(*this);
        }

        operator double() const
        {
            return std::get<double>(*this);
        }

        operator float() const
        {
            return static_cast<float>(std::get<double>(*this));
        }

        operator std::string_view() const
        {
            if(index() == 0)
                return "";

            return std::get<std::string_view>(*this);
        }

        operator std::string() const
        {
            if(index() == 0)
                return "";

            return std::string{std::get<std::string_view>(*this)};
        }
    };

    inline param value(sqlite3_stmt* stmt, int i)
    {
        switch (sqlite3_column_type(stmt, i))
        {
        case SQLITE_INTEGER:
            return sqlite3_column_int64(stmt, i);
        case SQLITE_FLOAT:
            return sqlite3_column_double(stmt, i);
        case SQLITE_NULL:
            return nullptr;
        default:
            {
                auto s = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                return s ? s : "";
            }
        }
    }

    template <typename F>
    struct arity : arity<decltype(&F::operator())>
    {
    };

    template <typename F, typename Ret, typename... Args>
    struct arity<Ret(F::*)(Args...) const>
    {
        static const int value = sizeof...(Args);
    };

    template <typename F, std::size_t... idx>
    auto call(F&& f, sqlite3_stmt* stmt, std::index_sequence<idx...>)
    {
        return f(value(stmt, idx)...);
    }

    template <typename F, typename... Args>
    auto call_once_impl(F&& f, Args&&... args)
    {
        return f(std::forward<Args>(args)...);
    }

    template <typename F, std::size_t... idx>
    auto call_once(F&& f, stmt& stmt, std::index_sequence<idx...>)
    {
        return call_once_impl(std::forward<F>(f), value(stmt.get(), idx)...);
    }
}
