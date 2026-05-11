//=====================================================================================
// PM4 Main File
//=====================================================================================

#include "mbed.h"

// pes board pin map
#include "PESBoardPinMap.h"

// drivers
#include "DebounceIn.h"

// PM4-Projekt
#include "desedimentation.h"
#include "Input_Output.h"
#include "Scara.h"
#include "Vial_station.h"
#include <cstdio>

// standartwerte definieren für lesbarkeit

#define PRINTFACTIVE true


//#####################################################################################
// PM2 Globale Deklarationen
//#####################################################################################

Scara& getScara() {
    static Scara scara(PRINTFACTIVE);
    return scara;
}



//#####################################################################################
// PM4 INIT-Funktion
//#####################################################################################
void pm4init(void) {

    // init der Module
    getScara();
    return;
}


//#####################################################################################
// PM4 MAIN-Funktion
//#####################################################################################
void pm4main(void) {

    getScara().cycle();

    return;
}


//#####################################################################################
// PM4 RESET-Funktion
//#####################################################################################
void pm4reset(void) {

    reset_desedimentation();

    if (PRINTFACTIVE) printf("* * * RESET * * *");
    
    getScara().reset();

    return;
}
