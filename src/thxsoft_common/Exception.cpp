#include "Exception.h"

namespace thxsoft::common
{
    Exception::Exception(const char* message)
        : _message(std::string(message))
    {
    }

    Exception::Exception(std::string message)
        : _message(std::move(message))
    {
    }

    const char* Exception::what() const noexcept
    {
        return _message.c_str();
    }
}
