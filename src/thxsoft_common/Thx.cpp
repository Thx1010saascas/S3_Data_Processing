#include "Thx.h"
#include <cmath>
#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>

std::string Thx::getDuration(const std::shared_ptr<time_t>& startTime, const std::shared_ptr<time_t>& endTime, const DurationLabelSize durationLabelSize)
{
    if(startTime == nullptr || endTime == nullptr)
        return "";

    return getDuration(*startTime, *endTime, durationLabelSize);
}

std::string Thx::getDuration(const time_t startTime, const time_t endTime, const DurationLabelSize durationLabelSize)
{
    if(endTime == 0)
        return "";

    const auto start = std::chrono::system_clock::from_time_t(startTime);
    const auto end = std::chrono::system_clock::from_time_t(endTime);
    const auto diff = std::chrono::round<std::chrono::milliseconds>(end - start).count();

    return Thx::toDurationString(diff, durationLabelSize);
}

std::optional<long long> Thx::toInt64(const std::string& value)
{
    std::optional<long long> optValue;

    if(!value.empty())
        optValue.emplace(stoll(value));

    return optValue;
}

std::optional<int> Thx::toInt(const std::string& value)
{
    std::optional<int> optValue;

    if(!value.empty())
        optValue.emplace(stoi(value));

    return optValue;
}

std::optional<double> Thx::toDouble(const std::string& value)
{
    std::optional<double> optValue;

    if(!value.empty())
        optValue.emplace(stod(value));

    return optValue;
}

long long Thx::stoll(const std::string& value, const long long defaultValue)
{
    if(!value.empty())
        return std::stoll(value);

    return defaultValue;
}

int Thx::stoi(const std::string& value, const int defaultValue)
{
    if(!value.empty())
        return std::stoi(value);

    return defaultValue;
}

double Thx::stod(const std::string& value, const double defaultValue)
{
    if(!value.empty())
        return std::stod(value);

    return defaultValue;
}

int Thx::indexOfAny(const std::string& str, const std::unordered_set<char>& chars)
{
    for(size_t i = 0; i < str.length(); ++i)
    {
        if(chars.contains(str[i]))
        {
            return i;
        }
    }
    return -1;
}

