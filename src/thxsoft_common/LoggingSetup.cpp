#include "LoggingSetup.h"
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <filesystem>

void LoggingSetup::setupDefaultLogging(const std::string& logPath, const spdlog::level::level_enum logLevel, const int flushIntervalSeconds, const int maxFileSize, const int maxFiles)
{
    spdlog::init_thread_pool(1024 * 8, 1);
    const auto consoleSink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_st>();
    std::shared_ptr<spdlog::async_logger> logger;

    if(!logPath.empty())
    {
        const auto fullPath = std::filesystem::path(logPath);

        if (fullPath.has_parent_path())
        {
            std::error_code ec;
            const auto path = fullPath.parent_path();

            if(!exists(path, ec))
                create_directory(path);
        }

        const auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_st>(logPath, maxFileSize, maxFiles);
        const auto sinks = spdlog::sinks_init_list{consoleSink, fileSink};
        logger = std::make_shared<spdlog::async_logger>("main", sinks, spdlog::thread_pool());

        if(flushIntervalSeconds > 0)
            spdlog::flush_every(std::chrono::seconds(flushIntervalSeconds));
    }
    else
    {
        logger = std::make_shared<spdlog::async_logger>("main", consoleSink, spdlog::thread_pool());
    }

    set_default_logger(logger);

    logger->set_pattern("%H:%M:%S %^%L%$ %s:%# [%O] %v");
    logger->set_level(logLevel);
}
