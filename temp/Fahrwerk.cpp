// Fahrwerk.cpp

#include "mbed.h"

// pes board pin map
#include "PESBoardPinMap.h"

// Treiber
#include "FastPWM.h"
#include "DCMotor.h"
#include "LineFollower.h"
#include "Fahrwerk.h"


//##########################################################################################
// Globale Objekte
//##########################################################################################
//
//..........................................................................................
// Motoren
//..........................................................................................
// Enable Power
DigitalOut enable_motors(PB_ENABLE_DCMOTORS);

// Initialisierung diverser Parameter
const float maximaleSpannung = 12.0f;
const float uebersetzungsverhaeltnis = 488.28125f;
const float Motorkonstante = 28.0f / 12.0f;

// Objekte erstellen
DCMotor motorLinks(PB_PWM_M1, PB_ENC_A_M1, PB_ENC_B_M1, uebersetzungsverhaeltnis, Motorkonstante, maximaleSpannung);
DCMotor motorRechts(PB_PWM_M2, PB_ENC_A_M2, PB_ENC_B_M2, uebersetzungsverhaeltnis, Motorkonstante, maximaleSpannung);


//##########################################################################################
// Initalisierung Fahrwerk
//##########################################################################################
//
// Einmaliges Aufrufen am Anfang
void initFahrwerk(){

}

//##########################################################################################
// Wiederholung Fahrwerk
//##########################################################################################
//
// Wiederhohlte Aufrufe (50Hz)
void cycleFahrwerk(){

}


//##########################################################################################
// Funktion zum Fahren
//##########################################################################################
//
// !!!!!!!!!!! Rückwärts macht Probleme !!!!!!!!!!!!!!
//
// Parameter    Geschwindigkeit         Interval [-1 ; 1] (vorwärts ; Rückwärts)
// Parameter    Kurvenateil             Interval [-1 ; 1] (Links ; Rechts)
void setGeschwindigkeit(float geschwindigkeit, float kurvenanteil){
    // Diverse Parameter erstellen
    float links=0.0f, rechts=0.0f;

    // schauen, ob geschw. = 0
    if (geschwindigkeit ==0){
        stopFahrwerk();
        return;
    }

    // kurvenanteil Prüfen
    if (kurvenanteil >= -1 && kurvenanteil <= 1){

        // Funktion links: f(x) = 0.5x+0.5 (0<x)    0.75x+0.5 (-0.66<x<0)    3x+2 (x<0.66)
        if (0<kurvenanteil){
            links = 0.5f*kurvenanteil+0.5f;
        }else if ((-2.0f/3.0f)<kurvenanteil){
            links = 0.75f*kurvenanteil+0.5;
        }else {
            links = 3.0f*kurvenanteil+2.0f;
        }

        // Funktion rechts: f(x) = -0.5x+0.5 (0>x)    -0.75x+0.5 (0.66>x>0)    -3x+2 (x>0.66)
       if (0>kurvenanteil){
            rechts = -0.5f*kurvenanteil+0.5f;
        }else if ((2.0f/3.0f)>kurvenanteil){
            rechts = -0.75f*kurvenanteil+0.5;
        }else {
            rechts = -3.0f*kurvenanteil+2.0f;
        }


        // anpassen der Geschwindigkeit
        links *=geschwindigkeit *1.0f;
        rechts *=geschwindigkeit *1.0f;

        // printf("links: %0.5f\trechts: %0.5f\n", links, rechts);

        // wert setzen
        motorLinks.setVelocity(links);
        motorRechts.setVelocity(rechts);

    }

}

//##########################################################################################
// Ändern der maximalen Beschleunigung
//##########################################################################################
//
// Parameter    beschleunigung          Gewünste Beschleunigung
void setBeschleunigungFahrwerk(float beschleunigung){

    motorLinks.setMaxAcceleration(beschleunigung);
    motorRechts.setMaxAcceleration(beschleunigung);
    // printf("A auf %0.5f\t",beschleunigung);
}

//##########################################################################################
// Stoppen des Fahrzeuges
//##########################################################################################
//
// Direkter Halt des Fahrzeuges
void stopFahrwerk(){

    motorLinks.setVelocity(0.0f);
    motorRechts.setVelocity(0.0f);
}

