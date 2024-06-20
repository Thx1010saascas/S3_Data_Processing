#pragma once
#include <PostgresBatchUpdate.h>
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
    const string _upsertString = R"(INSERT INTO gaia (index,source_id,ra,dec,glon,glat,ruwe,parallax,teff_gspphot,teff_source,logg_gspphot,mh_gspphot,ebpminrp_gspphot,phot_g_mean_mag,phot_bp_mean_mag,bp_rp,non_single_star,luminosity,radius) VALUES {} ON CONFLICT DO NOTHING;)";
};
