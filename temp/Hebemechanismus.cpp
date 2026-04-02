
// Hebemechanismus.cpp

#include "mbed.h"

// pes board pin map
#include "PESBoardPinMap.h"

// drivers
#include "Hebemechanismus.h"
#include "Servo.h"

//##########################################################################################
// Globale Variable
//##########################################################################################
#define OBENLINKS 1.0f
#define UNTENLINKS 0.0f
#define OBENRECHTS 0.0f
#define UNTENRECHTS 1.0f
#define DELTA 0.001f


//##########################################################################################
// Globale Objekte
//##########################################################################################
//
//..........................................................................................
// Servo
//..........................................................................................

// servo-objekt erstellen
Servo servoLinks(PB_D1);
Servo servoRechts(PB_D3);
bool doServoStop = false;




//##########################################################################################
// initialisierung Servo
//##########################################################################################
//
// Einmaliges Aufrufen am Anfang
void initHebemechanismus(){
    
    // min und max PWM links
    float minPWMLinks = 0.011f;
    float maxPWMLinks = 0.110f;

    // min und max PWM rechts;
    float minPWMRechts = 0.011f;
    float maxPWMRechts = 0.115f;

    // PWM-Werte übergeben
    servoLinks.calibratePulseMinMax(minPWMLinks, maxPWMLinks);
    servoRechts.calibratePulseMinMax(minPWMRechts, maxPWMRechts);

}

//##########################################################################################
// Wiederholung Servo
//##########################################################################################
//
// Wiederholte Aufrufe (50Hz)
void cycleHebemechanismus(){
    if (doServoStop){
        StopHebemechanismus();
    }

}

// Befehle

//##########################################################################################
// Gewicht Absenken
//##########################################################################################
// Senkt das Gesicht nach unten
void GewichtAbsenken(){

    servoLinks.setPulseWidth(UNTENLINKS);
    servoRechts.setPulseWidth(UNTENRECHTS);
}

//##########################################################################################
// Gewicht anheben
//##########################################################################################
//
// Hebt das Gewicht nach oben
void GewichtAnheben(){
    servoLinks.setPulseWidth(OBENLINKS);
    servoRechts.setPulseWidth(OBENRECHTS); 
}

//##########################################################################################
// Gewicht links positionieren
//##########################################################################################
//
// Setzt das Gewicht links auf eine bestimmte Position
void GewichtSetPositionLinks(float position) {
    servoLinks.setPulseWidth(position);
}

//##########################################################################################
// Gewicht rechts positionieren
//##########################################################################################
//
// Setzt das Gewicht rechts auf eine bestimmte Position
void GewichtSetPositionRechts(float position){
    servoRechts.setPulseWidth(position);
}

//##########################################################################################
// Beschleunigung anpassen Servo
//##########################################################################################
//
// Setzt die maximale Beschleunigung der Servos
void setBeschleunigungHebemechanismus(float beschleunigung){
    servoLinks.setMaxAcceleration(beschleunigung);
    servoRechts.setMaxAcceleration(beschleunigung);
}

//##########################################################################################
// Stoppen der Servo
//##########################################################################################
//
// Stoppt die Servo in der Aktuellen Position
void StopHebemechanismus(){
    doServoStop = true;
    servoLinks.setPulseWidth(servoLinks.getCurrentPulseWidth());
    servoRechts.setPulseWidth(servoRechts.getCurrentPulseWidth());

    if (!GewichtInBewegung()){
        doServoStop = false;
    }
}

//##########################################################################################
// Enable Servo
//##########################################################################################
//
// Aktiviert die Servo
void enableHebemechanismus(){
    if (!servoLinks.isEnabled())
        servoLinks.enable();
    if (!servoRechts.isEnabled())
        servoRechts.enable();
}

//##########################################################################################
// Disable Servo
//##########################################################################################
//
// Deaktiviert die Servo
void disableHebemechanismus(){
    if (servoLinks.isEnabled()){
        servoLinks.disable();
    }
    if (servoRechts.isEnabled()){
        servoRechts.disable();
    }
}

// Rückmeldungen

//##########################################################################################
// erhalte Gewichtszustand
//##########################################################################################
//
// Rückgabe
bool GewichtUnten(){
    bool returnWert = false;
    float curLinks = servoLinks.getCurrentPulseWidth();
    float curRechts = servoRechts.getCurrentPulseWidth();

    // wenn nahe an Sollwert, true
    if (  (isCloseto(curLinks, UNTENLINKS, DELTA))  &&  (isCloseto(curRechts, UNTENRECHTS, DELTA))  ){
        returnWert = true;
    }

    return returnWert;
}

//##########################################################################################
// erhalte Gewichtszustand
//##########################################################################################
//
// 
bool GewichtOben(){
    // Parameter Definieren
    bool returnWert = false;
    float curLinks = servoLinks.getCurrentPulseWidth();
    float curRechts = servoRechts.getCurrentPulseWidth();

    // wenn nahe an Sollwert, true
    if (  (isCloseto(curLinks, OBENLINKS, DELTA))  &&  (isCloseto(curRechts, OBENRECHTS, DELTA))  ){
        returnWert = true;
    }

    return returnWert;
}

//##########################################################################################
// erhalte Gewichtszustand
//##########################################################################################


bool GewichtInBewegung(){
    // printf("currentspeed: %0.5f\t", servoLinks.getCurrentVelocity());
    if (!isCloseto(servoLinks.getCurrentVelocity(), 0.0f, 0.002f) ){
        return true;
    }
    if (!isCloseto(servoRechts.getCurrentVelocity(), 0.0f, 0.002f) ){
        return true;
    }
    return false;
}

//##########################################################################################
// erhalte max. Beschleunigung der Servo
//##########################################################################################
//
// 
float getBeschleunigungHebemechanismus(){
    float lin = servoLinks.getMaxAcceleration();
    float rec = servoRechts.getMaxAcceleration();

    return (lin +rec)/2.0f;
}

//##########################################################################################
// Statusabfrage ob Servos aktiviert sind
//##########################################################################################
//
// return       bool                    true = Aktiviert
bool isEnabledHebemechanismus(){
    bool returnBool=false;

    if (servoLinks.isEnabled() && servoRechts.isEnabled()){
        returnBool = true;
        // printf("enabled\t");
    }
    return returnBool;
}

//##########################################################################################
// Statusabfrage ob Servos deaktiviert sind
//##########################################################################################
//
// return       bool                    true = Deaktiviert
bool isDisabledHebemechanismus(){
    bool returnBool=false;

    if (!servoLinks.isEnabled() && !servoRechts.isEnabled()){
        returnBool = true;
        // printf("disabled\t");
    }
    return returnBool;
}


bool isCloseto(float a, float b, float delta){
    delta = abs(delta);
    if( (a >= (b-delta) )  &&   (a <= (b+delta) ) ){
        return true;
    }else return false;
}