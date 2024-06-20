#include "ExportProgressManager.h"

#include <fstream>
#include <spdlog/spdlog.h>

ExportProgressManager::ExportProgressManager(const string& stateFilePath)
    : _stateFilePath(stateFilePath),
      _sempaphore(counting_semaphore<1>(1))

{
    auto file = ifstream(stateFilePath, ios::in);

    if (!file.is_open()) // It may not exist yet.
        return;

    string label;
    while(getline(file, label))
    {
        _readFiles.insert(label);
    }

    file.close();
}

void ExportProgressManager::writeState()
{
    _sempaphore.acquire();
    auto file = ofstream(_stateFilePath, ios::out);

    for(const auto& label : _readFiles)
    {
        file << label << endl;
    }

    file.close();

    _sempaphore.release();
}

void ExportProgressManager::add(const string& label)
{
    _readFiles.insert(label);
}

bool ExportProgressManager::isComplete(const string& label) const
{
    return _readFiles.contains(label);
}
