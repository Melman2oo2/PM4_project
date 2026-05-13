// Vial_station.h

#include "mbed.h"
#include "Point.h"

extern DigitalIn Vial_0;
extern DigitalIn Vial_1;
extern DigitalIn Vial_2;
extern DigitalIn Vial_3;
extern DigitalIn Vial_4;


#ifndef VIAL_STATION_H
#define VIAL_STATION_H

    // Rueckmeldungen

    void init_VialStation();

    bool hasNextVial_VialStation();

    Point vialPresent_VialStation();   

#endif
