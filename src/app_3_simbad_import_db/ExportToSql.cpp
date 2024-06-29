#include "ExportToSql.h"
#include <spdlog/spdlog.h>

#include "CsvParser.h"
#include "DatabaseUtils.h"
#include "SimbadRowProcessor.h"

using namespace pqxx;
using namespace thxsoft::database;

ExportToSql::ExportToSql(const string& connectionString)
{
    _batchAdder = make_shared<PostgresBatchUpdate>(connectionString, _upsertString);
}

void ExportToSql::commit() const
{
    _batchAdder->commit();
}

void ExportToSql::append(const CsvParser& csvParser) const
{
    _batchAdder->addRow(
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::IndexColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::TypeColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::NameColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::NameWolfColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::NameRossColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::NameSColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::NameSSColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::NameVSColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::NameHipColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::NameHdColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::NameGjColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::NameWiseColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::Name2MassColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::NameGaiaColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::NameTycColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::NameNgcColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::SourceIdColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::RaColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::DecColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::GLatColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::GLonColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::ParallaxColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::TeffColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::RadiusColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::LuminosityColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::FehColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::BMagColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::VMagColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::GMagColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::SpectralTypeColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(SimbadRowProcessor::ObjectTypeColumnName))
    );
}
