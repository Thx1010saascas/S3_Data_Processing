#pragma once

struct RadiusCalculator {

    static double calculateWithLSol(double lSol, int effectiveTemperature);
    static double calculateWithWatts(double luminosityInWatts, int effectiveTemperature);
};
