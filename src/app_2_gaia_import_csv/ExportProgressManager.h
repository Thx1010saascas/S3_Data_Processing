#pragma once
#include <string>
#include <semaphore>
#include <unordered_set>


struct ExportProgressManager {
    explicit ExportProgressManager(const std::string& stateFilePath);

    void writeState();

    [[nodiscard]]bool isComplete(const std::string& label) const;

    void add(const std::string& label);

private:
    const std::string _stateFilePath;
    std::counting_semaphore<1> _sempaphore;
    std::unordered_set<std::string> _readFiles;
};
