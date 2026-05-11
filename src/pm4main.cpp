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
   
    
    init_desedimentation();

    init_VialStation();
    
   
    
    return;
}




//#####################################################################################
// PM4 MAIN-Funktion
//#####################################################################################
void pm4main(void) {
    
    // cycle Funktionen aufrufen
   
    cycle_desedimentation();

   
    cycle_InputOuput();
    
    if(getMask_VialStation()){
        vialPresent_VialStation();
    }
   
    

    //=================================================================================
    // SCHRITTKETTE ROBOTERABLAUF
    //=================================================================================
    switch (status) {
        //-----------------------------------------------------------------------------
        // Initialisierung
        //-----------------------------------------------------------------------------
        case Initialieren:
            // init der Module im pm2init
            
            
            break;
            
        
        //-----------------------------------------------------------------------------
        // Fehlerstatus
        //-----------------------------------------------------------------------------
        case Fehler:
            // 
            
                printf("********** Fehler **********\n");

            break;
            
        //-----------------------------------------------------------------------------
        // DEFAULT
        //-----------------------------------------------------------------------------
        default: 
            // gehe zu Fehler
            status = Fehler;
            break;
    }
    //=================================================================================
    // ENDE DES ROBOTERABLAUF
    //=================================================================================
    


    return;
}


//#####################################################################################
// PM4 RESET-Funktion
//#####################################################################################
void pm4reset(void) {

    reset_desedimentation();

    if (PRINTFACTIVE) printf("* * * RESET * * *");
    return;
}
