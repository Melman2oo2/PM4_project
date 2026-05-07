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
#define GERADEAUS 0.0f
#define NORMAL_GESCH 1.0f

#define PRINTFACTIVE true


//#####################################################################################
// PM2 Globale Deklarationen
//#####################################################################################

static Scara& getScara() {
    static Scara scara;
    return scara;
}


// Ablaufschritte
typedef enum {
    Initialieren,

    Ende,
    Fehler
} t_RoboterStatus;


// Schritt des Roboterablaufes 
t_RoboterStatus status;

// Flags



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
    // Point temp = scara.current;
    // scara.target.setCartesian(20.0f, 0.0f, -50.0f);
    
    getScara().printstate();

    return;
}


//#####################################################################################
// PM4 RESET-Funktion
//#####################################################################################
void pm4reset(void) {

    if (PRINTFACTIVE) printf("* * * RESET * * *");
    getScara().stop();

    // scara.disable();

    return;
}
