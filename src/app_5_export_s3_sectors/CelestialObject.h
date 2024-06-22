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
        optional<string> spectralType;
        string sectorId;
        optional<double> teff;
        double parallax;
        double parsecs;
        optional<double> magnitudeVorB;
        optional<double> radius;
        optional<double> luminosity;
        optional<double> metallicity;
        double x;
        double y;
        double z;
        optional<long long> sourceId;
        bool isBinary;
        SimbadObjectTypes type;

        [[nodiscard]] string toString() const
        {
            return format("{} X={},Y={},Z={}", name1, z, y, z);
        }
    };
}