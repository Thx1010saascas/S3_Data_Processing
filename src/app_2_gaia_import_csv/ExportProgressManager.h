#pragma once
#include <string>
#include <semaphore>
#include <unordered_set>

using namespace std;

struct ExportProgressManager {
    explicit ExportProgressManager(const string& stateFilePath);

    void writeState();

    [[nodiscard]]bool isComplete(const string& label) const;

    void add(const string& label);

private:
    const string _stateFilePath;
    counting_semaphore<1> _sempaphore;
    unordered_set<string> _readFiles;
};
