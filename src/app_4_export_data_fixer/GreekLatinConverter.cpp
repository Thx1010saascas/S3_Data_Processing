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
            { "And", "Andromedae" },
            { "Ant", "Antliae" },
            { "Aps", "Apodis" },
            { "Aqr", "Aquarii" },
            { "Aql", "Aquilae" },
            { "Ara", "Ara" },
            { "Ari", "Arietis" },
            { "Aur", "Aurigae" },
            { "Boo", "Bootis" },
            { "Cae", "Carinae" },
            { "Cam", "Camelopardalis" },
            { "Cnc", "Cancri" },
            { "CVn", "Canum Venaticorum" },
            { "CMa", "Canis Majoris" },
            { "CMi", "Canis Minoris" },
            { "Cap", "Capricorni" },
            { "Car", "Carina" },
            { "Cas", "Cassiopeiae" },
            { "Cen", "Centauri" },
            { "Cep", "Cephei" },
            { "Cet", "Ceti" },
            { "Cha", "Chamaeleontis" },
            { "Cir", "Circini" },
            { "Col", "Columbae" },
            { "Com", "Comae Berenices" },
            { "CrA", "Coronae Australis" },
            { "CrB", "Coronae Borealis" },
            { "Crv", "Corvi" },
            { "Crt", "Crateris" },
            { "Cyg", "Cygni" },
            { "Cru", "Crucis" },
            { "Del", "Delphini" },
            { "Dor", "Doradus" },
            { "Dra", "draconis" },
            { "Eps", "Epsilon" },
            { "Eql", "Equulei" },
            { "Equ", "Equulei" },
            { "Eri", "Eridani" },
            { "Eta", "Etu" },
            { "For", "Fornacis" },
            { "Gem", "Geminorum" },
            { "Gru", "Gruis" },
            { "Her", "Herculis" },
            { "Hor", "Horologii" },
            { "Hya", "Hydrae" },
            { "Hyi", "Hydri" },
            { "Ind", "Indi" },
            { "Lac", "Lacertae" },
            { "Leo", "Leonis" },
            { "LMi", "Leonis Minoris" },
            { "Lep", "Leporis" },
            { "Lib", "Librae" },
            { "Lup", "Lupi" },
            { "Lyn", "Lyncis" },
            { "Lyr", "Lyrae" },
            { "Men", "Mensae" },
            { "Mic", "Microscopii" },
            { "Mon", "Monocerotis" },
            { "Mus", "Muscae" },
            { "Nor", "Normae" },
            { "Oct", "Octantis" },
            { "Oph", "Ophiuchi " },
            { "Ori", "Orionis" },
            { "Pav", "Pavonis" },
            { "Peg", "Pegasi" },
            { "Per", "Persei" },
            { "Phe", "Phoenicis" },
            { "Pic", "Pictoris" },
            { "Psc", "Piscium" },
            { "PsA", "Piscis Austrini" },
            { "Pup", "Puppis" },
            { "Pyx", "Pyxidis" },
            { "Ret", "Reticuli" },
            { "Sge", "Sagitta" },
            { "Sgr", "Sagittarii" },
            { "Sco", "Scorpii" },
            { "Scl", "Sculptoris" },
            { "Sct", "Scuti" },
            { "Ser", "Serpentis" },
            { "Sex", "Sextantis" },
            { "Tau", "Tauri" },
            { "tel", "Telescopii" },
            { "Tri", "Trianguli" },
            { "TrA", "Trianguli Australis" },
            { "Tuc", "Tucanae" },
            { "UMa", "Ursae Majoris" },
            { "UMi", "Ursae Minoris" },
            { "Vel", "Velorum" },
            { "Vir", "Virginis" },
            { "Vol", "Volantis" },
            { "Vul", "Vulpeculae" }
       };
}