//##########################################################################################
// Fahrwerk Aktivieren
//##########################################################################################
//
// Aktiviert die Objekte der Motoren
void enableMotoren(){

    enable_motors = 1;
}

//##########################################################################################
// Fahrwerk Deaktivieren
//##########################################################################################
//
// Deaktiviert die Objekte der Motoren
void disableMotoren(){

    enable_motors = 0;
}

//##########################################################################################
// Motion Planner Aktivieren
//##########################################################################################
//
// Aktiviert den Motion Planner für sanftes Anfahren
void enableMotionPlannerFahrwerk(){
    const float ACCELERATION_GAIN = 5.0f;

    // links
    motorLinks.enableMotionPlanner();
    // limit max. acceleration to half of the default acceleration
    motorLinks.setMaxAcceleration(motorLinks.getMaxAcceleration() * ACCELERATION_GAIN);

    // rechts
    motorRechts.enableMotionPlanner();
    // limit max. acceleration to half of the default acceleration
    motorRechts.setMaxAcceleration(motorRechts.getMaxAcceleration() * ACCELERATION_GAIN);
}

//##########################################################################################
// Motion PLaner Deaktivieren
//##########################################################################################
//
// Deaktiviert den Motion Planner
void disableMotionPlannerFahrwerk(){

    motorLinks.disableMotionPlanner();
    motorRechts.disableMotionPlanner();
}


// LineFollower Bastel
float motorSpeedLinks = 0.0;
float motorSpeedRechts = 0.0;

void FolgeDerLinie() {
    motorLinks.setVelocity(motorSpeedLinks);
    motorRechts.setVelocity(motorSpeedRechts);
}

// Rückmeldungen

//##########################################################################################
// Erhalte die Geschwindigkeit
//##########################################################################################
//
// return       float                   Aktuelle Geschwindigkeit
float getAktuelleGeschwindigkeit(){

    float links = motorLinks.getVelocity();
    float rechts = motorRechts.getVelocity();

    float geschwindigkeit = 0;

    // Kurvenwert analysieren
    // wert (x>0.66)
    if (rechts<0){
        geschwindigkeit = (0.5f*rechts+3.0f*links)/2.5f;

    // wert (x<-0.66)
    }else if (links<0){
        geschwindigkeit = (3.0f*rechts+0.5f*links)/2.5f;

    // wert (0<x<0.66)
    }else if (links > rechts){
        geschwindigkeit = (0.5f*rechts+0.75f*links)/0.625f;

    // wert (-0.66=<x<0)
    } else if (rechts > links){
        geschwindigkeit = (0.75f*rechts+0.5f*links)/0.625f;

    // wert (x=0)
    } else if (rechts == links){
        geschwindigkeit = links+rechts;

    // fail save
    } else{

        geschwindigkeit = (links + rechts);
    }

    return geschwindigkeit*0.5f;
}

//##########################################################################################
// Erhalte die maximale Beschleunigung
//##########################################################################################
//
// return       float                   Aktuelle Geschwindigkeit
float getBeschleunigungFahrwerk(){
    // Parameter setzen
    float accl, accr, durchschnitt;

    // einzelne Geschw. erhalten
    accl = motorLinks.getMotionPlanerAcceleration();
    accr = motorRechts.getMotionPlanerAcceleration();

    // Duchschnitt erhalten
    durchschnitt = (accl+accr)/2.0f;
    return durchschnitt;
}

//##########################################################################################
// Erhalte die Linienposition
//##########################################################################################
//
// return       float                   Interval [-1 ; 1] (Links ; Rechts)
float getLinienposition(){
    return 0;
}

//##########################################################################################
// Statusabfrage ob Motoren aktiviert sind
//##########################################################################################
//
// return       bool                    true = Aktiviert
bool isEnabledMotoren(){
    bool returnBool=false;

    if (enable_motors.read() == 1){
        returnBool = true;
        // printf("enabled\t");
    }
    return returnBool;
}

//##########################################################################################
// Statusabfrage ob Motore deaktiviert sind
//##########################################################################################
//
// return       bool                    true = Deaktiviert
bool isDisabledMotoren(){
    bool returnBool=false;

    if (enable_motors.read() == 0){
        returnBool = true;
        // printf("disabled\t");
    }
    return returnBool;
}