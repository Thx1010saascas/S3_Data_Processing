#pragma once

struct UnitConverter
{
    static double metersToFeet(const double meters) { return meters * 3.280839895; }
    static double feetToMeters(const double feet) { return feet / 3.280839895; }
    static double nauticalMilesToFeet(const double nm) { return nm * 6076.11549; }
    static double feetToNauticalMiles(const double ft) { return ft / 6076.11549; }
    static double milesPhToNauticalMilesPh(const double mph) { return mph / 1.151; }
};
