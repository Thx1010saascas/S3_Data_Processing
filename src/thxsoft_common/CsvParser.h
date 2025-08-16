#pragma once
#include <istream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>

namespace thxsoft::common
{
    class CsvParser {
    public:
        explicit CsvParser(const std::string& data);
        explicit CsvParser(const std::shared_ptr<std::istream>& stream);

        [[nodiscard]]int lineNumber() const;
        void seekStart() const;
        void appendColumn(const std::string& name) const;
        [[nodiscard]]int getOrdinal(const std::string& columnName) const;
        [[nodiscard]]std::shared_ptr<std::vector<std::string>> getColumnNames() const;
        [[nodiscard]]std::shared_ptr<std::vector<std::string>> rawData() const;
        void setValue(const std::string& columnName, const std::string& value) const;
        void setValue(int ordinal, const std::string& value) const;

        [[nodiscard]]std::string getValue(const std::string& columnName) const;
        [[nodiscard]]std::optional<double> getValueAsDouble(const std::string& columnName) const;
        [[nodiscard]]std::optional<int> getValueAsInteger(const std::string& columnName) const;
        [[nodiscard]]std::optional<long long> getValueAsInt64(const std::string& columnName) const;
        [[nodiscard]]std::string getValue(int ordinal) const;
        [[nodiscard]]std::optional<double> getValueAsDouble(int ordinal) const;
        [[nodiscard]]std::optional<int> getValueAsInteger(int ordinal) const;
        [[nodiscard]]std::optional<long long> getValueAsInt64(int ordinal) const;
        bool readLine();

    private:
        void readHeader();
        std::optional<std::string> readRawLine() ;

        int _lineNumber;
        /// <summary>
        /// Defines a prefix to be used in Source column names to represent an index rather than a column name reference.
        /// </summary>
        const std::string ColumnReferenceByIndexPrefix = "###";

        const std::shared_ptr<std::istream> _stream;
        const std::shared_ptr<std::vector<std::string>> _manualColumns;
        const std::shared_ptr<std::vector<std::string>> _columnNames;
        const std::shared_ptr<std::unordered_map<std::string, int>> _columnNameIndexes;
        const std::shared_ptr<std::vector<std::string>> _rawData;
    };
}