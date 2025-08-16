#include <cmath>
#include <stdexcept>
#include "GalacticCoordinate.h"
#include "AstronomyConstants.h"

namespace thxsoft::astronomy
{
    std::shared_ptr<GalacticCoordinate> GalacticCoordinate::equatorialToGalactic(const double rai, const double deci)
    {
        double pos1[3];
        double pos2[3];

        /*
           Rotation matrix A_g from Hipparcos documentation eq. 1.5.11.
        */
        constexpr double ag[3][3] =
        {
            { -0.0548755604, +0.4941094279, -0.8676661490 },
            { -0.8734370902, -0.4448296300, -0.1980763734 },
            { -0.4838350155, +0.7469822445, +0.4559837762 }
        };

        /*
           Form position vector in equatorial system from input coordinates.
        */

        const auto r = rai * 15.0 * AstronomyConstants::Degrees2Radians;
        const auto d = deci * AstronomyConstants::Degrees2Radians;
        pos1[0] = cos(d) * cos(r);
        pos1[1] = cos(d) * sin(r);
        pos1[2] = sin(d);

        /*
           Rotate position vector to galactic system, using Hipparcos
           documentation eq. 1.5.13.
        */

        pos2[0] = ag[0][0] * pos1[0] + ag[1][0] * pos1[1] + ag[2][0] * pos1[2];
        pos2[1] = ag[0][1] * pos1[0] + ag[1][1] * pos1[1] + ag[2][1] * pos1[2];
        pos2[2] = ag[0][2] * pos1[0] + ag[1][2] * pos1[1] + ag[2][2] * pos1[2];

        /*
           Decompose galactic vector into longitude and latitude.
        */
        const auto xyproj = sqrt(pos2[0] * pos2[0] + pos2[1] * pos2[1]);

        double g;

        if (xyproj > 0.0)
            g = atan2(pos2[1], pos2[0]);
        else
            g = 0.0;

        auto glon = g * AstronomyConstants::Radians2Degrees;
        if (glon < 0.0)
            glon += 360.0;

        g = atan2(pos2[2], xyproj);
        auto glat = g * AstronomyConstants::Radians2Degrees;

        return std::make_shared<GalacticCoordinate>(glat, glon);
    }

    std::shared_ptr<CartesianCoordinate> GalacticCoordinate::ToCartesianCoordinates(const centrePoints centrePoint) const
    {
        if (lightYears == nullptr)
            throw std::invalid_argument("The number of light years must be set.");

        const auto lat = latitude * AstronomyConstants::Degrees2Radians;
        const auto lon = longitude * AstronomyConstants::Degrees2Radians;

        auto x = *lightYears * cos(lat) * cos(lon);
        auto y = *lightYears * cos(lat) * sin(lon);
        auto z = *lightYears * sin(lat);

        if (centrePoint == SagittariusA)
        {
            x -= SolCentreOffsetX;
            y -= SolCentreOffsetY;
            z -= SolCentreOffsetZ;
        }

        return std::make_shared<CartesianCoordinate>(x, y, z);
    }
}
