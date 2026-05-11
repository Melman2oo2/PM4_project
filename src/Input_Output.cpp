//Notes an self (Louis):
// -  LED leuchtet momentan während gesamter Scanzeit 



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






static bool ACTIVE_SCAN;
#define SIMULATED_SCAN_TIMER_TIME 200 // 10 Sekunden, 50Hz





#define LIGHT_SOLL 0.5f // Gewolltes Licht in Lux
#define LIGHT_TOL 0.1f // Erlaubte Abweichung in Lux

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

AnalogIn LightSensor(PB_A1);

AnalogIn TempSensor(PB_A0);



// Output

DigitalOut ScanLed(PB_4);


//##########################################################################################
// Startet den Scan
//##########################################################################################
//
// Gibt optisches Signal aus, wenn der Scan starten soll
void startScan(){
    ACTIVE_SCAN = 1;
    ScanLed = 1; 
    
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
        

    // Es gab nie einen Scan
    if(ever_has_been == 0 && ACTIVE_SCAN == 0){
        printf("kein Scan erkannt");
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
        ever_has_been = 0;
        printf("Scan beendet");
        return 1;
    }

    return 0;
    
}

// Cycle
//##########################################################################################
// Wiederholung Input_Output
//##########################################################################################
//
// Wiederholte Aufgufe (50Hz)
void cycle_InputOuput(){

    
   

     //Simulierten Timer für Scan
     static int SCAN_TIMER = 0;     
     if(ACTIVE_SCAN){
        SCAN_TIMER ++; 
        

        if(light_overdose()){
            printf("error, zu hell");
        }

        if(temperature_overdose()){
            printf("error, zu warm/kalt");
        }
        
    }else if (SCAN_TIMER == SIMULATED_SCAN_TIMER_TIME)
    {
        ACTIVE_SCAN = 0;
        ScanLed = 0;
        SCAN_TIMER = 0;
    }
    
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
    
    static float lux_values[20]; 
    static int lux_i = 0;
    static float lux_avg;

    lux_values[lux_i] = input_value; //Durchnitt Wert bestimmen
    if(lux_i < 19){
        lux_i ++;
    }else{
        lux_i = 0;   
    }

    
    for(int i = 0; i < 20; i++){
        lux_avg += lux_values[i];
    } 
    lux_avg /= 20;
    
    if(lux_avg < LIGHT_SOLL){

        return 0;
        
    }else{

        printf("Licht is nix gut\n");
        return 1;
        
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
    cel_avg /= 20;
    printf("%f\n", input_value);
    if(((TEMP_SOLL - TEMP_TOL) < cel_avg ) && (cel_avg < (TEMP_SOLL + TEMP_TOL))){
        
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