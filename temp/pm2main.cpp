//=====================================================================================
// PM2 Main File
//=====================================================================================

#include "mbed.h"

// pes board pin map
#include "PESBoardPinMap.h"

// drivers
#include "DebounceIn.h"

// PM2-Projekt
#include "Fahrwerk.h"
#include "Hebemechanismus.h"
#include "IrSensoren.h"
#include <cstdio>

// standartwerte definieren für lesbarkeit
#define GERADEAUS 0.0f
#define NORMAL_GESCH 1.0f

#define PRINTFACTIVE true


//#####################################################################################
// PM2 Globale Deklarationen
//#####################################################################################

// Ablaufschritte
typedef enum {
    Initialieren,
    LinieFolgen,
    SeilBefahren,
    GewichtAbsenken1,
    Einpendeln1,
    FahreAufSeil1,
    Langsamzurueck1,
    GewichtAnheben1,
    BalkenUberqueren,
    GewichtAbsenken2,
    Einpendeln2,
    FahreAufSeil2,
    Langsamzurueck2,
    GewichtAnheben2,
    FahreAufZielplattform,
    Ziel,
    SchlusssequenzInit,
    SchlusssequenzAnnaehern,
    SchlusssequenzFahreVorWand,
    SchlusssequenzFahreInWand,
    SchlusssequenzSetzeZurueck,
    Ende,
    Fehler
} t_RoboterStatus;


// Schritt des Roboterablaufes 
t_RoboterStatus status;


// Challenge-Typ
typedef enum {
    Minimalchallange,
    Starchallange,
} t_challenge;

// aktuelle Challenge
t_challenge challenge;

// Pin festlegen zum Challenge-Typ schalten
DigitalIn challengeSwitch(PC_5);

//bool zum status über Balken speichern
bool ueberBalken = false;

// int wart zum warten
int wart = 0;

int schlusssequenzCounter = 1;
#define SCHLUSSEQUENZ_ANZAHL 3

//#####################################################################################
// PM2 INIT-Funktion
//#####################################################################################
void pm2init(void) {

    // init der Module
    initFahrwerk();
    initHebemechanismus();
    // initIrSensoren();

    status = Initialieren;

    // mode challenge switch
    challengeSwitch.mode(PullUp);

    return;
}


