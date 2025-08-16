#pragma once
#include "memory"
#include <CartesianCoordinate.h>

namespace thxsoft::astronomy
{
    struct GalacticCoordinate
    {
        static constexpr double SolCentreOffsetX = 26669.932847404576;
        static constexpr double SolCentreOffsetY = 55.857458409526735;
        static constexpr double SolCentreOffsetZ = -21.490911348048357;

        GalacticCoordinate(const double latitude, const double longitude, double* lightYears = nullptr)
            : latitude(latitude), longitude(longitude), lightYears(lightYears)
        {
        }

        enum centrePoints
        {
            Sol,
            SagittariusA
        };

        double latitude;
        double longitude;
        double* lightYears;

        [[nodiscard]]
        std::shared_ptr<CartesianCoordinate> ToCartesianCoordinates(centrePoints centrePoint = SagittariusA) const;
        static std::shared_ptr<GalacticCoordinate> equatorialToGalactic(double rai, double deci);
    };
}
