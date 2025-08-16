#pragma once

namespace thxsoft::astronomy
{
    struct CartesianCoordinate
    {
        CartesianCoordinate(const double x, const double y, const double z)
         : x(x), y(y), z(z)
        {
        }

        const double x;
        const double y;
        const double z;
    };
}
