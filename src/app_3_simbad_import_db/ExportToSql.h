#pragma once
#include "PostgresBatchUpdate.h"
#include "CsvParser.h"

using namespace thxsoft::common;
using namespace thxsoft::database::postgres;

class ExportToSql {
public:
    explicit ExportToSql(const std::string& connectionString);

    void append(const CsvParser& csvParser) const;
    void commit() const;

private:
    std::shared_ptr<PostgresBatchUpdate> _batchAdder;
    std::shared_ptr<pqxx::connection> _dbWriteConnection;

    // Must be the same order as ExportToSql::append
    const std::string _upsertString = R"(INSERT INTO simbad (index,type,name,name_wolf,name_ross,name_s,name_ss,name_vs,name_hip,name_hd,name_gj,name_wise,name_2mass,name_gaia,name_tyc,name_ngc,g_source_id,ra,dec,glat,glon,parallax,teff,radius,luminosity,fe_h,b,v,g_mag,spectral_type,object_type) VALUES {} ON CONFLICT DO NOTHING;)";
};
