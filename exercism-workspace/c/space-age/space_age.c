#include "space_age.h"

#define EARTH_YEAR_IN_SECONDS 31557600.0

float age(planet_t planet, int64_t seconds)
{
    // Check for invalid planet
    if (planet < MERCURY || planet > NEPTUNE) {
        return -1.0;
    }
    
    // Orbital periods relative to Earth years
    float orbital_periods[] = {
        0.2408467,   // Mercury
        0.61519726,  // Venus
        1.0,         // Earth
        1.8808158,   // Mars
        11.862615,   // Jupiter
        29.447498,   // Saturn
        84.016846,   // Uranus
        164.79132    // Neptune
    };
    
    // Calculate age in Earth years first
    float earth_years = seconds / EARTH_YEAR_IN_SECONDS;
    
    // Divide by the planet's orbital period to get age on that planet
    return earth_years / orbital_periods[planet];
}