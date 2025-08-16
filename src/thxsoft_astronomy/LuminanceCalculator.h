#pragma once

struct LuminanceCalculator {
    static double calculateVBand(double vMeanMagnitude, double parsecs);
    static double calculateGBand(double gMeanMagnitude, double parsecs, int teff);
    static bool teffIsInGBandCalculationRange(int teff);
};
