#include "SimbadRowProcessor.h"
#include <format>
#include <spdlog/spdlog.h>
#include "AstronomyConverter.h"
#include "LuminanceCalculator.h"
#include "RadiusCalculator.h"
#include "TeffCalculator.h"
#include "GalacticCoordinate.h"
#include "SimbadObjectType.h"
#include "Thx.h"

using namespace thxsoft::astronomy;

bool SimbadRowProcessor::processRow(const double minParallax, const CsvParser& csvParser)
{
    if(const double parallax = csvParser.getValueAsDouble(ParallaxColumnName).value();
        parallax < minParallax)
        return false;

    if (csvParser.getValue(TeffColumnName).empty())
    {
        if(!setTeffFromCalculation(csvParser))
            return false;
    }

    // These have default values, make sure they are cleared.
    csvParser.setValue(ObjectTypeColumnName, "");
    csvParser.setValue(LuminosityColumnName, "");
    csvParser.setValue(RadiusColumnName, "");
    csvParser.setValue(SourceIdColumnName, "");

    csvParser.setValue(TypeColumnName, csvParser.getValue(TypeColumnName));
    csvParser.setValue(SpectralTypeColumnName, csvParser.getValue(SpectralTypeColumnName));

    populateNames(csvParser);
    populateObjectType(csvParser);
    populateGalacticCoordinates(csvParser);

    // Teff is now an integer.
    if(const auto teff = csvParser.getValueAsDouble(TeffColumnName);
        teff.has_value())
    {
        const auto intTeff = static_cast<int>(teff.value());
        csvParser.setValue(TeffColumnName, to_string(intTeff));
        populateLuminosityAndRadius(csvParser, intTeff);
    }
    return true;
}

bool SimbadRowProcessor::hasUpperAndLower(const string& string)
{
    auto hasUpper = false;
    auto hasLower = false;
    for(auto ch : string)
    {
        if (islower(ch))
            hasLower = true;
        else if (isupper(ch))
            hasUpper = true;

        if(hasLower && hasUpper)
            return true;
    }
    return false;
}

string SimbadRowProcessor::getIfInCatalog(const vector<string>& cats, const string& catalogue)
{
    for(const auto& cat : cats)
    {
        if(cat.starts_with(catalogue))
        {
            const auto name = getCleanName(cat);

            return name;
        }
    }

    return "";
}

void SimbadRowProcessor::populateNames(const CsvParser& csvParser)
{
    const auto cats = Thx::split(csvParser.getValue(Name1ColumnName), "|");
    string name1 = "";

    if(const auto mainId = csvParser.getValue(MainIdColumnName); mainId.starts_with("NAME "))
    {
        const auto cleanName = getCleanName(mainId);
        if(!cleanName.starts_with('['))
            name1 = cleanName;
    }

    for(const auto& name : cats)
    {
        if(name.starts_with("NAME "))
        {
            const auto cleanName = getCleanName(name);

            if(!cleanName.starts_with('['))
            {
                name1 = cleanName;
                break;
            }
        }
        else if(name.starts_with("Gaia DR3 "))
            csvParser.setValue(SourceIdColumnName, Thx::trim(name.substr(strlen("Gaia DR3 "))));
    }

    const auto name2 = getIfInCatalog(cats, "Wolf ");
    const auto name3 = getIfInCatalog(cats, "Ross ");
    const auto name4 = getIfInCatalog(cats, "HD ");
    const auto name5 = getIfInCatalog(cats, "GJ ");
    const auto name6 = getIfInCatalog(cats, "* ");
    const auto name7 = getIfInCatalog(cats, "** ");
    const auto name8 = getIfInCatalog(cats, "V* ");
    const auto name9 = getIfInCatalog(cats, "Gaia DR3 ");
    const auto name10 = getIfInCatalog(cats, "2MASS ");

    csvParser.setValue(Name1ColumnName, name1);
    csvParser.setValue(Name2ColumnName, name2);
    csvParser.setValue(Name3ColumnName, name3);
    csvParser.setValue(Name4ColumnName, name4);
    csvParser.setValue(Name5ColumnName, name5);
    csvParser.setValue(Name6ColumnName, name6);
    csvParser.setValue(Name7ColumnName, name7);
    csvParser.setValue(Name8ColumnName, name8);
    csvParser.setValue(Name9ColumnName, name9);
    csvParser.setValue(Name10ColumnName, name10);
}

