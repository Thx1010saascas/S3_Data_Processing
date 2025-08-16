#pragma once

#include <cmath>
#include "AstronomyConstants.h"

namespace thxsoft::astronomy
{
    struct AstronomyConverter
    {
        static double toMSol(const double massKg)
        {
            return massKg / AstronomyConstants::SolMassKg;
        }

        static double toRSol(const double radiusMeters)
        {
            return radiusMeters / AstronomyConstants::SolRadiusMeters;
        }

        static double toLSol(const double luminanceInWatts)
        {
            return luminanceInWatts / AstronomyConstants::SolLuminosityWatts;
        }

        static double toParsecsMas(const double milliArcSeconds)
        {
            return toParsecsAs(milliArcSeconds  / 1000.0);
        }

        static double toParsecsAs(const double arcSeconds)
        {
            return 1.0 / arcSeconds;
        }

        static double toLightYears(const double parsecs)
        {
            return parsecs * 3.26156;
        }

        static double toAbsoluteMagnitude(const double parsecs, const double relativeMagnitude)
        {
            return -(5.0 * log10(parsecs) - 5.0) + relativeMagnitude;
        }
    };
}
