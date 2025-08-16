#include "SpectralClassifier.h"

namespace thxsoft::astronomy
{
    std::string SpectralClassifier::getSpectralType(const int teff, const double* absoluteMagnitude)
    {
        std::shared_ptr<SpectralClassifications> result = nullptr;

        if (absoluteMagnitude != nullptr)
        {
            if ((teff > 15000 && *absoluteMagnitude > 5) ||
                (teff > 8000 && *absoluteMagnitude > 8) ||
                teff > 5500 && *absoluteMagnitude > 10)
            {
                return "WD";
            }

            if ((teff <= 8000 && *absoluteMagnitude > -3 && *absoluteMagnitude < 1.5) ||
                (teff <= 7000 && *absoluteMagnitude > -3 && *absoluteMagnitude < 2) ||
                (teff <= 6500 && *absoluteMagnitude > -3 && *absoluteMagnitude < 3) ||
                (teff <= 6200 && *absoluteMagnitude > -3 && *absoluteMagnitude < 3.5))
            {
                result = Find("II", teff);
            }
            else
            {
                if (teff <= 28000 && *absoluteMagnitude <= -3)
                    result = Find("I", teff);
            }
        }

        if (result == nullptr)
            result = Find("V", teff);

        if (result != nullptr)
            return result->to_string();

        return "";
    }

    std::shared_ptr<SpectralClassifications> SpectralClassifier::Find(const std::string& sequence, const int teff)
    {
        for (auto classification : Classifications)
        {
            if (teff >= classification.minTeff && teff <= classification.maxTeff)
            {
                classification.sequence = sequence;
                return std::make_shared<SpectralClassifications>(classification);
            }
        }

        return nullptr;
    }
}

