
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

<<<<<<< HEAD
=======

Servo Servo_desedimentation(PB_D3);


//..........................................................................................
// Schalter
//..........................................................................................



>>>>>>> origin/Loschua


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
    printf("%d\n", desedimentation_timer);

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
    stop_desedimentation();
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
// beendung der Desedimentation
//##########################################################################################
//
// beendet deîe Desedimentation so, dass das Vial oben ist
void stop_desedimentation(){

    Servo_input = 0.0f;
     
   


   
    return;
}

//##########################################################################################
// enabled desedimentation
//##########################################################################################
//
// enablet den Servo
void enable_desedimentation(){
    if (!Servo_desedimentation.isEnabled()){
        Servo_desedimentation.enable();
    }
    return;
}
//##########################################################################################
// disabled desedimentation
//##########################################################################################
//
// disabled den Servo
void disable_desedimentation(){
    if (Servo_desedimentation.isEnabled()){
        Servo_desedimentation.disable();
    }
    return;
}
// Rueckmeldungen

//##########################################################################################
// Statusabfrage, ob Servo aktiviert ist
//##########################################################################################
//
// return       bool            true = Aktiviert
bool isEnabled_desedimentation(){
    if (Servo_desedimentation.isEnabled()){
        return 1;
    }
    return 0;
}

//##########################################################################################
// Statusabfrage, ob Servo deaktiviert ist
//##########################################################################################
//
// return       bool            true = deaktiviert
bool isDisabled_desedimentation(){
    if (!Servo_desedimentation.isEnabled()){
        return 1;
    }
    return 0;
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
        
        printf("nicht oben\n");
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