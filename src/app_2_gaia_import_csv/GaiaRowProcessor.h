#pragma once
#include <string>
#include "CsvParser.h"

using namespace std;
using namespace thxsoft::common;

class GaiaRowProcessor {
public:
    static bool processRow(double minparallax, const CsvParser& csvParser);

    inline static const string IndexColumnName = "random_index";
    inline static const string SourceIdColumnName = "source_id";
    inline static const string RaColumnName = "ra";
    inline static const string DecColumnName = "dec";
    inline static const string GLatColumnName = "b";
    inline static const string GLonColumnName = "l";
    inline static const string ParallaxColumnName = "parallax";
    inline static const string RuweColumnName = "ruwe";
    inline static const string TeffColumnName = "teff_gspphot";
    inline static const string TeffSourceColumnName = "teff_source";
    inline static const string GMeanMagColumnName = "phot_g_mean_mag";
    inline static const string BpMeanMagColumnName = "phot_bp_mean_mag";
    inline static const string RpMeanMagColumnName = "phot_rp_mean_mag";
    inline static const string BpRpColumnName = "bp_rp";
    inline static const string LoggColumnName = "logg_gspphot";
    inline static const string MhColumnName = "mh_gspphot";
    inline static const string EbpminrpColumnName = "ebpminrp_gspphot";
    inline static const string LuminosityColumnName = "luminosity";
    inline static const string RadiusColumnName = "radius";
    inline static const string NonSingleStarColumnName = "non_single_star";

private:

    static bool setTeffFromCalculation(const CsvParser& csvParser);
    static void populateLuminosityAndRadius(const CsvParser& csvParser);
};

