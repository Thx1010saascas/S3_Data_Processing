#include "GaiaRowProcessor.h"

#include <format>
#include "AstronomyConverter.h"
#include "LuminanceCalculator.h"
#include "RadiusCalculator.h"
#include "TeffCalculator.h"

using namespace thxsoft::astronomy;


bool GaiaRowProcessor::processRow(const double minparallax, const CsvParser& csvParser)
{
    if(csvParser.getValue(IndexColumnName).empty() ||
        csvParser.getValue(DecColumnName).empty() ||
        csvParser.getValue(ParallaxColumnName).empty())
        return false;

    const auto parallax = csvParser.getValueAsDouble(ParallaxColumnName).value();

    if(parallax < minparallax)
        return false;

    if (csvParser.getValueAsDouble(RuweColumnName) > 2.0)
        return false;

    if (!csvParser.getValue(TeffColumnName).empty())
        csvParser.setValue(TeffSourceColumnName, "1");
    else
    {
        if(!setTeffFromCalculation(csvParser))
            return false; // Can't calculate a temperature so exit.

        populateLuminosityAndRadius(csvParser);
    }

    return true;
}

void GaiaRowProcessor::populateLuminosityAndRadius(const CsvParser& csvParser)
{
    const auto teff = static_cast<int>(csvParser.getValueAsDouble(TeffColumnName).value());

    if (teff >= 3090 && teff <= 10000)
    {
        const auto mag = csvParser.getValueAsDouble(BpMeanMagColumnName);
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

bool GaiaRowProcessor::setTeffFromCalculation(const CsvParser& csvParser)
{
    const auto csvGMeanMag = csvParser.getValue(GMeanMagColumnName);
    const auto csvBpMeanMag = csvParser.getValue(BpMeanMagColumnName);
    const auto csvRpMeanMag = csvParser.getValue(RpMeanMagColumnName);
    const auto csvLoggGspphot = csvParser.getValue(LoggColumnName);

    if (csvLoggGspphot.empty() || csvGMeanMag.empty() || csvBpMeanMag.empty() || csvRpMeanMag.empty())
        return false;

    const auto csvFeh = csvParser.getValue(MhColumnName);
    const auto csvReddening = csvParser.getValue(EbpminrpColumnName);

    const auto temperature = TeffCalculator::calculate(
        stod(csvLoggGspphot),
        stod(csvGMeanMag),
        stod(csvBpMeanMag),
        stod(csvRpMeanMag),
        csvFeh.empty() ? 0 : stod(csvFeh),
        csvReddening.empty() ? 0 : stod(csvReddening));

    if (temperature == nullptr)
        return false;

    csvParser.setValue(TeffColumnName, to_string(temperature->teff));
    csvParser.setValue(TeffSourceColumnName, "2");

    return true;
}


