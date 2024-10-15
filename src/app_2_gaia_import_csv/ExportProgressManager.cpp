#include "ExportProgressManager.h"

#include <fstream>
#include <spdlog/spdlog.h>

ExportProgressManager::ExportProgressManager(const std::string& stateFilePath)
    : _stateFilePath(stateFilePath),
      _sempaphore(std::counting_semaphore<1>(1))

{
    auto file = std::ifstream(stateFilePath, std::ios::in);

    if (!file.is_open()) // It may not exist yet.
        return;

    std::string label;
    while(getline(file, label))
    {
        _readFiles.insert(label);
    }

    file.close();
}

void ExportProgressManager::writeState()
{
    _sempaphore.acquire();
    auto file = std::ofstream(_stateFilePath, std::ios::out);

    for(const auto& label : _readFiles)
    {
        file << label << std::endl;
    }

    file.close();

    _sempaphore.release();
}

void ExportProgressManager::add(const std::string& label)
{
    _readFiles.insert(label);
}

bool ExportProgressManager::isComplete(const std::string& label) const
{
    return _readFiles.contains(label);
}
