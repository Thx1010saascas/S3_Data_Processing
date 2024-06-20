#pragma once
#include <pqxx/pqxx>
#include <SqliteBatchUpdate.h>
#include "CelestialObject.h"
#include "SpectralClassifier.h"

using namespace std;

namespace thxsoft::export_s3_sectors
{
    class DbReader {
    public:
        explicit DbReader(const string&, int searchRangeLimitLy, int objectBitMask);

        void getStars(bool getNonGaiaStars, const function<void(const CelestialObject *)>&) const;

    private:
        static double GetSectorNumber(double v);
        [[nodiscard]] long long getMaxIdAsync(const string& tableName) const;

        const pqxx::zview _nonGaiaObjectsQuery =
            "SELECT simbad.index, "
            "COALESCE(new_name1, name1), COALESCE(new_name2, name2), COALESCE(new_name3, name3), name4, parallax, teff, "
            "new_spectral_type, fe_h, luminosity, radius, "
            "COALESCE(v, g_mag), x, y, z, g_source_id, CASE WHEN type like '%**%' THEN 1 ELSE 0 END, object_type "
            "FROM simbad LEFT OUTER JOIN export_overrides nf ON nf.index = simbad.index "
            "WHERE id >= {} AND id < {} AND distance_ly <= {} AND g_source_id IS NULL AND object_type & {} > 0";
        const pqxx::zview _gaiaObjectsQuery =
            "SELECT g.index, "
            "COALESCE(new_name1, name1), COALESCE(new_name2, name2), COALESCE(new_name3, name3), name4, g.parallax, teff_gspphot, "
            "new_spectral_type, COALESCE(mh_gspphot, fe_h), g.luminosity, g.radius, "
            "g.phot_bp_mean_mag, g.x, g.y, g.z, g.source_id, non_single_star >= 1, COALESCE(object_type, 1) "
            "FROM gaia AS g "
            "LEFT OUTER JOIN simbad AS s ON g.source_id = s.g_source_id "
            "LEFT OUTER JOIN export_overrides nf ON nf.index = s.index "
            "WHERE g.id >= {} AND g.id < {} AND g.distance_ly <= {} AND COALESCE(object_type, 1)  & {} > 0";

        const int QueryPageSize = 250000;

        shared_ptr<astronomy::SpectralClassifier> spectralClassifier;
        shared_ptr<pqxx::connection> _dbReadConnection;
        const int _searchRangeLimitLy;
        const int _objectBitMask;
        chrono::time_point<chrono::steady_clock> _start;
    };
}
