#pragma once
#include <map>

enum SimbadObjectTypes
{
    Unknown          = 0,
    Star             = 1,
    StarGroup        = 1 << 1,
    LowMassStar      = 1 << 2,
    BrownDwarf       = 1 << 3,
    Planet           = 1 << 4,
    Cluster          = 1 << 5,
    OpenCluster      = 1 << 6,
    GlobularCluster  = 1 << 7,
    XRay             = 1 << 8,
    NeutronStar      = 1 << 9,
    WhiteDwarf       = 1 << 10,
    Nebula           = 1 << 11,
    SuperNovae       = 1 << 12,
    PartOfGalaxy     = 1 << 14,
    Galaxy           = 1 << 15,
    ActiveGalaxyNucleus= 1 << 16
};

const std::map<std::string_view, SimbadObjectTypes> SimbadObjectTypesMap = {
    {"Star", Star},
    {"StarGroup", StarGroup},
    {"LowMassStar", LowMassStar},
    {"BrownDwarf", BrownDwarf},
    {"Planet", Planet},
    {"Cluster", Cluster},
    {"OpenCluster", OpenCluster},
    {"GlobularCluster", GlobularCluster},
    {"XRay", XRay},
    {"NeutronStar", NeutronStar},
    {"WhiteDwarf", WhiteDwarf},
    {"Nebula", Nebula},
    {"SuperNovae", SuperNovae},
    {"PartOfGalaxy", PartOfGalaxy},
    {"Galaxy", Galaxy},
    {"ActiveGalaxyNucleus", ActiveGalaxyNucleus},
};
