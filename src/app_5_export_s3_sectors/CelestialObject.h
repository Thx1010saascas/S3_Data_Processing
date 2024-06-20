#pragma once
#include <string>
#include <format>
#include "SimbadObjectType.h"

using namespace std;

namespace thxsoft::export_s3_sectors
{
    struct CelestialObject
    {
        long long index;
        string name1;
        string name2;
        string name3;
        string name4;
        string spectralType;
        string sectorId;
        const int* teff;
        double parallax;
        double parsecs;
        const double* magnitudeVorB;
        const double* radius;
        const double* luminosity;
        const double* metallicity;
        double x;
        double y;
        double z;
        const long long* sourceId;
        bool isBinary;
        SimbadObjectTypes type;

        [[nodiscard]] string toString() const
        {
            return format("{} X={},Y={},Z={}", name1, z, y, z);
        }
    };
}