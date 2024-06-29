#pragma once
#include <queue>
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
        static void pushNameString(queue<string>& queue, const char* name);
        static string popNameString(queue<string>& queue);
        static double myRound(double x);

    private:
        static double GetSectorNumber(double v);
        [[nodiscard]] long long getMaxIdAsync(const string& tableName) const;

        const pqxx::zview _nonGaiaObjectsQuery =
            "SELECT simbad.index,"
            "COALESCE(new_name, name),name_wolf,name_ross,COALESCE(new_name_s, name_s),COALESCE(new_name_ss, name_ss),COALESCE(new_name_vs,name_vs),"
            "name_hip, name_hd, name_gj,name_wise,name_2mass,name_gaia,name_tyc,name_ngc,parallax,teff,"
            "new_spectral_type, fe_h, luminosity, radius,"
            "COALESCE(v, g_mag), x, y, z, g_source_id, CASE WHEN type like '%**%' THEN 1 ELSE 0 END, object_type "
            "FROM simbad LEFT OUTER JOIN export_overrides nf ON nf.index = simbad.index "
            "WHERE id >= {} AND id < {} AND distance_ly <= {} AND object_type & {} > 0 AND "
            "(g_source_id IS NULL OR (g_source_id IS NOT NULL AND (SELECT COUNT(*) FROM gaia WHERE source_id = g_source_id) = 0))";
        const pqxx::zview _gaiaObjectsQuery =
            "SELECT g.index,"
            "COALESCE(new_name, name),name_wolf,name_ross,COALESCE(new_name_s, name_s),COALESCE(new_name_ss, name_ss),COALESCE(new_name_vs,name_vs),"
            "name_hip, name_hd, name_gj,name_wise,name_2mass,COALESCE(name_gaia, 'Gaia DR3 ' || source_id),name_tyc,name_ngc,"
            "g.parallax, teff_gspphot,new_spectral_type, COALESCE(mh_gspphot, fe_h), g.luminosity, g.radius,"
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