constexpr uint32_t SimbadRowProcessor::hash(const char *s, const int off) noexcept
{
    return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];
}

void SimbadRowProcessor::populateObjectType(const CsvParser& csvParser)
{
    auto objectType = static_cast<int>(Unknown);

    for (const auto& type : Thx::split(csvParser.getValue("type"), "|"))
    {
        switch (hash(type.c_str()))
        {
        case hash("N*"):
        case hash("Psr"): objectType |= NeutronStar | Star; break;
        case hash("OpC"): objectType |= OpenCluster; break;
        case hash("GlC"): objectType |= GlobularCluster; break;
        case hash("C?*"):
        case hash("Cl*"): objectType |= Cluster; break;
        case hash("Pl?"):
        case hash("Pl"): objectType |= Planet; break;
        case hash("BD*"): objectType |= BrownDwarf | Star; break;
        case hash("LM*"): objectType |= LowMassStar | Star; break;
        case hash("As*"):
        case hash("St*"):
        case hash("MGr"): objectType |= StarGroup; break;
        case hash("ULX"):
        case hash("X"): objectType |= XRay; break;
        case hash("WD"): objectType |= WhiteDwarf | Star; break;
        case hash("PN"): objectType |= Nebula; break;
        case hash("SN*"): objectType |= SuperNovae; break;
        case hash("Pog"): objectType |= PartOfGalaxy; break;
        case hash("G"): objectType |= Galaxy; break;
        case hash("AG?"):
        case hash("AGN"): objectType |= ActiveGalaxyNucleus; break;
        default:
            if(type.find('*') != string::npos)
                objectType |= Star;
            break;
        }
    }

    csvParser.setValue(ObjectTypeColumnName, to_string(objectType));
}

void SimbadRowProcessor::populateLuminosityAndRadius(const CsvParser& csvParser, const optional<int>& teff)
{
    if (LuminanceCalculator::teffIsInGBandCalculationRange(teff.value()))
    {
        const auto mag = csvParser.getValueAsDouble(VMagColumnName);
        if (mag.has_value())
        {
            const auto parsecs = AstronomyConverter::toParsecsMas(csvParser.getValueAsDouble(ParallaxColumnName).value());

            if (const auto luminosityLSol = LuminanceCalculator::calculateGBand(mag.value(), parsecs, teff.value());
                isfinite(luminosityLSol))
            {
                if (const auto radius = RadiusCalculator::calculateWithLSol(luminosityLSol, teff.value());
                    isfinite(radius))
                {
                    csvParser.setValue("luminosity", format("{:.3f}", luminosityLSol));
                    csvParser.setValue("radius", format("{:.3f}", AstronomyConverter::toRSol(radius)));
                }
            }
        }
    }
}

bool SimbadRowProcessor::setTeffFromCalculation(const CsvParser& csvParser)
{
    const auto bMag = csvParser.getValueAsDouble(BMagColumnName);
    const auto vMag = csvParser.getValueAsDouble(VMagColumnName);
    const auto teff = TeffCalculator::calculate(bMag, vMag);

    if (!teff.has_value())
    {
        // All stars require a temperature.
        if(const auto objectType = csvParser.getValueAsInteger(ObjectTypeColumnName).value();
            (objectType & static_cast<int>(Star)) > 0)
            return false;
    }

    csvParser.setValue("teff",  teff.has_value() ? to_string(teff.value()) : "");

    return true;
}


void SimbadRowProcessor::populateGalacticCoordinates(const CsvParser& csvParser)
{
    const auto ra = csvParser.getValueAsDouble(RaColumnName).value();
    const auto dec = csvParser.getValueAsDouble(DecColumnName).value();

    const auto galacticCoordinates = GalacticCoordinate::equatorialToGalactic(ra, dec);

    csvParser.setValue(GLatColumnName, to_string(galacticCoordinates->latitude));
    csvParser.setValue(GLonColumnName, to_string(galacticCoordinates->longitude));
}

string SimbadRowProcessor::getCleanName(const string& value)
{
    if (value.empty())
        return value;

    string name;
    if (value.starts_with("NAME "))
    {
        name = value.substr(strlen("NAME "));
    }
    else
        name = value;

    name = Thx::trim(Thx::deleteDuplicateSpaces(name));

    return name;
}
