#include "BolometricCalculator.h"
#include "AstronomyConstants.h"
#include <complex>

using namespace thxsoft::astronomy;

static constexpr double BCg4000To8000K[5][2] =
{
    { 0.06, 0.02634},
    { 0.00006731, 0.00002438},
    { -0.00000006647, -0.000000001129},
    { 0.00000000002859, -0.000000000006722},
    { -0.000000000000007197, 0.000000000000001635}
};

static constexpr double BCg3300To4000K[5][2]
{
    { 1.749, -2.487},
    { 0.001977, -0.001876},
    { 0.0000003737, 0.0000002128},
    { -0.00000000008966, 0.0000000003807},
    { -0.00000000000004183, 0.00000000000006570}
};

double BolometricCalculator::getCorrectionFactor(const int teff)
{
    const auto teffOffset = teff - AstronomyConstants::SolEffectiveTemperature;
    const auto array = teff >= 4000 ? BCg4000To8000K : BCg3300To4000K;
    auto bcg = 0.0;
            
    for (auto i = 0; i < 5; i++)
    {
        const auto power = pow(teffOffset, i);
            
        bcg += array[i][0] * power;
    }

    return bcg;
}
    
