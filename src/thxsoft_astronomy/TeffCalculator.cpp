#include "TeffCalculator.h"
#include <cmath>

std::optional<double> TeffCalculator::calculate(const std::optional<double>& bMag, const std::optional<double>& vMag)
{
    const auto bv = bMag.has_value() && vMag.has_value() ? bMag.value() - vMag.value() : NAN;

    if(bv == NAN)
        return std::nullopt;

    return calculate(bv);
}

std::optional<double> TeffCalculator::calculate(const double bMag, const double vMag)
{
    return calculate(bMag - vMag);
}

std::optional<double> TeffCalculator::calculate(const double bvMag)
{
    const auto teff = static_cast<int>(4600 * ((1 / ((0.92 * bvMag) + 1.7)) + (1 / ((0.92 * bvMag) + 0.62))));

    return teff < 500 ? std::nullopt : std::optional<double>(teff);
}

std::shared_ptr<EffectiveTemperature> TeffCalculator::calculate(
    const double loggSurfaceGravity,
    double photGMeanMag,
    const double photBpMeanMag,
    const double photRpMeanMag,
    double feh,
    const double ebvReddening,
    double j2Mass,
    double h2Mass,
    double k2Mass,
    const double ej2,
    const double eh2,
    const double ek2)
{
    static const double cpol[12][15] =
    {
        { 7980.8845, -4138.3457, 1264.9366, -130.4388, 0.0, 285.8393, -324.2196, 106.8511, -4.9825, 0.0, 4.5138, -203.7774, 126.6981, -14.7442, 40.7376 }, // BP-RP
        { 8172.2439, -2508.6436, 442.6771, -25.3120, 0.0, 251.5862, -240.7094, 86.0579, -11.2705, 0.0, -45.9166, -137.4645, 75.3191, -8.7175, 21.5739 }, // BP-J
        { 8158.9380, -2146.1221, 368.1630, -24.4624, 0.0, 231.8680, -170.8788, 52.9164, -6.8455, 0.0, -45.5554, -142.9127, 55.2465, -4.1694, 17.6593 }, // BP-H
        { 8265.6045, -2124.5574, 355.5051, -23.1719, 0.0, 209.9927, -161.4505, 50.5904, -6.3337, 0.0, -27.2653, -160.3595, 67.9016, -6.5232, 16.5137 }, // BP-K

        { 9046.6493, -7392.3789, 2841.5464, 0.0, -85.7060, 0.0, -88.8397, 80.2959, 0.0, -15.3872, 0.0, 54.6816, 0.0, 0.0, -32.9499 }, // RP-J
        { 8870.9090, -4702.5469, 1282.3384, 0.0, -15.8164, 0.0, -30.1373, 27.9228, 0.0, -4.8012, 0.0, 25.1870, 0.0, 0.0, -22.3020 }, // RP-H
        { 8910.6966, -4305.9927, 1051.8759, 0.0, -8.6045, 0.0, -76.7984, 55.5861, 0.0, -3.9681, 0.0, 35.4718, 0.0, 0.0, -16.4448 }, // RP-K

        { 8142.3539, -3003.2988, 499.1325, -4.8473, 0.0, 244.5030, -303.1783, 125.8628, -18.2917, 0.0, -125.8444, 59.5183, 0.0, 0.0, 16.8172 }, //  G-J
        { 8133.8090, -2573.4998, 554.7657, -54.0710, 0.0, 229.2455, -206.8658, 68.6489, -10.5528, 0.0, -124.5804, 41.9630, 0.0, 0.0, 7.9258 }, //  G-H
        { 8031.7804, -1815.3523, 0.0, 70.7201, -1.7309, 252.9647, -342.0817, 161.3031, -26.7714, 0.0, -120.1133, 42.6723, 0.0, 0.0, 10.0433 }, //  G-K

        { 7346.2000, 5810.6636, 0.0, -2880.3823, 669.3810, 415.3961, 2084.4883, 3509.2200, 1849.0223, 0.0, -49.0748, 6.8032, 0.0, 0.0, -100.3419 }, // G-BP
        { 8027.1190, -5796.4277, 0.0, 1747.7036, -308.7685, 248.1828, -323.9569, -120.2658, 225.9584, 0.0, -35.8856, -16.5715, 0.0, 0.0, 48.5619 } // G-RP
    };

    static constexpr double Crg[3] = { 2.609, -0.475, 0.053 };
    static constexpr double Crb[4] = { 2.998, -0.140, -0.175, 0.062 };
    static constexpr double Crr[2] = { 1.689, -0.059 };

    //  colour range for dwarfs
    static constexpr double Dr[12] = { 2.00, 3.00, 4.00, 4.20, 1.05, 1.60, 1.85, 2.10, 2.60, 2.80, -0.15, 0.85 };
    static constexpr double Db[12] = { 0.20, 0.25, 0.40, 0.30, 0.20, 0.20, 0.20, 0.15, 0.25, 0.20, -1.00, 0.15 };

    //  colour range for giants
    static constexpr double Gr[12] = { 2.55, 4.20, 4.90, 5.30, 1.55, 2.45, 2.70, 2.80, 3.70, 3.90, -0.15, 1.15 };
    static constexpr double Gb[12] = { 0.20, 0.90, 0.40, 0.30, 0.60, 0.20, 0.20, 1.00, 0.25, 0.20, -1.40, 0.15 };

    // Fitzpatrick/Schlafly extinction coefficients
    const double itbr = 0.8;
    const double cRj = 0.719;
    const double cRh = 0.455;
    const double cRk = 0.306;

    const auto ok =
        (std::isfinite(photBpMeanMag) && std::isfinite(photRpMeanMag) &&
         photBpMeanMag >= 5 && photRpMeanMag >= 5 && std::isfinite(ebvReddening) &&
         std::isfinite(loggSurfaceGravity) && std::isfinite(feh) &&
         ebvReddening >= 0 && loggSurfaceGravity >= 0 && loggSurfaceGravity <= 5 && feh >= -8 && feh <= 0.6);

    if (ok)
    {
        // stars with -8 <= feh < -4 are assigned feh = -4.
        if (feh < -4.0)
            feh = -4.0;

        // remove Gaia G<6 because of uncalibrated CCD saturation. Also remove Inf
        if (photGMeanMag < 6.0 || !std::isfinite(photGMeanMag))
            photGMeanMag = NAN;

        // if G<8 from DR3, then correct for saturation as per Riello et al. (2021)
        // Note that G<6 are excluded anyway
        if (photGMeanMag < 8.0)
            photGMeanMag = photGMeanMag - 0.09892 + 0.059 * photGMeanMag - 0.009775 * photGMeanMag * photGMeanMag + 0.0004934 * photGMeanMag * photGMeanMag * photGMeanMag;

        // more house-cleaning to remove 2MASS photometry likely to be bad
        if (j2Mass < 5 || ej2 > 0.05) j2Mass = NAN;
        if (h2Mass < 5 || eh2 > 0.05) h2Mass = NAN;
        if (k2Mass < 5 || ek2 > 0.05) k2Mass = NAN;

        // compute colour dependent extinction coefficients
        const auto bprp = (photBpMeanMag - photRpMeanMag) - itbr * ebvReddening;
        const auto rGg = Crg[0] + Crg[1] * bprp + Crg[2] * bprp * bprp;
        const auto rBp = Crb[0] + Crb[1] * bprp + Crb[2] * bprp * bprp + Crb[3] * bprp * bprp * bprp;
        const auto rRp = Crr[0] + Crr[1] * bprp;


        double clr0[12];
        double teffCalc[12];

        clr0[0] = photBpMeanMag - photRpMeanMag - (rBp - rRp) * ebvReddening;
        clr0[1] = photBpMeanMag - j2Mass - (rBp - cRj) * ebvReddening;
        clr0[2] = photBpMeanMag - h2Mass - (rBp - cRh) * ebvReddening;
        clr0[3] = photBpMeanMag - k2Mass - (rBp - cRk) * ebvReddening;
        clr0[4] = photRpMeanMag - j2Mass - (rRp - cRj) * ebvReddening;
        clr0[5] = photRpMeanMag - h2Mass - (rRp - cRh) * ebvReddening;
        clr0[6] = photRpMeanMag - k2Mass - (rRp - cRk) * ebvReddening;
        clr0[7] = photGMeanMag - j2Mass - (rGg - cRj) * ebvReddening;
        clr0[8] = photGMeanMag - h2Mass - (rGg - cRh) * ebvReddening;
        clr0[9] = photGMeanMag - k2Mass - (rGg - cRk) * ebvReddening;
        clr0[10] = photGMeanMag - photBpMeanMag - (rGg - rBp) * ebvReddening;
        clr0[11] = photGMeanMag - photRpMeanMag - (rGg - rRp) * ebvReddening;

        // derive Teff in all colour indices
        for (auto j = 0; j < 12; j++)
            teffCalc[j] = cpol[j][0] + cpol[j][1] * clr0[j] +
                          cpol[j][2] * clr0[j] * clr0[j] +
                          cpol[j][3] * clr0[j] * clr0[j] * clr0[j] +
                          cpol[j][4] * clr0[j] * clr0[j] * clr0[j] * clr0[j] * clr0[j] +
                          cpol[j][5] * loggSurfaceGravity +
                          cpol[j][6] * loggSurfaceGravity * clr0[j] +
                          cpol[j][7] * loggSurfaceGravity * clr0[j] * clr0[j] +
                          cpol[j][8] * loggSurfaceGravity * clr0[j] * clr0[j] * clr0[j] +
                          cpol[j][9] * loggSurfaceGravity * clr0[j] * clr0[j] * clr0[j] * clr0[j] * clr0[j] +
                          cpol[j][10] * feh + cpol[j][11] * feh * clr0[j] +
                          cpol[j][12] * feh * clr0[j] * clr0[j] +
                          cpol[j][13] * feh * clr0[j] * clr0[j] * clr0[j] +
                          cpol[j][14] * feh * loggSurfaceGravity * clr0[j];

        for (auto j = 0; j < 12; j++)
        {
            // remove Teffs outside of colour cuts
            if ((loggSurfaceGravity > 3.2 && (clr0[j] > Dr[j] || clr0[j] < Db[j])) || !std::isfinite(teffCalc[j]))
            {
                teffCalc[j] = NAN;
            }

            if ((loggSurfaceGravity <= 3.2 && (clr0[j] > Gr[j] || clr0[j] < Gb[j])) || !std::isfinite(teffCalc[j]))
            {
                teffCalc[j] = NAN;
            }
        }

        auto count = 0;
        auto avg = 0.0;
        for (double j : teffCalc)
        {
            if(std::isfinite(j))
            {
                count++;
                avg += j;
            }
        }

        avg = avg / count;

        if (avg > 0) // Puke, but no stars will be 0 degrees.
        {
            EffectiveTemperature et {};

            et.colorIndex0 = teffCalc[0],
            et.colorIndex1 = teffCalc[1],
            et.colorIndex2 = teffCalc[2],
            et.colorIndex3 = teffCalc[3],
            et.colorIndex4 = teffCalc[4],
            et.colorIndex5 = teffCalc[5],
            et.colorIndex6 = teffCalc[6],
            et.colorIndex7 = teffCalc[7],
            et.colorIndex8 = teffCalc[8],
            et.colorIndex9 = teffCalc[9],
            et.colorIndex10 = teffCalc[10],
            et.colorIndex11 = teffCalc[11],
            et.teff = static_cast<int>(avg);

            return std::make_shared<EffectiveTemperature>(et);
        }

        //          Console.WriteLine("*** No star passes quality requirements ***");
    }

//        Console.WriteLine("*** No star satisfies basic input requirements to derive its Teff ***");
    return nullptr;
}
