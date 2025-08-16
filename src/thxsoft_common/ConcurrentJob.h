#pragma once
#include <future>
#include <spdlog/spdlog.h>

struct ConcurrentJob
{
    template<typename T>
    static void waitForThreadsToFinish(std::vector<std::future<T>>* tasks)
    {
        waitForAvailableThread(tasks, 0);
    }

    template<typename T>
    static void waitForAvailableThread(std::vector<std::future<T>>* tasks, const int maxThreadCount)
    {
        // ReSharper disable once CppDFALoopConditionNotUpdated
        while(tasks->size() >= maxThreadCount)
        {
            auto task = tasks->begin();
            while (task != tasks->end())
            {
                if(task->wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout)
                    ++task;
                else
                    task = tasks->erase(task);
            }

            if(maxThreadCount == 0)
            {
                if(tasks->empty())
                    break;

                spdlog::info("Waiting for {} tasks to finish.", tasks->size());
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
};
