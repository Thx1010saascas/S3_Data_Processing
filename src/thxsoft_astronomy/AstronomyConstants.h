#pragma once

namespace thxsoft::astronomy
{
    struct AstronomyConstants
    {
        #define PI 3.141592653589793238462643383279502884L
        static constexpr double Degrees2Radians = 0.017453292519943296;
        static constexpr double Radians2Degrees = 57.295779513082321;

        // North Galactic Pole (NGP) coordinates (J2000)
        static constexpr double NgpRightAscensionRadians = 192.8595 * Degrees2Radians;
        static constexpr double NgpDeclinationRadians = 27.1283 * Degrees2Radians;

        // static constants for Galactic Center (J2000)
        static constexpr double GcRightAscensionRadians = 267.126 * Degrees2Radians;
        static constexpr double GcDeclinationRadians = -28.933 * Degrees2Radians;

        // Galactic longitude of the North Celestial Pole (NCP)
        static constexpr double NcpLongitudeRadians = 122.932 * Degrees2Radians;

        static constexpr double SolEffectiveTemperature = 5772.0;
        static constexpr double StefanBoltzmannConstant = 5.67e-8; // Stefan-Boltzmann constant (Wm^-2 K^-4)

        static constexpr double SolMassKg = 1.9884e30;
        static constexpr double SolRadiusMeters = 6.957e8;
        static constexpr double SolLuminosityWatts = 3.828e26;
        static constexpr double SolAbsoluteVisualMagnitude = 4.83;
        static constexpr double SolAbsoluteBolometricMagnitude = 4.74;
    };
}