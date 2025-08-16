#pragma once
#include <string>
#include <exception>

namespace thxsoft::common
{
    class Exception final : public std::exception
    {
        std::string _message;

    public:
        explicit Exception(std::string message);
        explicit Exception(const char* message);

        [[nodiscard]] const char* what() const noexcept override;
    };
}