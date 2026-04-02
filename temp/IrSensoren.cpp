// Irsensoren.cpp
#include "mbed.h"

// pes board pin map
#include "PESBoardPinMap.h"

// drivers
#include "DebounceIn.h"
#include <cstdio>
#include "AvgFilter.h"

#define IR_FILTER_SAMPLES 20

const int irDebug = 0;

// Analogeingangsobjekte
AnalogIn SensorVorGewichtAnalogIn(PC_2); // A0
AnalogIn SensorHinterGewichtAnalogIn(PC_3); // A1
AnalogIn SensorDistanzNachVorneAnalogIn(PB_1); // A3

// Distanzfilterobjekte
AvgFilter sensorVorGewichtFilter(IR_FILTER_SAMPLES);
AvgFilter sensorHinterGewichtFilter(IR_FILTER_SAMPLES);
AvgFilter sensorDistanzNachVorneFilter(IR_FILTER_SAMPLES);

// Distanzmessvariablen
float irVorGewichtDistance_mV = 0.0f; // define a variable to store measurement (in mV)
float irHinterGewichtDistance_mV = 0.0f; // define a variable to store measurement (in mV)
float irDistanzDistance_mV = 0.0f; // define a variable to store measurement (in mV)


//#####################################################################################
// cyclic
//#####################################################################################
void cyclicIrSensoren() {
    
    // read analog input
    irVorGewichtDistance_mV = 1.0e3f * SensorVorGewichtAnalogIn.read() * 3.3f;
    irHinterGewichtDistance_mV = 1.0e3f * SensorHinterGewichtAnalogIn.read() * 3.3f;
    irDistanzDistance_mV = 1.0e3f * SensorDistanzNachVorneAnalogIn.read() * 3.3f;

    // Eingangswert filtern und speichern für Weiterverarbeitung
    irVorGewichtDistance_mV = sensorVorGewichtFilter.apply(irVorGewichtDistance_mV);
    irHinterGewichtDistance_mV = sensorHinterGewichtFilter.apply(irHinterGewichtDistance_mV);
    irDistanzDistance_mV = sensorDistanzNachVorneFilter.apply(irDistanzDistance_mV);

}


//#####################################################################################
// Distanz berechnen
//#####################################################################################
float irSensorDistanzErmitteln(float irDistance_mV, float a, float b)
{

    // avoid division by zero by adding a small value to the denominator
    if (irDistance_mV + b == 0.0f)
        irDistance_mV -= 0.001f;

    return a / (irDistance_mV + b); // distanze in cm
}


//#####################################################################################
// Sensor vor Gewicht: Boden erkannt
//#####################################################################################
bool sensorVorGewichtBodenErkannt() {
    // Parameter
    const float irParamA = 2.574e+04f;
    const float irParamB = -29.37f;
    const float schwellwertBodenerkennung = 18.0;

    // Distanzmessvariablen
    float irDistance_cm = 0.0f;

    // Diszanz berechnen
    irDistance_cm = irSensorDistanzErmitteln(irVorGewichtDistance_mV, irParamA, irParamB);

    if (irDebug == 1) {
    printf("VorGewicht: %04f", irDistance_cm);
    }

    // Distanz auswerten
    if (irDistance_cm < schwellwertBodenerkennung) {
        return true;
    } else {
        return false;
    }
}


//#####################################################################################
// Sensor hinter Gewicht: Boden erkannt
//#####################################################################################
bool sensorHinterGewichtBodenErkannt() {
    // Parameter
    const float irParamA = 2.574e+04f;
    const float irParamB = -29.37f;
    const float schwellwertBodenerkennung = 20.0;

    // Distanzmessvariablen
    float irDistance_cm = 0.0f;

    // Diszanz berechnen
    irDistance_cm = irSensorDistanzErmitteln(irHinterGewichtDistance_mV, irParamA, irParamB);
    
    if (irDebug == 1) {
    printf(" NachGewicht: %04f", irDistance_cm);
    }

    // Distanz auswerten
    if (irDistance_cm < schwellwertBodenerkennung) {
        return true;
    } else {
        return false;
    }
}


//#####################################################################################
// Sensor vor Gewicht: Boden erkannt
//#####################################################################################
float sensorDistanzNachVorneInCm() {
    // Parameter
    const float irParamA = 2.574e+04f;
    const float irParamB = -29.37f;

    // Distanzmessvariablen
    float irDistance_cm = 0.0f;

    // Diszanz berechnen
    irDistance_cm = irSensorDistanzErmitteln(irDistanzDistance_mV, irParamA, irParamB);


    if (irDebug == 1) {
    printf(" NachVorne: %04f\n", irDistance_cm);
    }

    // Distanz ausgeben
    return irDistance_cm;
}

