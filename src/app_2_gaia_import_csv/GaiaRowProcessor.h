#pragma once
#include <string>
#include "CsvParser.h"

using namespace thxsoft::common;

class GaiaRowProcessor {
public:
    static bool processRow(double minparallax, const CsvParser& csvParser);

    inline static const std::string IndexColumnName = "random_index";
    inline static const std::string SourceIdColumnName = "source_id";
    inline static const std::string RaColumnName = "ra";
    inline static const std::string DecColumnName = "dec";
    inline static const std::string GLatColumnName = "b";
    inline static const std::string GLonColumnName = "l";
    inline static const std::string ParallaxColumnName = "parallax";
    inline static const std::string RuweColumnName = "ruwe";
    inline static const std::string TeffColumnName = "teff_gspphot";
    inline static const std::string TeffSourceColumnName = "teff_source";
    inline static const std::string GMeanMagColumnName = "phot_g_mean_mag";
    inline static const std::string BpMeanMagColumnName = "phot_bp_mean_mag";
    inline static const std::string RpMeanMagColumnName = "phot_rp_mean_mag";
    inline static const std::string BpRpColumnName = "bp_rp";
    inline static const std::string LoggColumnName = "logg_gspphot";
    inline static const std::string MhColumnName = "mh_gspphot";
    inline static const std::string EbpminrpColumnName = "ebpminrp_gspphot";
    inline static const std::string LuminosityColumnName = "luminosity";
    inline static const std::string RadiusColumnName = "radius";
    inline static const std::string NonSingleStarColumnName = "non_single_star";

private:

    static bool setTeffFromCalculation(const CsvParser& csvParser);
    static void populateLuminosityAndRadius(const CsvParser& csvParser);
};

