
// desedimentation.cpp

#include "mbed.h"

// pes board pin map
#include "PESBoardPinMap.h"

// drivers
#include "desedimentation.h"
#include "Servo.h"

//##########################################################################################
// Globale Variable
//##########################################################################################

int desedimentation_timer;

float Servo_input = 0.0f;


const float maxVelocity = 1.0f; 
const float maxAcceleration = 0.3f;






//##########################################################################################
// Globale Objekte
//##########################################################################################
//
//..........................................................................................
// Servo
//..........................................................................................


Servo Servo_desedimentation(PB_D3);


//..........................................................................................
// Schalter
//..........................................................................................





//##########################################################################################
// initialisierung
//##########################################################################################
//
// Einmaliges Aufrufen am Anfang
void init_desedimentation(){

   
    Servo_desedimentation.setMaxAcceleration(maxAcceleration);
    Servo_desedimentation.setMaxVelocity(maxVelocity);
    Servo_desedimentation.calibratePulseMinMax(0.035f, 0.120f);
    
    Servo_desedimentation.enable();
    
    Servo_desedimentation.setPulseWidth(0.0f);

    return;
}

void reset_desedimentation(){
    
    
    Servo_desedimentation.setPulseWidth(0.0f);
    Servo_desedimentation.disable();

}

//##########################################################################################
// Wiederholung Desedimentation
//##########################################################################################
//
// Wiederholte Aufrufe (50Hz)
void cycle_desedimentation(){


static bool forward = true;
  

Servo_desedimentation.setPulseWidth(Servo_input);
    

if(!isTimerDone_desedimentation()){
    
    desedimentation_timer --;
    //printf("%d\n", desedimentation_timer);

    if(forward){
        Servo_input = 0.0f;
        if(Servo_desedimentation.getCurrentPulseWidth() <= 0.01f){
            forward = false;
        }
    }
    if(!forward){
        Servo_input = 1.0f;
        if(Servo_desedimentation.getCurrentPulseWidth() >= 0.99f){
            forward = true;
        }
    }
   }else{
    Servo_input = 0.0f;
   }

   


    return;
}


// Befehle
//##########################################################################################
// Beginn der Desedimentation
//##########################################################################################
//
// startet die desedimentation
void start_desedimentation(){
    
    start_timer(10);
    


    printf("Desedimentation started!\n");
    return;
}

//##########################################################################################
// start Timer
//##########################################################################################
//
// startet den Timer
void start_timer(int time){

    int cycles = time * 50; 
    desedimentation_timer = cycles;


    return;
}




//##########################################################################################
// Statusabfrage, ob vial oben ist
//##########################################################################################
//
// return       bool            true = vial ist oben
bool isVialOben_desedimentation(){
    float what = Servo_desedimentation.getCurrentPulseWidth();
    if(what <= 0.01f){
      
        return 1;
    }else{
        
        printf("Vial ist nicht oben\t");
        return 0;
    }


    
}

//##########################################################################################
// Statusabfrage, ob der timer fertig ist
//##########################################################################################
//
// return       bool            true = timer ist fertig
bool isTimerDone_desedimentation(){
    

    if(desedimentation_timer == 0 ){
       
        return 1;
    }else{
        return 0;
    }
}