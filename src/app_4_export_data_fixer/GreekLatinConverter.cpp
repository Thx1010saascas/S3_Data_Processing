#include "GreekLatinConverter.h"
#include "Thx.h"

namespace thxsoft::export_data_fixer_db
{
    string GreekLatinConverter::fixName(const string& old_name)
    {
        if (old_name.empty())
            return old_name;

        auto new_name = string();
        const auto words = Thx::split(old_name);

        for(auto word : words)
        {
            if(word == "*")
                continue;

            if(new_name.length() > 0)
                new_name.append(" ");

            if (auto number_index = word.find("0"); number_index != string::npos)
            {
                auto subWord = word.substr(0, number_index);

                new_name.append(_LanguageData.contains(subWord) ? _LanguageData[word] : subWord);

                new_name.append(word.substr(number_index));
            }
            else
            {
                new_name.append(_LanguageData.contains(word) ? _LanguageData[word] : word);
            }
        }

        return new_name;
    }

    GreekLatinConverter::LanguageData GreekLatinConverter::_LanguageData
       {
           // Greek
            { "alf", "Alpha" },
            { "bet", "Beta" },
            { "gam", "Gamma" },
            { "del", "Delta" },
            { "eps", "Epsilon" },
            { "zet", "Zeta" },
            { "eta", "Eta" },
            { "tet", "Theta" },
            { "iot", "Iota" },
            { "kap", "Kapa" },
            { "lam", "Lambda" },
            { "mu.", "Mu" },
            { "nu.", "Nu" },
            { "ksi", "Xi" },
            { "omi", "Omicron" },
            { "pi.", "Pi" },
            { "rho", "Rho" },
            { "sig", "Sigma" },
            { "tau", "Tau" },
            { "ups", "Upsilon" },
            { "phi", "Phi" },
            { "psi", "Psi" },
            { "chi", "Xi" },
            { "ome", "Omega" },


            // Latin
            { "And", "Andromeda" },
            { "Ant", "Antlia" },
            { "Aps", "Apus" },
            { "Aqr", "Aquarius" },
            { "Aql", "Aquila" },
            { "Ara", "Ara" },
            { "Ari", "Aries" },
            { "Aur", "Auriga" },
            { "Boo", "Bootes" },
            { "Cae", "Caelum" },
            { "Cam", "Camelopardalis" },
            { "Cnc", "Cancer" },
            { "CVn", "Canes Venatici" },
            { "CMa", "Canis Major" },
            { "CMi", "Canis Minor" },
            { "Cap", "Capricornus" },
            { "Car", "Carina" },
            { "Cas", "Cassiopeia" },
            { "Cen", "Centaurus" },
            { "Cep", "Cepheus" },
            { "Cet", "Cetus" },
            { "Cha", "Chamaeleon" },
            { "Cir", "Circinus" },
            { "Col", "Columba" },
            { "Com", "Coma Berenices" },
            { "CrA", "Corona Australis" },
            { "CrB", "Corona Borealis" },
            { "Crv", "Corvus" },
            { "Crt", "Crater" },
            { "Cyg", "Cygnus" },
            { "Cru", "Crux" },
            { "Del", "Delphinus" },
            { "Dor", "Dorado" },
            { "Dra", "Draco" },
            { "Eps", "Epsilon" },
            { "Eql", "Equuleus" },
            { "Equ", "Equuleus" },
            { "Eri", "Eridani" },
            { "Eta", "Eta" },
            { "For", "Fornax" },
            { "Gem", "Gemini" },
            { "Gru", "Grus" },
            { "Her", "Hercules" },
            { "Hor", "Horologium" },
            { "Hya", "Hydra" },
            { "Hyi", "Hydrus" },
            { "Ind", "Indus" },
            { "Lac", "Lacerta" },
            { "Leo", "Leo" },
            { "LMi", "Leo Minor" },
            { "Lep", "Lepus" },
            { "Lib", "Libra" },
            { "Lup", "Lupus" },
            { "Lyn", "Lynx" },
            { "Lyr", "Lyra" },
            { "Men", "Mensa" },
            { "Mic", "Microscopium" },
            { "Mon", "Monocerotis" },
            { "Mus", "Musca" },
            { "Nor", "Norma" },
            { "Oct", "Octans" },
            { "Oph", "Ophiuchus" },
            { "Ori", "Orion" },
            { "Pav", "Pavonis" },
            { "Peg", "Pegasus" },
            { "Per", "Perseus" },
            { "Phe", "Phoenix" },
            { "Pic", "Pictor" },
            { "Psc", "Pisces" },
            { "PsA", "Piscis Austrinus" },
            { "Pup", "Puppis" },
            { "Pyx", "Pyxis" },
            { "Ret", "Reticulum" },
            { "Sge", "Sagitta" },
            { "Sgr", "Sagittarius" },
            { "Sco", "Scorpius" },
            { "Scl", "Sculptor" },
            { "Sct", "Scutum" },
            { "Ser", "Serpens" },
            { "Sex", "Sextans" },
            { "Tau", "Taurus" },
            { "tel", "Telescopium" },
            { "Tri", "Triangulum" },
            { "TrA", "Triangulum Australe" },
            { "Tuc", "Tucana" },
            { "UMa", "Ursa Major" },
            { "UMi", "Ursa Minor" },
            { "Vel", "Vela" },
            { "Vir", "Virgo" },
            { "Vol", "Volans" },
            { "Vul", "Vulpecula" }
       };
}