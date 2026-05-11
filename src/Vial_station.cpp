
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


static int colum = 1;


//##########################################################################################
// Globale Objekte
//##########################################################################################
//
//..........................................................................................
// Schalter
//..........................................................................................



    DigitalIn Vial_0(PA_9);
    DigitalIn Vial_1(PC_7);
    DigitalIn Vial_2(PB_6);
    DigitalIn Vial_3(PA_7);
    DigitalIn Vial_4(PA_6);

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
bool getMask_VialStation(){
    if(Vial_0 + Vial_1 + Vial_2 + Vial_3 + Vial_4){
        printf("Vial ist umen: ");
        return 1;
    }else{
        
        printf("Vial nicht erkannt\n");
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
    

    switch (colum)
    {
    case 1:
        
        if(Vial_0.read()){
            printf("Vial_0 gefunden\n");
            return Point(1.0, 1.0, 1.0);
        }else{
           nextVial_VialStation();            
        }
        break;
    
    case 2:
        if(Vial_1.read()){
            printf("Vial_1\n");
            return Point(2.0, 2.0, 2.0);
        }else{
            nextVial_VialStation();
        }
        break;    
    
    case 3:
        if(Vial_2.read()){
            printf("Vial_2 gefunden\n");
            return Point(3.0, 3.0, 3.0);
        }else{
            nextVial_VialStation();
        }
        break;   
        
    case 4:
        if(Vial_3.read()){
            printf("Vial_3 gefunden\n");
            return Point(4.0, 4.0, 4.0);
        }else{
            nextVial_VialStation();
        }
        break;    

    case 5:
        if(Vial_4.read()){
            printf("Vial_4 gefunden\n");
            return Point(5.0, 5.0, 5.0);
        }else{
            nextVial_VialStation();
        }
        break;       
    
    }
    return Point(0.0, 0.0, 0.0);
    
} 

//##########################################################################################
// erhalte Nummer des nächsten Vials
//##########################################################################################
//
// return       uint32_t        Nummer des nächsten vials   wenn false, ist letztes vial erreicht
uint32_t nextVial_VialStation(){
      
    colum += 1;

    if(colum > 5){
        colum = 1;
       
    } 
  
    return (colum);
}

