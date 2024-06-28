#pragma once
#include "PostgresBatchUpdate.h"
#include "CsvParser.h"

using namespace std;
using namespace thxsoft::common;
using namespace thxsoft::database::postgres;

class ExportToSql {
public:
    explicit ExportToSql(const string& connectionString);

    void append(const CsvParser& csvParser) const;
    void commit() const;

private:
    shared_ptr<PostgresBatchUpdate> _batchAdder;
    shared_ptr<pqxx::connection> _dbWriteConnection;

    // Must be the same order as ExportToSql::append
    const string _upsertString = R"(INSERT INTO simbad (index,type,name1,name2,name3,name4,name5,name6,name7,name8,name9,name10,g_source_id,ra,dec,glat,glon,parallax,teff,radius,luminosity,fe_h,b,v,g_mag,spectral_type,object_type) VALUES {} ON CONFLICT DO NOTHING;)";
};
