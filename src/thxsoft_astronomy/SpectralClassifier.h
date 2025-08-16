#pragma once
#include "memory"
#include <string>
#include <format>
#include <utility>

namespace thxsoft::astronomy
{
    struct SpectralClassifications
    {
        explicit SpectralClassifications(std::string  classification, const int minTeff, const int maxTeff)
            : classification(std::move(classification)), minTeff(minTeff), maxTeff(maxTeff)
        {
        }

        std::string sequence;
        const std::string classification;
        const int minTeff;
        const int maxTeff;

        std::string to_string()
        {
            return format("{}{}", classification, sequence);
        }
    };

    class SpectralClassifier
    {
        public:
            std::string getSpectralType(int teff, const double* absoluteMagnitude);

        private:
            std::shared_ptr<SpectralClassifications> Find(const std::string& sequence, int teff);

            SpectralClassifications Classifications[73] =
            {
                SpectralClassifications("O0", 49450, 50550),
                SpectralClassifications("O1", 47900, 49449),
                SpectralClassifications("O2", 45900, 47899),
                SpectralClassifications("O3", 43900, 45899),
                SpectralClassifications("O4", 42150, 43899),
                SpectralClassifications("O5", 40450, 42149),
                SpectralClassifications("O6", 38300, 40449),
                SpectralClassifications("O7", 36100, 38299),
                SpectralClassifications("O8", 34200, 36099),
                SpectralClassifications("O9", 32350, 34199),
                SpectralClassifications("B0", 28700, 32349),
                SpectralClassifications("B1", 23300, 28699),
                SpectralClassifications("B2", 18800, 23299),
                SpectralClassifications("B3", 16700, 18799),
                SpectralClassifications("B4", 16050, 16699),
                SpectralClassifications("B5", 15100, 16049),
                SpectralClassifications("B6", 14250, 15099),
                SpectralClassifications("B7", 13150, 14249),
                SpectralClassifications("B8", 11500, 13149),
                SpectralClassifications("B9", 10200, 11499),
                SpectralClassifications("A0", 9500, 10199),
                SpectralClassifications("A1", 9050, 9499),
                SpectralClassifications("A2", 8700, 9049),
                SpectralClassifications("A3", 8425, 8699),
                SpectralClassifications("A4", 8175, 8424),
                SpectralClassifications("A5", 8005, 8174),
                SpectralClassifications("A6", 7835, 8004),
                SpectralClassifications("A7", 7675, 7834),
                SpectralClassifications("A8", 7495, 7674),
                SpectralClassifications("A9", 7310, 7494),
                SpectralClassifications("F0", 7120, 7309),
                SpectralClassifications("F1", 6920, 7119),
                SpectralClassifications("F2", 6785, 6919),
                SpectralClassifications("F3", 6710, 6784),
                SpectralClassifications("F4", 6610, 6709),
                SpectralClassifications("F5", 6450, 6609),
                SpectralClassifications("F6", 6315, 6449),
                SpectralClassifications("F7", 6230, 6314),
                SpectralClassifications("F8", 6115, 6229),
                SpectralClassifications("F9", 5990, 6114),
                SpectralClassifications("G0", 5895, 5989),
                SpectralClassifications("G1", 5815, 5894),
                SpectralClassifications("G2", 5745, 5814),
                SpectralClassifications("G3", 5700, 5744),
                SpectralClassifications("G4", 5670, 5699),
                SpectralClassifications("G5", 5630, 5669),
                SpectralClassifications("G6", 5575, 5629),
                SpectralClassifications("G7", 5515, 5574),
                SpectralClassifications("G8", 5410, 5514),
                SpectralClassifications("G9", 5305, 5409),
                SpectralClassifications("K0", 5220, 5304),
                SpectralClassifications("K1", 5110, 5219),
                SpectralClassifications("K2", 4935, 5109),
                SpectralClassifications("K3", 4710, 4934),
                SpectralClassifications("K4", 4520, 4709),
                SpectralClassifications("K5", 4370, 4519),
                SpectralClassifications("K6", 4200, 4369),
                SpectralClassifications("K7", 4045, 4199),
                SpectralClassifications("K8", 3960, 4044),
                SpectralClassifications("K9", 3890, 3959),
                SpectralClassifications("M0", 3755, 3889),
                SpectralClassifications("M1", 3610, 3754),
                SpectralClassifications("M2", 3495, 3609),
                SpectralClassifications("M3", 3315, 3494),
                SpectralClassifications("M4", 3130, 3314),
                SpectralClassifications("M5", 2935, 3129),
                SpectralClassifications("M6", 2745, 2934),
                SpectralClassifications("M7", 2625, 2744),
                SpectralClassifications("M8", 2475, 2624),
                SpectralClassifications("M9", 2190, 2474),
                SpectralClassifications("L", 1999, 2190),
                SpectralClassifications("T", 1500, 1999),
                SpectralClassifications("Y", 500, 1499)
        };
    };
}
