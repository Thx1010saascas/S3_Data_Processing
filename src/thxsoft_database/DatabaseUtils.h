#pragma once
#include <format>
#include <string>

namespace thxsoft::database
{
    struct DatabaseUtils {
        static std::string makeDbColumnValue(const std::string& value);

        template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
        static std::string asDbString(T value)
        {
            return std::format("{}", value);
        }
        template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
        static std::string asDbString(T value)
        {
            return std::to_string(value);
        }
        template <typename T, std::enable_if_t<std::is_same_v<T,std::string>, int> = 0>
        static std::string asDbString(T value)
        {
            return makeDbColumnValue(value);
        }
        template <typename T, std::enable_if_t<std::is_pointer_v<T>, int> = 0>
        static std::string asDbString(T value)
        {
            if(value == nullptr)
                return "null";

            return std::to_string(*value);
        }
        template <typename T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
        static std::string asDbString(T value)
        {
            return std::to_string(static_cast<int>(value));
        }
    };
}