time_t Thx::nowUtc()
{
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

time_t Thx::nowUtcMs()
{
    return std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
}

time_t Thx::timeToFileTimeMultiplier(const time_t time)
{
    return (time * 10000000LL) + 116444736000000000LL;
}

void Thx::setCustomLocale()
{
    const std::locale fallbackLocal(std::cout.getloc(), new LocaleNumSeparated);
    std::locale::global(fallbackLocal);
}

std::string Thx::toLower(const std::string& text)
{
    auto t = std::string(text);

    std::ranges::transform(t, t.begin(), ::tolower);

    return t;
}

std::string Thx::toUpper(const std::string& text)
{
    auto t = std::string(text);

    std::ranges::transform(t, t.begin(), ::toupper);

    return t;
}

std::string Thx::escapeSingleQuote(std::string const& original)
{
    std::string results(original.size() + std::ranges::count(original, '\''), '\'');

    auto dest = results.begin();

    for(auto current = original.begin(); current != original.end(); ++current)
    {
        if(*current == '\'')
            ++dest;

        *dest = *current;
        ++dest;
    }

    return results;
}

std::string Thx::toDurationString(const std::chrono::time_point<std::chrono::steady_clock>& startTime, const DurationLabelSize shortLabels)
{
    const auto now = std::chrono::steady_clock::now();
    const auto count = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();

    return toDurationString(count, shortLabels);
}

std::string Thx::toDurationString(const long long milliseconds, const DurationLabelSize shortLabels)
{
    const auto days = floor(milliseconds / 1000 / 60 / 60 / 24);
    const auto daysLeft = milliseconds - (days * 1000 * 60 * 60 * 24);
    const auto hours = floor(daysLeft / 1000 / 60 / 60);
    const auto hoursLeft = daysLeft - (hours * 1000 * 60 * 60);
    const auto minutes = floor(hoursLeft / 1000 / 60);
    const auto seconds = (hoursLeft - (minutes * 1000 * 60)) / 1000;

    if(days < 0)
        return "the future";

    auto value = std::string();

    if(floor(days) > 0)
    {
        value += std::format("{} day", floor(days));

        if(floor(days) != 1)
            value += "s";
    }

    if(hours > 0)
    {
        if(!value.empty())
            value += " ";

        value += std::format("{}{}", hours, shortLabels == DurationLong ? " hour" : shortLabels == DurationShort ? " hr" : shortLabels == DurationShorter ? "h" : "");

        if((shortLabels == DurationLong || shortLabels == DurationShort) && hours != 1)
            value += "s";
    }

    if(minutes > 0)
    {
        if(!value.empty())
        {
            if(shortLabels == DurationShortest)
            {
                value += ":";

                if(minutes < 10)
                    value += "0";
            }
            else
                value += " ";
        }

        value += std::format("{}{}", minutes, shortLabels == DurationLong ? " minute" : shortLabels == DurationShort ? " min" : shortLabels == DurationShorter ? "m" : "");

        if((shortLabels == DurationLong || shortLabels == DurationShort) && minutes != 1)
            value += "s";
    }

    const auto secs = static_cast<int>(seconds);

    if(secs > 0)
    {
        if(!value.empty())
        {
            if(shortLabels == DurationShortest)
            {
                value += ":";

                if(seconds < 10)
                    value += "0";
            }
            else
                value += " ";
        }

        value += std::format("{}{}", secs, shortLabels == DurationLong ? " second" : shortLabels == DurationShort ? " sec" : shortLabels == DurationShorter ? "s" : "");

        if((shortLabels == DurationLong || shortLabels == DurationShort) && secs != 1)
            value += "s";
    }

    return value;
}

std::string Thx::trim(const std::string& in)
{
    if(in.empty())
        return in;

    auto b = in.begin(), e = in.end();

    while(std::isspace(*b))
    {
        ++b;
    }

    if(b != e)
    {
        while(std::isspace(*(e - 1)))
        {
            --e;
        }
    }

    return std::string().assign(b, e);
}

std::vector<std::string> Thx::split(const std::string& s, const std::string& delimiter)
{
    std::vector<std::string> ret;
    auto           start = 0;
    int            end;
    do
    {
        end = s.find(delimiter, start);
        const auto len = end - start;
        auto       token = s.substr(start, len);
        ret.emplace_back(trim(token));
        start += len + delimiter.length();
    }
    while(end != std::string::npos);
    return ret;
}

size_t Thx::_countOccurrences(std::string_view s, const std::string_view needle)
{
    auto res = 0;
    auto pos = 0;
    while((pos = s.find(needle, pos)) != std::string_view::npos)
    {
        ++res;
        pos += needle.size();
    }
    return res;
}

std::string Thx::_replaceNotLonger(std::string s, const std::string_view& what, const std::string_view& with)
{
    std::string_view::size_type wpos = 0;
    std::string_view::size_type rpos = 0;
    while(true)
    {
        auto new_rpos = s.find(what, rpos);
        if(new_rpos == std::string::npos)
        {
            new_rpos = s.size();
        }
        const auto n = new_rpos - rpos;
        copy(s.begin() + rpos, s.begin() + new_rpos, s.begin() + wpos);
        wpos += n;
        rpos = new_rpos;
        if(rpos == s.size())
        {
            break;
        }
        std::ranges::copy(with, s.begin() + wpos);
        wpos += with.size();
        rpos += what.size();
    }
    s.resize(wpos);
    return s;
}

std::string Thx::_replaceLonger(std::string s, const std::string_view& what, const std::string_view& with)
{
    const auto occurrences = _countOccurrences(s, what);
    auto       rpos = s.size();
    auto       wpos = rpos + occurrences * (with.size() - what.size());
    s.resize(wpos);

    while(wpos != rpos)
    {
        auto new_rpos = s.rfind(what, rpos - what.size());
        if(new_rpos == std::string::npos)
        {
            new_rpos = 0;
        }
        else
        {
            new_rpos += what.size();
        }
        const auto n = rpos - new_rpos;
        copy_backward(s.begin() + new_rpos, s.begin() + rpos, s.begin() + wpos);
        wpos -= n;
        rpos = new_rpos;
        if(wpos == rpos)
        {
            break;
        }
        std::ranges::copy_backward(with, s.begin() + wpos);
        wpos -= with.size();
        rpos -= what.size();
    }
    return s;
}

bool Thx::_bothAreSpaces(const char lhs, const char rhs)
{
    return (lhs == rhs) && (lhs == ' ');
}

std::string Thx::deleteDuplicateSpaces(std::string& str)
{
    if(str.empty())
        return str;

    const auto new_end = std::unique(str.begin(), str.end(), _bothAreSpaces);
    str.erase(new_end, str.end());

    return str;
}

std::string Thx::replace(std::string s, const std::string_view& what, const std::string_view& with)
{
    if(what.empty())
        return s;

    if(what.size() >= with.size())
    {
        return _replaceNotLonger(std::move(s), what, with);
    }

    return _replaceLonger(std::move(s), what, with);
}

std::string Thx::readTextFile(const std::filesystem::path& filePath)
{
    if(!exists(filePath))
        return "";

    std::ifstream ifs(filePath);
    const auto textFile = std::string(std::istreambuf_iterator<char>{ifs}, {});
    ifs.close();

    return textFile;
}

void Thx::writeTextFile(const std::filesystem::path& filePath, const std::string& data)
{
    std::ofstream ofs(filePath);
    ofs << data;
    ofs.close();
}


#if __has_include("windows.h")
void Thx::copyToClipboard(const std::string& text)
{
    if(OpenClipboard(nullptr))
    {
        EmptyClipboard();
        const HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.length());
        memcpy(GlobalLock(hMem), text.c_str(), text.length());
        GlobalUnlock(hMem);
        SetClipboardData(CF_TEXT, hMem);
        CloseClipboard();
    }
}
#endif

std::string Thx::getDateForFileName(const time_t time)
{
    std::stringstream stream;
    stream << std::put_time(std::localtime(&time), "%F"); // ISO 8601 without timezone information.
    auto timeString = stream.str();
    std::ranges::replace(timeString, ':', '-');
    return timeString;
}

std::string Thx::getDateTimeForFileName(const time_t time)
{
    std::stringstream stream;
    stream << std::put_time(std::localtime(&time), "%F_%T"); // ISO 8601 without timezone information.
    auto timeString = stream.str();
    std::ranges::replace(timeString, ':', '-');
    return timeString;
}

std::string Thx::getUiTime(const time_t time)
{
    std::string s(30, '\0');
    const auto  size = std::strftime(&s[0], s.size(), "%r", std::localtime(&time));
    s.resize(size);
    return s;
}

std::string Thx::getUiTimestamp(const time_t time)
{
    std::string s(50, '\0');
    const auto  size = std::strftime(&s[0], s.size(), "%d %b %y %r", std::localtime(&time));
    s.resize(size);
    return s;
}

std::string Thx::getDbTimestamp(const time_t time)
{
    //return format("{:%FT%TZ}", std::chrono::system_clock::from_time_t(time));

    std::string s(50, '\0');
    const auto  size = std::strftime(&s[0], s.size(), "%FT%TZ", std::gmtime(&time));
    s.resize(size);
    return s;
}
