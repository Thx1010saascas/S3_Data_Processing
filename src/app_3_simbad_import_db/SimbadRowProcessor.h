#pragma once
#include <string>
#include "CsvParser.h"

using namespace thxsoft::common;

class SimbadRowProcessor {
public:
    static bool processRow(double minParallax, const CsvParser& csvParser);

    static constexpr uint32_t hash(const char *s, int off = 0) noexcept;

    inline static const std::string IndexColumnName = "index";
    inline static const std::string MainIdColumnName = "main_id";
    inline static const std::string TypeColumnName = "type";
    inline static const std::string NameColumnName = "name";
    inline static const std::string NameWolfColumnName = "name_wold";
    inline static const std::string NameRossColumnName = "name_ross";
    inline static const std::string NameSColumnName = "name_s";
    inline static const std::string NameSSColumnName = "name_ss";
    inline static const std::string NameVSColumnName = "name_vs";
    inline static const std::string NameHipColumnName = "name_hip";
    inline static const std::string NameHdColumnName = "name_hd";
    inline static const std::string NameGjColumnName = "name_gj";
    inline static const std::string NameWiseColumnName = "name_wise";
    inline static const std::string Name2MassColumnName = "name_2mass";
    inline static const std::string NameGaiaColumnName = "name_gaia";
    inline static const std::string NameTycColumnName = "name_tyc";
    inline static const std::string NameNgcColumnName = "name_ngc";
    inline static const std::string SourceIdColumnName = "g_source_id";
    inline static const std::string RaColumnName = "ra";
    inline static const std::string DecColumnName = "dec";
    inline static const std::string GLatColumnName = "glat";
    inline static const std::string GLonColumnName = "glon";
    inline static const std::string ParallaxColumnName = "parallax";
    inline static const std::string TeffColumnName = "teff";
    inline static const std::string RadiusColumnName = "radius";
    inline static const std::string LuminosityColumnName = "luminosity";
    inline static const std::string FehColumnName = "fe_h";
    inline static const std::string BMagColumnName = "b";
    inline static const std::string VMagColumnName = "v";
    inline static const std::string GMagColumnName = "g_mag";
    inline static const std::string SpectralTypeColumnName = "spectral_type";
    inline static const std::string ObjectTypeColumnName = "object_type";

private:
    static bool setTeffFromCalculation(const CsvParser& csvParser);
    static bool hasUpperAndLower(const std::string& string);
    static std::string getIfInCatalog(const std::vector<std::string>& cats, const std::string& catalogue);
    static void populateNames(const CsvParser& csvParser);
    static void populateObjectType(const CsvParser& csvParser);
    static void populateLuminosityAndRadius(const CsvParser& csvParser, const std::optional<int>& teff);
    static void populateGalacticCoordinates(const CsvParser& csvParser);

    static std::string getCleanName(const std::string& value);
};

