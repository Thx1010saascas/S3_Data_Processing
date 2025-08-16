#include "LuminanceCalculator.h"
#include <AstronomyConverter.h>
#include <BolometricCalculator.h>

using namespace thxsoft::astronomy;

// https://www.aanda.org/articles/aa/pdf/2018/08/aa32516-17.pdf

/// <summary>
/// Returns the Visual band luminance is LSol.
/// </summary>
double LuminanceCalculator::calculateVBand(const double vMeanMagnitude, const double parsecs)
{
    const auto mV = vMeanMagnitude - 5.0 * log10(parsecs) + 5;
    const auto luminance = pow(10, ((mV - AstronomyConstants::SolAbsoluteVisualMagnitude) / -2.5));
        
    return luminance;
}

/// <summary>
/// Returns the Green band luminance is LSol.
/// </summary>
double LuminanceCalculator::calculateGBand(const double gMeanMagnitude, const double parsecs, const int teff)
{
    const auto mG = AstronomyConverter::toAbsoluteMagnitude(parsecs, gMeanMagnitude);
    const auto bcg = BolometricCalculator::getCorrectionFactor(teff);
    const auto luminance = pow(10, ((mG + bcg - AstronomyConstants::SolAbsoluteBolometricMagnitude) / -2.5));
        
    return luminance;
}

bool LuminanceCalculator::teffIsInGBandCalculationRange(const int teff)
{
    return teff >= 3090 && teff <= 10000;
}
