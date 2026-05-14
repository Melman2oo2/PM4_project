
// Vial_station.cpp

#include "mbed.h"

// pes board pin map
#include "PESBoardPinMap.h"

// drivers
#include "DebounceIn.h"
#include "Vial_station.h"

#include "Point.h"

//##########################################################################################
// Globale Variable
//##########################################################################################




//##########################################################################################
// Globale Objekte
//##########################################################################################
//
//..........................................................................................
// Schalter
//..........................................................................................



    DigitalIn Vial_0(PB_0);
    DigitalIn Vial_1(PA_4);
    DigitalIn Vial_2(PC_5);
    DigitalIn Vial_3(PA_7);
    DigitalIn Vial_4(PB_1);

    void init_VialStation(){
    Vial_0.mode(PullUp);
    Vial_1.mode(PullUp);
    Vial_2.mode(PullUp);
    Vial_3.mode(PullUp);
    Vial_4.mode(PullUp);

    }

//##########################################################################################
// Erhalte die Maske der Vials
//##########################################################################################
//
// return      uint16_t          Maske der Vials
bool hasNextVial_VialStation(){
    if(Vial_0 + Vial_1 + Vial_2 + Vial_3 + Vial_4){
        
        return 1;
    }else{
        
        printf("Vial nicht erkannt\t");
        return 0;
    }     

    return 0;
}

//##########################################################################################
// erhalte, ob vial vorhanden
//##########################################################################################
//
// return       Point            Position des Vials, oder (0,0,0) wenn kein Vial
Point vialPresent_VialStation(){
        
    if(Vial_0.read()){
        printf("Vial_0 gefunden\t");
        return Point(1.0, 1.0, 1.0);
    }
        
    
    
    if(Vial_1.read()){
        printf("Vial_1 gefunden\t");
        return Point(2.0, 2.0, 2.0);
    }   
    
    
    if(Vial_2.read()){
        printf("Vial_2 gefunden\t");
        return Point(3.0, 3.0, 3.0);
    } 
        
    
    if(Vial_3.read()){
        printf("Vial_3 gefunden\t");
        return Point(4.0, 4.0, 4.0);
    }   

    
    if(Vial_4.read()){
        printf("Vial_4 gefunden\t");
        return Point(5.0, 5.0, 5.0);
    }     
    
    
    return Point(0.0, 0.0, 0.0);
    
} 



