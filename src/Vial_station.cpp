
// Vial_station.cpp

#include "mbed.h"

// pes board pin map
#include "PESBoardPinMap.h"

// drivers
#include "Vial_station.h"
#include "DebounceIn.h"

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

DigitalIn Vial_0(PA_3);
DigitalIn Vial_1(PA_2);
DigitalIn Vial_2(PA_10);
DigitalIn Vial_3(PB_3);
DigitalIn Vial_4(PB_5);



//##########################################################################################
// Erhalte die Maske der Vials
//##########################################################################################
//
// return      uint16_t          Maske der Vials
bool getMask_Vialstation(){
    if(Vial_0 + Vial_1 + Vial_2 + Vial_3 + Vial_4){
        return 1;
    }else{
         return 0;
    }     

    return 0;
}

//##########################################################################################
// erhalte, ob vial vorhanden
//##########################################################################################
//
// return       bool            true = Vial vorhanden
bool vialPresent_VialStation(){

    switch (colum)
    {
    case 1:
        if(Vial_0){
        return 1;
        }else{
        return 0;
        }
        break;
    
    case 2:
        if(Vial_1){
        return 1;
        }else{
        return 0;
        }
        break;    
    
    case 3:
        if(Vial_2){
        return 1;
        }else{
        return 0;
        }
        break;   
        
    case 4:
        if(Vial_3){
        return 1;
        }else{
        return 0;
        }
        break;    

    case 5:
        if(Vial_4){
        return 1;
        }else{
        return 0;
        }
        break;       
    
    }

    
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