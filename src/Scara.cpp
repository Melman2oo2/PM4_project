// Scara.cpp

#include "mbed.h"

// pes board pin map
#include "PESBoardPinMap.h"

// Treiber
#include "FastPWM.h"
#include "DCMotor.h"
#include "Servo.h"
#include "Scara.h"
#include "DebounceIn.h"
#include "Point.h"


//##########################################################################################
// Globale Variable
//##########################################################################################
#define SCARA_FIRST_MAX 1.0f
#define SCARA_FIRST_MIN 0.0f

#define SCARA_SECOND_MAX 0.0f
#define SCARA_SECOND_MIN 1.0f

#define SCARA_GRIPPER_MAX 0.0f
#define SCARA_GRIPPER_MIN 1.0f

#define DELTA 0.001f

#define LENGTH 20.0f

#define GEWINDESTEIGUNG 8.0f


//##########################################################################################
// Globale Objekte
//##########################################################################################
//
//..........................................................................................
// Punkte
//..........................................................................................

Point scara_target(0,0,LENGTH, 0);
Point scara_curr(0,0,0);

//..........................................................................................
// Servo
//..........................................................................................
// servo-objekt erstellen
Servo servoFirst(PB_D0);
Servo servoSecond(PB_D1);
Servo servoGripper(PB_D2);

bool doServoStop = false;


//..........................................................................................
// Motoren
//..........................................................................................
// Initialisierung diverser Parameter
const float maximaleSpannung = 12.0f;
const float uebersetzungsverhaeltnis = 488.28125f;
const float Motorkonstante = 28.0f / 12.0f;
// Enable Power
DigitalOut enable_motors(PB_ENABLE_DCMOTORS);
// Objekt ertellen
DCMotor z_motor(PB_PWM_M1, PB_ENC_A_M1, PB_ENC_B_M1, uebersetzungsverhaeltnis, Motorkonstante, maximaleSpannung);

bool doDCStop = false;


//##########################################################################################
// Initalisierung Scara
//##########################################################################################
//
// Einmaliges Aufrufen am Anfang
void init_Scara(){
    
    // min und max PWM first
    float minPWMfirst = 0.011f;
    float maxPWMfirst = 0.110f;

    // min und max PWM second
    float minPWMsecond = 0.011f;
    float maxPWMsecond = 0.115f;

    // min und max PWM gripper
    float minPWMgripper = 0.011f;
    float maxPWMgripper = 0.115f;

    // PWM-Werte übergeben
    servoFirst.calibratePulseMinMax(minPWMfirst, maxPWMfirst);
    servoSecond.calibratePulseMinMax(minPWMsecond, maxPWMsecond);
    servoGripper.calibratePulseMinMax(minPWMgripper, maxPWMgripper);

    return;
}

//##########################################################################################
// Wiederholung Scara
//##########################################################################################
//
// Wiederhohlte Aufrufe (50Hz)
void cycle_Scara(){
    if (doServoStop){
        stop_Scara_x_y();
    }
        if (doDCStop){
        stop_Scara_z();
    }

    updatescara_target();
    updatescara_curr();

    return;
}

// Befehle
//##########################################################################################
// Scara geht zu dieser Position
//##########################################################################################
//
// Scara Ziel: Kartesische Position
bool updatescara_target(){
    Pscara pscara = scara_target.getScara(LENGTH);
    if (pscara.impossible) return false;
    servoFirst.setPulseWidth(pscara.theta_1);
    servoSecond.setPulseWidth(pscara.theta_2);

    z_motor.setRotation(pscara.z / GEWINDESTEIGUNG);

    return true;
}

//##########################################################################################
// greifft das Vial
//##########################################################################################
//
// greifft die Vial basierend auf der höhe des Vials
void grabVial_Scara(uint32_t height){

    return;
}

//##########################################################################################
// platziert das Vial
//##########################################################################################
//
// platziert das Vial basierend auf der Zielhöhe des Vials
void placeVial_Scara(uint32_t height){

    return;
}

//##########################################################################################
// setzt die max. Beschleunigung
//##########################################################################################
//
// setzt die max. Servo und DC Beschleunigung
void setAcceleration_Scara(uint32_t acceleration){
    
    servoFirst.setMaxAcceleration(acceleration/2);
    servoSecond.setMaxAcceleration(acceleration);
    servoGripper.setMaxAcceleration(acceleration);

    z_motor.setMaxAcceleration(acceleration*2);

    return;
}

//##########################################################################################
// setzt die max. Geschwindigkeit
//##########################################################################################
//
// setzt die max. Servo und DC Geschwindigkeit
void setSpeed_Scara(uint32_t speed){

    servoFirst.setMaxVelocity(speed/2);
    servoSecond.setMaxVelocity(speed);
    servoGripper.setMaxVelocity(speed);

    z_motor.setMaxVelocity(speed*2);

    return;
}

//##########################################################################################
// Stoppt den Scara
//##########################################################################################
//
// Stoppt alle Motoren des Scaras
void stop_Scara(){

    stop_Scara_x_y();
    stop_Scara_z();

    return;
}

//##########################################################################################
// Stoppt den Scara in der X und Y Achse
//##########################################################################################
//
// Stoppt alle SERVOS des Scaras
void stop_Scara_x_y(){
    doServoStop = true;
    servoFirst.setPulseWidth(servoFirst.getCurrentPulseWidth());
    servoSecond.setPulseWidth(servoSecond.getCurrentPulseWidth());

    if (!isMoving_Scara_x_y()){
        doServoStop = false;
    }
    
    return;
}

