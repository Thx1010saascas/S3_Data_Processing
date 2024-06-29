#pragma once
#include <string>
#include "CsvParser.h"

using namespace std;
using namespace thxsoft::common;

class SimbadRowProcessor {
public:
    static bool processRow(double minParallax, const CsvParser& csvParser);

    static constexpr uint32_t hash(const char *s, int off = 0) noexcept;

    inline static const string IndexColumnName = "index";
    inline static const string MainIdColumnName = "main_id";
    inline static const string TypeColumnName = "type";
    inline static const string NameColumnName = "name";
    inline static const string NameWolfColumnName = "name_wold";
    inline static const string NameRossColumnName = "name_ross";
    inline static const string NameSColumnName = "name_s";
    inline static const string NameSSColumnName = "name_ss";
    inline static const string NameVSColumnName = "name_vs";
    inline static const string NameHipColumnName = "name_hip";
    inline static const string NameHdColumnName = "name_hd";
    inline static const string NameGjColumnName = "name_gj";
    inline static const string NameWiseColumnName = "name_wise";
    inline static const string Name2MassColumnName = "name_2mass";
    inline static const string NameGaiaColumnName = "name_gaia";
    inline static const string NameTycColumnName = "name_tyc";
    inline static const string NameNgcColumnName = "name_ngc";
    inline static const string SourceIdColumnName = "g_source_id";
    inline static const string RaColumnName = "ra";
    inline static const string DecColumnName = "dec";
    inline static const string GLatColumnName = "glat";
    inline static const string GLonColumnName = "glon";
    inline static const string ParallaxColumnName = "parallax";
    inline static const string TeffColumnName = "teff";
    inline static const string RadiusColumnName = "radius";
    inline static const string LuminosityColumnName = "luminosity";
    inline static const string FehColumnName = "fe_h";
    inline static const string BMagColumnName = "b";
    inline static const string VMagColumnName = "v";
    inline static const string GMagColumnName = "g_mag";
    inline static const string SpectralTypeColumnName = "spectral_type";
    inline static const string ObjectTypeColumnName = "object_type";

private:
    static bool setTeffFromCalculation(const CsvParser& csvParser);
    static bool hasUpperAndLower(const string& string);
    static string getIfInCatalog(const vector<string>& cats, const string& catalogue);
    static void populateNames(const CsvParser& csvParser);
    static void populateObjectType(const CsvParser& csvParser);
    static void populateLuminosityAndRadius(const CsvParser& csvParser, const optional<int>& teff);
    static void populateGalacticCoordinates(const CsvParser& csvParser);

    static string getCleanName(const string& value);
};

