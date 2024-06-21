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

bool SimbadRowProcessor::processRow(const double minparallax, const CsvParser& csvParser)
{
    const auto parallax = csvParser.getValueAsDouble(ParallaxColumnName).value();

    if(parallax < minparallax)
        return false;

    if (csvParser.getValue(TeffColumnName).empty())
    {
        if(!setTeffFromCalculation(csvParser))
            return false; // Can't calculate a temperature so exit.
    }

    // Teff is now an integer.
    csvParser.setValue(TeffColumnName, to_string(static_cast<int>(csvParser.getValueAsDouble(TeffColumnName).value())));

    // These have default values, make sure they are cleared.
    csvParser.setValue(ObjectTypeColumnName, "");
    csvParser.setValue(LuminosityColumnName, "");
    csvParser.setValue(RadiusColumnName, "");
    csvParser.setValue(SourceIdColumnName, "");

    csvParser.setValue(TypeColumnName, csvParser.getValue(TypeColumnName));
    csvParser.setValue(SpectralTypeColumnName, csvParser.getValue(SpectralTypeColumnName));

    populateNames(csvParser);
    populateObjectType(csvParser);
    populateLuminosityAndRadius(csvParser);
    populateGalacticCoordinates(csvParser);

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

void SimbadRowProcessor::appendIfInCatalog(const vector<string>& cats, vector<string>& names, const string& catalogue)
{
    for(const auto& cat : cats)
    {
        if(cat.starts_with(catalogue))
        {
            names.push_back(getCleanName(cat));
        }
    }
}

void SimbadRowProcessor::populateNames(const CsvParser& csvParser)
{
    const auto cats = Thx::split(csvParser.getValue(Name1ColumnName), "|");
    auto names = vector<string>();

    {
        const auto mainId = csvParser.getValue(MainIdColumnName);

        if(mainId.starts_with("NAME "))
            names.push_back(getCleanName(mainId));
        else if(!mainId.starts_with("Gaia") && mainId.find("*") == string::npos && hasUpperAndLower(mainId))
            names.push_back(getCleanName(mainId));
    }

    for(const auto& name : cats)
    {
        if(name.starts_with("NAME "))
            names.push_back(getCleanName(name));
        else if(name.starts_with("Gaia DR3 "))
            csvParser.setValue(SourceIdColumnName, Thx::trim(name.substr(strlen("Gaia DR3 "))));
    }

    // Puke, but we need precidence...
    // A space at the end of the catalogue name is important.
    appendIfInCatalog(cats, names, "* ");
    appendIfInCatalog(cats, names, "** ");
    appendIfInCatalog(cats, names, "V* ");
    appendIfInCatalog(cats, names, "Wolf ");
    appendIfInCatalog(cats, names, "Ross ");
    appendIfInCatalog(cats, names, "HD ");
    appendIfInCatalog(cats, names, "GJ ");
    appendIfInCatalog(cats, names, "Gaia DR3 ");
    appendIfInCatalog(cats, names, "2MASS ");

    csvParser.setValue(Name1ColumnName, names.empty() ? "" : Thx::deleteDuplicateSpaces(names[0]));
    csvParser.setValue(Name2ColumnName, names.size() > 1 ? Thx::deleteDuplicateSpaces(names[1]) : "");
    csvParser.setValue(Name3ColumnName, names.size() > 2 ? Thx::deleteDuplicateSpaces(names[2]) : "");
    csvParser.setValue(Name4ColumnName, names.size() > 3 ? Thx::deleteDuplicateSpaces(names[3]) : "");
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

void SimbadRowProcessor::populateLuminosityAndRadius(const CsvParser& csvParser)
{
    const auto teff = static_cast<int>(csvParser.getValueAsDouble(TeffColumnName).value());

    if (LuminanceCalculator::teffIsInGBandCalculationRange(teff))
    {
        const auto mag = csvParser.getValueAsDouble(VMagColumnName);
        if (mag.has_value())
        {
            const auto parsecs = AstronomyConverter::toParsecsMas(csvParser.getValueAsDouble(ParallaxColumnName).value());
            const auto luminosityLSol = LuminanceCalculator::calculateGBand(mag.value(), parsecs, teff);

            if (isfinite(luminosityLSol))
            {
                const auto radius = RadiusCalculator::calculateWithLSol(luminosityLSol, teff);

                if (isfinite(radius))
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
    auto teff = csvParser.getValueAsDouble("teff");

    if(teff.has_value())
        return true;

    const auto bMag = csvParser.getValueAsDouble(BMagColumnName);
    const auto vMag = csvParser.getValueAsDouble(VMagColumnName);

    teff = TeffCalculator::calculate(bMag, vMag);

    if (!teff.has_value())
    {
        const auto objectType = csvParser.getValueAsInteger(ObjectTypeColumnName).value();

        // All stars require a temperature.
        if((objectType & static_cast<int>(Star)) == 0)
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

    name = Thx::trim(name);

    return name;
}