//##########################################################################################
// Stoppt den Scara in der Z Achse
//##########################################################################################
//
// Stoppt alle Motoren des Scaras
void stop_Scara_z(){

    doDCStop = true;
    z_motor.setVelocity(0.0f);

    if (!isMoving_Scara_z()){
        doDCStop = false;
    }
    

    return;
}

//##########################################################################################
// enabled den Scara
//##########################################################################################
//
// enabled die Motoren des Scaras
void enable_Scara(){
    // DC
    enable_motors = 1;

    // Servo
    if (!servoFirst.isEnabled())
        servoFirst.enable();
    if (!servoSecond.isEnabled())
        servoSecond.enable();

    return;
}

//##########################################################################################
// disabled den Scara
//##########################################################################################
//
// disabled die Motoren des Scaras
void disable_Scara(){
    // DC
    enable_motors = 0;

    // Servo
    if (servoFirst.isEnabled())
        servoFirst.disable();
    if (servoSecond.isEnabled())
        servoSecond.disable();

    return;
}

//##########################################################################################
// enabled den Motion Planer
//##########################################################################################
//
// enabled den Motion Planer der z-Achse
void enableMotionPlaner_Scara_z(){
    z_motor.enableMotionPlanner();
    return;
}

//##########################################################################################
// disabled den Motion Planer
//##########################################################################################
//
// disabled den Motion Planer der z-Achse
void disableMotionPlaner_Scara_z(){
    z_motor.disableMotionPlanner();
    return;
}

// Rückmeldungen

//##########################################################################################
// Statusabfrage, ob alle Motoren enabled
//##########################################################################################
//
// return       bool        true = alle Motoren Enabled
bool isEnabled_Scara(){

    if(!servoFirst.isEnabled()) return false;
    if(!servoSecond.isEnabled()) return false;
    if(!servoGripper.isEnabled()) return false;

    if(!enable_motors.read()) return false;

    return true;
}

//##########################################################################################
// Statusabfrage, ob alle Motoren disabled
//##########################################################################################
//
// return       bool        true = alle Motoren disabled
bool isdiabled_Scara(){

    if(servoFirst.isEnabled()) return false;
    if(servoSecond.isEnabled()) return false;
    if(servoGripper.isEnabled()) return false;

    if(enable_motors.read()) return false;

    return true;
}

//##########################################################################################
// Erhalde die aktuelle Geschwindigkeit
//##########################################################################################
//
// return       uint32_t    akt. Geschwindigkeit
uint32_t actualSpeed_Scara(){

    return 0;
}

//##########################################################################################
// zustandsabfrage, ob bewegung passiert
//##########################################################################################
//
// return       bool        true = Scara bewegt sich
bool isMoving_Scara(){

    if (isMoving_Scara_x_y()) return true;

    if (isMoving_Scara_z()) return true;

    return false;
}

//##########################################################################################
// zustandsabfrage, ob bewegung in X oder Y Achse passiert
//##########################################################################################
//
// return       bool        true = Scara bewegt sich in X oder Y Ebene
bool isMoving_Scara_x_y(){

    if (!isCloseto_Scara(servoFirst.getCurrentVelocity(), 0.0f, DELTA) ){
        return true;
    }
    if (!isCloseto_Scara(servoSecond.getCurrentVelocity(), 0.0f, DELTA) ){
        return true;
    }

    return false;;
}

//##########################################################################################
// zustandsabfrage, ob bewegung in Z Achse passiert
//##########################################################################################
//
// return       bool        true = Scara bewegt sich in Z Ebene
bool isMoving_Scara_z(){

    if (!isCloseto_Scara(z_motor.getVelocity(), 0.0f, DELTA) ){
        return true;
    }

    return false;
}

//##########################################################################################
// Erhalte die Max. Beschleunigung
//##########################################################################################
//
// return       uint32_t    max. Beschleunigung
uint32_t getAcceleration_Scara(){

    if(isCloseto_Scara(servoFirst.getMaxAcceleration()*2, servoSecond.getMaxAcceleration(), DELTA)){
        return servoSecond.getMaxAcceleration();
    }

    return -1;
}

//##########################################################################################
// Statusabfrage, ob die Position erreich ist
//##########################################################################################
//
// return       bool            true = position erreicht
bool positionErreich_Scara(){

    Cartesian target = scara_target.getCartesian();
    Cartesian curr = scara_curr.getCartesian();

    if(!isCloseto_Scara(target.x, curr.x, DELTA)) return false;
    if(!isCloseto_Scara(target.y, curr.y, DELTA)) return false;
    if(!isCloseto_Scara(target.z, curr.z, DELTA)) return false;

    return true;
}

//##########################################################################################
// Erhalte die Aktuelle Position einer achse
//##########################################################################################
//
// return       Point        aktuelle Position
void updatescara_curr(){

    scara_curr.setScara(servoFirst.getCurrentPulseWidth(), servoSecond.getCurrentPulseWidth(), LENGTH, z_motor.getRotation()*GEWINDESTEIGUNG);
    
    return;
}



//##########################################################################################
// abfragen, ob zwei double nache sind
//##########################################################################################
//
// return       bool        true = nache beieinander
bool isCloseto_Scara(double a, double b, float delta){
    delta = abs(delta);
    if( (a >= (b-delta) )  &&   (a <= (b+delta) ) ){
        return true;
    }else return false;
}