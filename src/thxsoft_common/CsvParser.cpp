#include "CsvParser.h"
#include <sstream>
#include <fstream>
#include "Thx.h"

namespace thxsoft::common
{
    CsvParser::CsvParser(const std::string& data)
        :   _lineNumber(0),
            _stream(std::make_shared<std::istringstream>(data)),
            _manualColumns(std::make_shared<std::vector<std::string>>()),
            _columnNames(std::make_shared<std::vector<std::string>>()),
            _columnNameIndexes(std::make_shared<std::unordered_map<std::string, int>>()),
            _rawData(std::make_shared<std::vector<std::string>>())
    {
    }

    CsvParser::CsvParser(const std::shared_ptr<std::istream>& stream)
        :   _lineNumber(0),
            _stream(stream),
            _manualColumns(std::make_shared<std::vector<std::string>>()),
            _columnNames(std::make_shared<std::vector<std::string>>()),
            _columnNameIndexes(std::make_shared<std::unordered_map<std::string, int>>()),
            _rawData(std::make_shared<std::vector<std::string>>())
    {
    }

    int CsvParser::lineNumber() const
    {
        return _lineNumber;
    }

    void CsvParser::appendColumn(const std::string& name) const
    {
        _manualColumns->push_back(name);
    }

    std::shared_ptr<std::vector<std::string>> CsvParser::getColumnNames() const
    {
        return _columnNames;
    }

    std::shared_ptr<std::vector<std::string>> CsvParser::rawData() const
    {
        return _rawData;
    }

    void CsvParser::seekStart() const
    {
        _stream->seekg(0, _stream->beg);
    }

    int CsvParser::getOrdinal(const std::string& columnName) const
    {
        if (columnName.starts_with(ColumnReferenceByIndexPrefix))
            return stoi(columnName.substr(ColumnReferenceByIndexPrefix.size(), columnName.size()-ColumnReferenceByIndexPrefix.size()));

        return _columnNameIndexes->at(Thx::toLower(columnName));
    }

    void CsvParser::setValue(const std::string& columnName, const std::string& value) const
    {
        const auto index = _columnNameIndexes->at(Thx::toLower(columnName));

        setValue(index, value);
    }

    void CsvParser::setValue(const int ordinal, const std::string& value) const
    {
        _rawData->at(ordinal) = value;
    }

    std::string CsvParser::getValue(const std::string& columnName) const
    {
        const auto index = _columnNameIndexes->at(Thx::toLower(columnName));
        const auto v = _rawData->at(index);

        return v == "null" ? std::string() : v;
    }

    std::optional<double> CsvParser::getValueAsDouble(const std::string& columnName) const
    {
        const auto value = getValue(columnName);

        if(value.empty())
            return std::nullopt;

        return stod(value);
    }
    
    std::optional<int> CsvParser::getValueAsInteger(const std::string& columnName) const
    {
        const auto value = getValue(columnName);

        if(value.empty())
            return std::nullopt;

        return stoi(value);
    }

    std::optional<long long> CsvParser::getValueAsInt64(const std::string& columnName) const
    {
        const auto value = getValue(columnName);

        if(value.empty())
            return std::nullopt;

        return stoll(value);
    }

    std::string CsvParser::getValue(const int ordinal) const
    {
        return _rawData->at(ordinal);
    }

    std::optional<double> CsvParser::getValueAsDouble(const int ordinal) const
    {
        const auto value = getValue(ordinal);

        if(value.empty())
            return std::nullopt;

        return stod(value);
    }

    std::optional<int> CsvParser::getValueAsInteger(const int ordinal) const
    {
        const auto value = getValue(ordinal);

        if(value.empty())
            return std::nullopt;

        return stoi(value);
    }

    std::optional<long long> CsvParser::getValueAsInt64(const int ordinal) const
    {
        const auto value = getValue(ordinal);

        if(value.empty())
            return std::nullopt;

        return stoll(value);
    }

    bool CsvParser::readLine()
    {
        if (_columnNameIndexes->empty())
            readHeader();

        while (true)
        {
            const auto text = readRawLine();

            if(text == std::nullopt)
                break;

            auto next = Thx::trim(*text);

            _lineNumber++;

            if (next.empty() || next.starts_with("#"))
                continue;

            auto csvValue = std::string();
            auto inQuote = false;
            auto previousIsQuote = false;

            _rawData->clear();

            const auto buildColumnNameList = _columnNames->empty();

            for (auto i = 0; i < next.length(); i++)
            {
                constexpr char CommaCharacter = ',';
                constexpr char QuoteCharacter = '"';

                switch (const auto ch = next[i])
                {
                    case CommaCharacter:
                        previousIsQuote = false;
                        if (inQuote)
                            csvValue += ch;
                        else
                        {
                            auto columnName = Thx::trim(csvValue);

                            if(buildColumnNameList)
                                _columnNames->emplace_back(columnName);

                            _rawData->emplace_back(columnName);
                            csvValue.clear();
                        }

                        break;
                    case QuoteCharacter:
                        // Escaped quote character?
                        if (previousIsQuote)
                        {
                            previousIsQuote = false;
                            inQuote = false;

                            if (i < next.length() - 1 && next[i + 1] != ',')
                                csvValue += QuoteCharacter;
                        }
                        else
                        {
                            previousIsQuote = true;

                            inQuote = !inQuote;
                        }

                        break;
                    default:
                        previousIsQuote = false;
                        csvValue += ch;
                        break;
                }
            }

            const auto columnName = Thx::trim(csvValue);

            if(buildColumnNameList)
                _columnNames->emplace_back(columnName);

            _rawData->emplace_back(columnName);

            for (auto manual = _manualColumns->begin(); manual != _manualColumns->end(); ++manual)
            {
                if(buildColumnNameList)
                    _columnNames->emplace_back(*manual);

                _rawData->emplace_back("");
            }

            return true;
        }

        _rawData->clear();

        return false;
    }

    std::optional<std::string> CsvParser::readRawLine()
    {
        auto line = std::vector<char>();

        char value;
        auto inQuote = false;

        while (_stream->peek() != EOF)
        {
            _stream->read(&value, 1);

            if (inQuote && value == '"')
            {
                inQuote = false;
                line.push_back(value);
                continue;
            }

            switch (value)
            {
            case '"':
                    inQuote = !inQuote;
                    line.push_back(value);
                    break;
                case '\r':
                    continue;
                case '\n':
                    if (!inQuote)
                        return std::string(line.begin(), line.end());
                    line.push_back(value);
                    break;
                default:
                    if (value == 0)
                        throw std::out_of_range("The CSV file is not valid.");

                    line.push_back(value);
                    break;
            }
        }

        // End of stream and nothing read
        if (line.empty())
            return std::nullopt;

        return std::string(line.begin(), line.end());
    }

    void CsvParser::readHeader()
    {
        while (true)
        {
            auto line = readRawLine();

            if(line == std::nullopt)
                break;

            if (line->empty() || line->starts_with('#'))
                continue;

            auto index = 0;

            for (const auto& column : Thx::split(*line, ","))
                _columnNameIndexes->insert({Thx::trim(Thx::toLower(column)), index++});

            for (auto manual = _manualColumns->begin(); manual != _manualColumns->end(); ++manual)
                _columnNameIndexes->insert({Thx::trim(Thx::toLower(*manual)), index++});

            return;
        }

        _columnNameIndexes->clear();
    }
}
