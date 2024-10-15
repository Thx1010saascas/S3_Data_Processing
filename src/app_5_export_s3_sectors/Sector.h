#pragma once
#include <string>
#include <format>


namespace thxsoft::export_s3_sectors
{
    struct Sector {
        static constexpr double SectorLengthLy = 100.0;
        static constexpr double HalfSectorLengthLy = SectorLengthLy / 2.0;

        explicit Sector(const int x, const int y, const int z)
            : x(x), y(y), z(z)
        {
        }

        const int x;
        const int y;
        const int z;

        // override int GetHashCode()
        // {
        //     return toString().GetHashCode();
        // }

        bool operator==(Sector const & other) const {
            if (typeid(*this) != typeid(other))
                return false;

            return this->x == other.x && this->y == other.y && this->z == other.z;
        }

        bool operator!=(Sector const & other) const {
            return !(*this == other);
        }

        [[nodiscard]] std::string toString() const
        {
            return std::format("X={}, Y={}, Z={}", z, y, z);
        }

        [[nodiscard]] std::string toFileName() const
        {
            return std::format("Gaia_{}_{}_{}.csv", z, y, z);
        }
    };
}
