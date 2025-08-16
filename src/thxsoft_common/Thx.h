#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <filesystem>
#include <unordered_set>
#include "LoggingSetup.h"

enum DurationLabelSize
{
    DurationShortest,
    DurationShorter,
    DurationShort,
    DurationLong
};

class Thx
{
public:
    static std::string getDuration(const std::shared_ptr<time_t>& startTime, const std::shared_ptr<time_t>& endTime, DurationLabelSize durationLabelSize = DurationShortest);
    static std::string getDuration(time_t startTime, time_t endTime, DurationLabelSize durationLabelSize = DurationShortest);
    static long long stoll(const std::string& value, long long defaultValue = 0);
    static int stoi(const std::string& value, int defaultValue = 0);
    static double stod(const std::string& value, double defaultValue = 0);
    static std::optional<long long> toInt64(const std::string& value);
    static std::optional<int> toInt(const std::string& value);
    static std::optional<double> toDouble(const std::string& value);
    static int indexOfAny(const std::string& str, const std::unordered_set<char>& chars);
    static time_t nowUtc();
    static time_t nowUtcMs();
    static time_t timeToFileTimeMultiplier(time_t time);
    static void setCustomLocale();
    static std::string toLower(const std::string& text);
    static std::string toUpper(const std::string& text);
    static std::string toDurationString(const std::chrono::time_point<std::chrono::steady_clock>& startTime, DurationLabelSize shortLabels = DurationLong);
    static std::string toDurationString(long long milliseconds, DurationLabelSize shortLabels = DurationLong);
    static std::string escapeSingleQuote(const std::string& original);
    static std::vector<std::string> split(const std::string& s, const std::string& delimiter = " ");
    static std::string trim(const std::string& in);
    static std::string replace(std::string s, const std::string_view& what, const std::string_view& with);
    static std::string deleteDuplicateSpaces(std::string& str);
    static std::string readTextFile(const std::filesystem::path& filePath);
    static void writeTextFile(const std::filesystem::path& filePath, const std::string& data);
    static std::string getUiTime(time_t time);
    static std::string getUiTimestamp(time_t time);
    static std::string getDbTimestamp(time_t time);
    static std::string getDateForFileName(time_t time);
    static std::string getDateTimeForFileName(time_t time);

#if __has_include("windows.h")
    static void copyToClipboard(const std::string& text);
#endif
private:
    static bool _bothAreSpaces(char lhs, char rhs);
    static std::string _replaceLonger(std::string s, const std::string_view& what, const std::string_view& with);
    static std::string _replaceNotLonger(std::string s, const std::string_view& what, const std::string_view& with);
    static size_t _countOccurrences(std::string_view s, const std::string_view needle);
};

struct LocaleNumSeparated final : std::numpunct<char>
{
    [[nodiscard]] char do_thousands_sep() const override { return ','; }
    [[nodiscard]] std::string do_grouping() const override { return "\03"; }
};
