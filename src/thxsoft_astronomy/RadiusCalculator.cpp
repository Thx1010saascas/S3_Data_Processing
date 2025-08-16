#include "RadiusCalculator.h"
#include <cmath>
#include <AstronomyConstants.h>

using namespace thxsoft::astronomy;

// Returns the radius using LSol.
double RadiusCalculator::calculateWithLSol(const double lSol, const int effectiveTemperature)
{
    return calculateWithWatts(lSol * AstronomyConstants::SolLuminosityWatts, effectiveTemperature);
}

// Returns the radius using watts.
double RadiusCalculator::calculateWithWatts(const double luminosityInWatts, const int effectiveTemperature)
{
    const auto radius = sqrt(luminosityInWatts / (4 * PI * AstronomyConstants::StefanBoltzmannConstant * pow(effectiveTemperature, 4)));

    return radius;
}
