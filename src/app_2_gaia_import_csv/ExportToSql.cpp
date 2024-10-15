#include "ExportToSql.h"
#include <spdlog/spdlog.h>

#include "CsvParser.h"
#include "DatabaseUtils.h"
#include "GaiaRowProcessor.h"

using namespace pqxx;
using namespace thxsoft::database;

ExportToSql::ExportToSql(const std::string& connectionString)
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
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::IndexColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::SourceIdColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::RaColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::DecColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::GLonColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::GLatColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::RuweColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::ParallaxColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::TeffColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::TeffSourceColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::LoggColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::MhColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::EbpminrpColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::GMeanMagColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::BpMeanMagColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::BpRpColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::NonSingleStarColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::LuminosityColumnName)),
        DatabaseUtils::makeDbColumnValue(csvParser.getValue(GaiaRowProcessor::RadiusColumnName))
    );
}