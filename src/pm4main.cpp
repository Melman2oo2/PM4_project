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


Scara scara;


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
    scara.init();
    return;
}


//#####################################################################################
// PM4 MAIN-Funktion
//#####################################################################################
void pm4main(void) {

    scara.cycle();
    Point temp = scara.current;
    temp.setCylindrical(temp.getCylindrical().r, 0.5, 0.0f);
    scara.target.setCartesian(temp.getCartesian().x, temp.getCartesian().y, temp.getCartesian().z);
    
    printf("position: ");

    return;
}


//#####################################################################################
// PM4 RESET-Funktion
//#####################################################################################
void pm4reset(void) {

    if (PRINTFACTIVE) printf("* * * RESET * * *");
    scara.stop();
    return;
}
