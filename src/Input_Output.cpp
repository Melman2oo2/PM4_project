//Notes an self (Louis):
// -  CycleInputOutput muss noch gemacht werden, aber sehe sinn dahinter nicht.
// -  Bei StartScan muss no LED implementiert werden und allenfalls Simulierter  Timer mit Schalter ersetzt werden.   



// Input_Output.cpp

#include "mbed.h"

// pes board pin map
#include "PESBoardPinMap.h"

// drivers
#include "Input_Output.h"
#include "Servo.h"



//##########################################################################################
// Globale Variable
//##########################################################################################

bool ACTIVE_SCAN;

int SIMULATED_TIMER; 

#define LIGHT_MAX 100.0f // Max Licht in Lux
#define LIGHT_MIN 0.0f  // Min Licht in Lux

#define LIGHT_SOLL 0.0f // Gewolltes Licht in Lux
#define LIGHT_TOL 2.0f // Erlaubte Abweichung in Lux

#define TEMP_MAX 100.0f     // Max Temperatur in Grad Celsius        
#define TEMP_MIN 0.0f    // Min Temperatur in Grad Celsius    

#define TEMP_SOLL 25.0f // Gewollte Temperatur in Celsius
#define TEMP_TOL 2.0f // Erlaubte Abweichung in Celsius

//##########################################################################################
// Globale Objekte
//##########################################################################################
//
//..........................................................................................
// Schalter
//..........................................................................................


//..........................................................................................
// Sensoren
//..........................................................................................

AnalogIn LightSensor(PB_SENS_LIGHT);

AnalogIn TempSensor(PB_SENS_TEMP);



// Output
//##########################################################################################
// Startet den Scan
//##########################################################################################
//
// Gibt optisches Signal aus, wenn der Scan starten soll
void startScan(){
    ACTIVE_SCAN = 1;
    SIMULATED_TIMER = 500; //10sekunden Timer
    return;
}

// Input
//##########################################################################################
// Statusabfrage, ob Scan vertig ist
//##########################################################################################
//
// return       bool                true = scan ist fertig
bool isScanFinished(){
    static bool ever_has_been = 0;
    
    //Simulierten Timer runterlaufen lassen
    if(ACTIVE_SCAN){
        SIMULATED_TIMER -= 1;
    }else if (SIMULATED_TIMER = 0)
    {
       ACTIVE_SCAN = 0;
    }
    
    

    // Es gab nie einen Scan
    if(ever_has_been == 0 && ACTIVE_SCAN == 0){
        printf("Von Anfang an kein Scan");
        return 1;
    } 
    
    // Es gibt einen Scan zu beginn
    if(ever_has_been == 0 && ACTIVE_SCAN == 1){
        ever_has_been = 1;
        printf("Scan erkannt");
        return 0;
    }
    
    // Scan läuft
    if(ever_has_been == 1 && ACTIVE_SCAN == 1){
        
        printf("Scan im Gange");
        return 0;
    }

    // Scan beendet
    if(ever_has_been == 1 && ACTIVE_SCAN == 0){
        
        printf("Scan beendet");
        return 1;
    }
    
}

// Cycle
//##########################################################################################
// Wiederholung Input_Output
//##########################################################################################
//
// Wiederholte Aufgufe (50Hz)
void cycle_InputOuput(){

    return;
}

//##########################################################################################
// Abfrage Licht Überschritten
//##########################################################################################
//
// Wiederholte Aufgabe 1x pro main-schleife
// true = Licht ist schlecht

bool light_overdose(){
    float input_value = LightSensor.read(); //input Licht 0...1.0f 
    float lux_value = input_value * (LIGHT_MAX - LIGHT_MIN) + LIGHT_MIN; //input Licht in Lux

    static float lux_values[20]; 
    static int lux_i = 0;

    lux_values[lux_i] = lux_value; //Durchnitt Wert bestimmen
    if(lux_i < 19){
        lux_i ++;
    }else{
        lux_i = 0;   
    }

    float lux_avg = 0.0f;
    for(int i = 0; i < 20; i++){
        lux_avg += lux_values[i];
    } 
    
    if((LIGHT_SOLL - LIGHT_TOL) < lux_avg < (LIGHT_SOLL + LIGHT_TOL)){
        return 0;
    }else{
        return 1;
        printf("Licht is nix gut");
    } 
}


//##########################################################################################
// Abfrage Temperatur Überschritten
//##########################################################################################
//
// Wiederholte Aufgabe 1x pro main-schleife
// true = Temperatur ist schlecht

bool temperature_overdose(){
    float input_value = TempSensor.read(); //input Temperatur 0...1.0f 
    float cel_value = input_value * (TEMP_MAX - TEMP_MIN) + TEMP_MIN; //input Temperatur in Celsius

    static float cel_values[20]; 
    static int cel_i = 0;

    cel_values[cel_i] = cel_value; //Durchnitt Wert bestimmen
    if(cel_i < 19){
        cel_i ++;
    }else{
        cel_i = 0;   
    }

    float cel_avg = 0.0f;
    for(int i = 0; i < 20; i++){
        cel_avg += cel_values[i];
    } 
    
    if((TEMP_SOLL - TEMP_TOL) < cel_avg < (TEMP_SOLL + TEMP_TOL)){
        return 0;
    }else{
        return 1;
        printf("Temperatur is nix gut");
    } 

    
}



//##########################################################################################
// Initialisierung Input_Output
//##########################################################################################
//
// Einmaliges aufrufen am Anfang
void init_InputOutput(){

    return;
}