//#####################################################################################
// PM2 MAIN-Funktion
//#####################################################################################
void pm2main(void) {

    // cycle Funktionen aufrufen
    cycleFahrwerk();
    cycleHebemechanismus();
    cyclicIrSensoren();

    // Challenge enum korrekt setzen
    if (challengeSwitch.read()){
        challenge = Starchallange;
    } else {
        challenge = Minimalchallange;
    }
    
    
    //=================================================================================
    // SCHRITTKETTE ROBOTERABLAUF
    //=================================================================================
    switch (status) {
        //-----------------------------------------------------------------------------
        // Initialisierung
        //-----------------------------------------------------------------------------
        case Initialieren:
            // init der Module im pm2init
            
            // motoren Raupe enable
            if (isDisabledMotoren()){
                enableMotoren();
                //enableMotionPlannerFahrwerk();
                //setBeschleunigungFahrwerk(1.0f);
            }

            if (isDisabledHebemechanismus()){
                enableHebemechanismus();
                setBeschleunigungHebemechanismus(1.0f);
            }
            GewichtAnheben();

            // bool zum status über Balken setzten
            ueberBalken = false;

            wart = 0;
            
            // Transistion
            // direkt weitergehen
            if (GewichtOben()) {
                status = LinieFolgen;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt LinieFolgen **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Linie folgen
        //-----------------------------------------------------------------------------
        case LinieFolgen:
            // Linefollower
            FolgeDerLinie();
            

            // Transistion
            // 
            if (sensorVorGewichtBodenErkannt() == false) {
                status = SeilBefahren;
                
                // motion Planner aktivieren
                enableMotionPlannerFahrwerk();
                setBeschleunigungFahrwerk(1.0f);

                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt SeilBefahren **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Seil Befahren
        //-----------------------------------------------------------------------------
        case SeilBefahren:
            // sehr langsames fahren bis halb auf Plattform
            setGeschwindigkeit(NORMAL_GESCH/2.0f, GERADEAUS);
            
            // Transistion
                // 
            if (sensorHinterGewichtBodenErkannt() == false) {
                stopFahrwerk();
                
                // Unterscheide Challangetyp
                if (challenge == Minimalchallange) {
                    // Balkensequenz überspringen
                    status = GewichtAbsenken2;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt GewichtAbsenken2 **********\n");
                } else if (challenge == Starchallange) {
                    // Balkensequenz nicht überspringen
                    status = GewichtAbsenken1;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt GewichtAbsenken1 **********\n");
                }
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Gewicht Absenken 1
        //-----------------------------------------------------------------------------
        case GewichtAbsenken1:
            // Gewicht absenken, bis unten
            GewichtAbsenken();

            // Transistion
            // Gewicht unten
            if (GewichtUnten()) {
                status = Einpendeln1;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt Einpendeln1 **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Einpendeln 1 (kurz warten bis Fahrzeug ruht)
        //-----------------------------------------------------------------------------
        case Einpendeln1:
            // 2s warten
            wart ++;

            // Transistion
            // 2s abgelaufen
            if (wart >= 100) {
                wart = 0;
                status = FahreAufSeil1;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt FahreAufSeil1 **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Fahre auf Seil 1
        //-----------------------------------------------------------------------------
        case FahreAufSeil1:
            // fahren bis balken
            setGeschwindigkeit(NORMAL_GESCH, GERADEAUS);

            // Transistion
            // Boden vor Gewicht
            if (sensorVorGewichtBodenErkannt()) {
                stopFahrwerk();
                status = Langsamzurueck1;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt Langsamzurueck1 **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Platz machen für Gewicht 1
        //-----------------------------------------------------------------------------
        case Langsamzurueck1:
            // Zurück, bis Balken wieder weg (einklemmschutz)
            setGeschwindigkeit(-(NORMAL_GESCH/2.0f), GERADEAUS);

            // Transistion
            // Weg für Gewicht frei
            if (sensorVorGewichtBodenErkannt() == false) {
                stopFahrwerk();
                status = GewichtAnheben1;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt GewichtAnheben1 **********\n");
            }
            break;
          
        //-----------------------------------------------------------------------------
        // Gewicht Anheben 1
        //-----------------------------------------------------------------------------
        case GewichtAnheben1:
            // Gewicht anheben bis oben
            GewichtAnheben();

            // Transistion
            // Gewicht oben
            if (GewichtOben()) {
                status = BalkenUberqueren;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt BalkenUberqueren **********\n");
            }
            break;

 
        //-----------------------------------------------------------------------------
        // Balken ueberqueren
        //-----------------------------------------------------------------------------
        case BalkenUberqueren:
            // Fahren bis halb über Balken

            // langsames Fahren am Ende
            if (ueberBalken == false){
                setGeschwindigkeit(NORMAL_GESCH, GERADEAUS);
            } else setGeschwindigkeit(NORMAL_GESCH/2.0f, GERADEAUS);
            

            // test, balken überquert
            if (sensorHinterGewichtBodenErkannt()){
                ueberBalken = true;
            }

            if (PRINTFACTIVE) printf("überbalken:\t%d\n", ueberBalken);
            // Transistion
            // 
            if ( (sensorHinterGewichtBodenErkannt() == false) && ueberBalken) {
                stopFahrwerk();
                status = GewichtAbsenken2;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt GewichtAbsenken2 **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Gewicht Absenken 2
        //-----------------------------------------------------------------------------
        case GewichtAbsenken2:
            // Gewicht absenken, bis unten
            GewichtAbsenken();

            // Transistion
            // Gewicht unten
            if (GewichtUnten()) {
                status = Einpendeln2;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt Einpendeln2 **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Einpendeln 2
        //-----------------------------------------------------------------------------
        case Einpendeln2:
            // 2s
            wart ++;

            // Transistion
            // 2s abgelaufen
            if (wart >= 100) {
                wart = 0;
                status = FahreAufSeil2;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt FahreAufSeil2 **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // FahreAufSeil 2
        //-----------------------------------------------------------------------------
        case FahreAufSeil2:
            // fahren bis Zielplattform
            setGeschwindigkeit(NORMAL_GESCH, GERADEAUS);

            // Transistion
            // Boden vor Gewicht
            if (sensorVorGewichtBodenErkannt()) {
                stopFahrwerk();
                status = Langsamzurueck2;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt Langsamzurueck2 **********\n");
            }
            break;
 
        //-----------------------------------------------------------------------------
        // Platz machen für Gewicht 2
        //-----------------------------------------------------------------------------
        case Langsamzurueck2:
            // Zurück, bis Balken wieder weg (einklemmschutz)
            setGeschwindigkeit(-(NORMAL_GESCH/2.0f), GERADEAUS);

            // Transistion
            // Weg für Gewicht frei
            if (sensorVorGewichtBodenErkannt() == false) {
                stopFahrwerk();
                status = GewichtAnheben2;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt GewichtAnheben2 **********\n");
            }
            break;
             
        //-----------------------------------------------------------------------------
        // Gewicht Anheben 2
        //-----------------------------------------------------------------------------
        case GewichtAnheben2:
            // Gewicht anheben bis oben
            GewichtAnheben();

            // Transistion
            // Gewicht oben
            if (GewichtOben()) {
                status = FahreAufZielplattform;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt FahreAufZielplattform **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Fahre auf Zielplattform
        //-----------------------------------------------------------------------------
        case FahreAufZielplattform:
            // Fahren bis sensor hinten auf Plattform
            setGeschwindigkeit(NORMAL_GESCH, GERADEAUS);
            

            // Transistion
            // Boden hinter Gewicht
            if (sensorHinterGewichtBodenErkannt()) {
                stopFahrwerk();
                status = Ziel;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt Ziel **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Ziel
        //-----------------------------------------------------------------------------
        case Ziel:
            // Fahren bis dist. zur Messwand < 20cm
            setGeschwindigkeit(NORMAL_GESCH, GERADEAUS);

            // Transistion
            // 20 cm vor Wand
            if (sensorDistanzNachVorneInCm() <= 20.0f) {
                stopFahrwerk();
                wart = 0;
                status = SchlusssequenzInit;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt SchlusssequenzInit **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Schlusssequenz Init
        //-----------------------------------------------------------------------------
        case SchlusssequenzInit:
            // warte 5 Sekunden
            wart ++;

            // Transistion
            // 5 Sekunden
            if (wart >= 100) {
                wart = 0;
                status = SchlusssequenzAnnaehern;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt SchlusssequenzAnnaehern **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Schlusssequenz Annaehern
        //-----------------------------------------------------------------------------
        case SchlusssequenzAnnaehern:
            // fahre in den Nahbereich des Sensors
            setGeschwindigkeit(NORMAL_GESCH * 5.0f, GERADEAUS);

            // Transistion
            // fahre bis Sensor in den Nahbereich wechselt
            if (sensorDistanzNachVorneInCm() < 11.5) {
                status = SchlusssequenzFahreVorWand;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt SchlusssequenzFahreVorWand **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Schlusssequenz fahre vor Wand
        //-----------------------------------------------------------------------------
        case SchlusssequenzFahreVorWand:
            // fahre bis kurz vor der Wand
            setGeschwindigkeit(NORMAL_GESCH * 5.0f, GERADEAUS);

            // Transistion
            // knapp vor der wand
            if (sensorDistanzNachVorneInCm() > 14.0) {
                wart = 0;
                status = SchlusssequenzFahreInWand;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt SchlusssequenzFahreInWand **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Schlusssequenz fahre in Wand
        //-----------------------------------------------------------------------------
        case SchlusssequenzFahreInWand:
            // fahre noch 0.5 s
            wart++;
            setGeschwindigkeit(NORMAL_GESCH * 5.0f, GERADEAUS);

            // Transistion
            // 
            if ((wart >= 25) && (schlusssequenzCounter < SCHLUSSEQUENZ_ANZAHL)) {
                // wiederholen
                wart = 0;
                schlusssequenzCounter++;
                status = SchlusssequenzSetzeZurueck;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt SchlusssequenzSetzeZurueck **********\n");

            } else if ((wart >= 15) && (schlusssequenzCounter >= SCHLUSSEQUENZ_ANZAHL)) {
                // Ende
                wart = 0;
                stopFahrwerk();
                status = Ende;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt Ende **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Schlusssequenz setze zurueck
        //-----------------------------------------------------------------------------
        case SchlusssequenzSetzeZurueck:
            // fahre zurück
            setGeschwindigkeit(NORMAL_GESCH * -5.0f, GERADEAUS);

            // Transistion
            // bis genug Abstand
            if (sensorDistanzNachVorneInCm() < 12.0) {
                status = SchlusssequenzFahreVorWand;
                if (PRINTFACTIVE) printf("********** Wechsle zu Schritt SchlusssequenzFahreVorWand **********\n");
            }
            break;
            
        //-----------------------------------------------------------------------------
        // Ende, fertig
        //-----------------------------------------------------------------------------
        case Ende:
            // anable all

            if (isEnabledMotoren()){
                disableMotionPlannerFahrwerk();
                disableMotoren();
            }

            /* weglassen, damit die Gewichte keinen Seich machen
            if (isEnabledHebemechanismus()){
                disableMotionPlannerFahrwerk();
            }
            */

            break;
            
            
        //-----------------------------------------------------------------------------
        // Fehlerstatus
        //-----------------------------------------------------------------------------
        case Fehler:
            // 
            
                printf("********** Fehler **********\n");

            break;
            
        //-----------------------------------------------------------------------------
        // DEFAULT
        //-----------------------------------------------------------------------------
        default: 
            // gehe zu Fehler
            status = Fehler;
            break;
    }
    //=================================================================================
    // ENDE DES ROBOTERABLAUF
    //=================================================================================
    


    return;
}


//#####################################################################################
// PM2 RESET-Funktion
//#####################################################################################
void pm2reset(void) {

    stopFahrwerk();
    GewichtAnheben();
    ueberBalken = false;
    status = Initialieren;
    wart = 0; 
    if (PRINTFACTIVE) printf("* * * RESET * * *");
    return;
}
