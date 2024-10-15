#pragma once
#include <string>
#include <format>
#include "SimbadObjectType.h"


namespace thxsoft::export_s3_sectors
{
    struct CelestialObject
    {
        long long index;
        std::string name1;
        std::string name2;
        std::string name3;
        std::string name4;
        std::optional<std::string> spectralType;
        std::string sectorId;
        std::optional<double> teff;
        double parallax;
        double parsecs;
        std::optional<double> magnitudeVorB;
        std::optional<double> radius;
        std::optional<double> luminosity;
        std::optional<double> metallicity;
        double x;
        double y;
        double z;
        std::optional<long long> sourceId;
        bool isBinary;
        SimbadObjectTypes type;

        [[nodiscard]] std::string toString() const
        {
            return format("{} X={},Y={},Z={}", name1, z, y, z);
        }
    };
}