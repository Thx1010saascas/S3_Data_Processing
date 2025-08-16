
#pragma once
#include <memory>
#include <optional>

struct EffectiveTemperature;

struct TeffCalculator
{
    static std::optional<double> calculate(const std::optional<double>& bMag, const std::optional<double>& vMag);
    static std::optional<double> calculate(const double bMag, const double vMag);
    static std::optional<double> calculate(double bvMag);

    static std::shared_ptr<EffectiveTemperature> calculate(
        double loggSurfaceGravity,
        double photGMeanMag,
        double photBpMeanMag,
        double photRpMeanMag,
        double feh = 0,
        double ebvReddening = 0,
        double j2Mass = 0,
        double h2Mass = 0,
        double k2Mass = 0,
        double ej2 = 0.022,
        double eh2 = 0.024,
        double ek2 = 0.022);
};

struct EffectiveTemperature
{
    double colorIndex0;
    double colorIndex1;
    double colorIndex2;
    double colorIndex3;
    double colorIndex4;
    double colorIndex5;
    double colorIndex6;
    double colorIndex7;
    double colorIndex8;
    double colorIndex9;
    double colorIndex10;
    double colorIndex11;
    double colorIndex12;
    int teff;
};
