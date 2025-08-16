#pragma once

#include <spdlog/spdlog.h>

struct LoggingSetup {
    static void setupDefaultLogging(const std::string& logPath = "logs/log.txt", spdlog::level::level_enum logLevel = spdlog::level::debug, int flushIntervalSeconds = 10, int maxFileSize = 1024 * 1024, int maxFiles = 3);
};
