//=====================================================================================
// PM4 Main File
//=====================================================================================

#include "mbed.h"

// pes board pin map
#include "PESBoardPinMap.h"

// drivers
#include "DebounceIn.h"

// PM4-Projekt
#include "desedimentation.h"
#include "Input_Output.h"
#include "Scara.h"
#include "Vial_station.h"
#include <cstdio>

// standartwerte definieren für lesbarkeit

#define PRINTFACTIVE true


//#####################################################################################
// PM2 Globale Deklarationen
//#####################################################################################

Scara& getScara() {
    static Scara scara(PRINTFACTIVE);
    return scara;
}

// Tags für die Zustände
bool first_Vial = true;
bool kurzVorEnde = false;

// Globale Punkte
Point p_Desedimentation(-8.0f,128.8f, -43.5f);
Point p_idle(0.0f, 150.0f, -4.0f);
Point p_ENANTIOS(-3.0f,92.0f, -131.0f);
Point p_LidClose(-6.0f,93.0f, -77.3f);
Point p_LidOpen(177.0f,-17.0f, -73.0f);

Point p_test(-100.0f, 129.0f, -57.5f);

Point outputs[] = {
    Point(-86.0f, 73.0f, -57.5f),
    Point(-78.0f, 107.0f, -57.5f),
    Point(-69.5f, 136.0f, -57.5f),
    Point(-107.5f, 73.5f, -57.5f),
    Point(-102.8f, 103.0f, -57.5f),
    Point(-100.0f, 129.0f, -57.5f)
};


enum class PM4State {
    Start,
    Vial_from_Station,
    Vial_to_Desedimentation,
    Lid_from_ENANTIOS,
    Lid_to_holder,
    Vial_from_ENANTIOS,
    Vial_to_Output,
    Vial_from_Desedimentation,
    Vial_to_ENANTIOS,
    Lid_from_holder,
    Lid_to_ENANTIOS,
    Ende,
    ERROR,
    Zu_hell
};PM4State state = PM4State::Start;
PM4State error_from_state = PM4State::ERROR;
int output_counter = 0;


//#####################################################################################
// PM4 INIT-Funktion
//#####################################################################################
void pm4init(void) {

    // init der Module
    getScara();
    init_VialStation();
    init_desedimentation();
  

    return;
}


//#####################################################################################
// PM4 MAIN-Funktion
//#####################################################################################
void pm4main(void) {
    if(PRINTFACTIVE) printf("cycle begin\t");

    // if (PRINTFACTIVE) printf("Licht: %d\t", light_overdose());

    // cycle der Module
    getScara().cycle();
    cycle_desedimentation();
    cycle_InputOuput();

    // if(temperature_overdose()){
    //         error_from_state = PM4State::Zu_hell;
    //         state = PM4State::ERROR;
    // }


    // Swicht der State Machine
    switch (state)
    {
    case PM4State::Start:
        if(PRINTFACTIVE) printf("State: Start\t");
        // code for Start state


        if(getScara().isFinished() && hasNextVial_VialStation()) {
            getScara().grab(vialPresent_VialStation());
            state = PM4State::Vial_from_Station;
            // getScara().grab(p_test);
            // state = PM4State::Ende;
        }
        break;

    case PM4State::Vial_from_Station:
        if(PRINTFACTIVE) printf("State: Vial_from_Station\t");
        // code for Vial_from_Station state

        if(getScara().isFinished()) {
            getScara().place(p_Desedimentation);
            state = PM4State::Vial_to_Desedimentation;
        }
        break;

    case PM4State::Vial_to_Desedimentation:
        if(PRINTFACTIVE) printf("State: Vial_to_Desedimentation\t");
        // code for Vial_to_Desedimentation state

        if(getScara().isFinished()) {
            start_desedimentation();
            // wenn nicht erstes vial, warten bis scan fertig, sonst direkt weiter
            printf("first vial: %d\t", first_Vial);
            printf("scan finished: %d\t", isScanFinished());
            if(!first_Vial && !isScanFinished()) break;
            getScara().grab(p_LidClose);
            state = PM4State::Lid_from_ENANTIOS;
        }
        break;

    case PM4State::Lid_from_ENANTIOS:
        if(PRINTFACTIVE) printf("State: Lid_from_ENANTIOS\t");
        // code for Lid_from_ENANTIOS state

        if(getScara().isFinished()) {
            getScara().place(p_LidOpen);
            state = PM4State::Lid_to_holder;
        }
        break;

    case PM4State::Lid_to_holder:
        if(PRINTFACTIVE) printf("State: Lid_to_holder\t");
        // code for Place_Lid state

        if(getScara().isFinished()) {
            
            // unterscheifung, first Vial oder nicht
            if(first_Vial) {
                if(isVialOben_desedimentation() && isTimerDone_desedimentation()) {
                first_Vial = false;
                getScara().grab(p_Desedimentation);
                state = PM4State::Vial_from_Desedimentation;
                }

            } else {
                getScara().grab(p_ENANTIOS);
                state = PM4State::Vial_from_ENANTIOS;
            }


        }
        break;

    case PM4State::Vial_from_ENANTIOS:
        if(PRINTFACTIVE) printf("State: Vial_from_ENANTIOS\t");
        // code for Vial_from_ENANTIOS state

        if(getScara().isFinished()) {
            // error, wenn kein output platz mehr da
            if(output_counter >= 6) {
                error_from_state = PM4State::Vial_from_ENANTIOS;
                state = PM4State::ERROR;
            } else {
                getScara().place(outputs[output_counter]);
                output_counter++;
                state = PM4State::Vial_to_Output;
            }
        }
        break;

    case PM4State::Vial_to_Output:
        if(PRINTFACTIVE) printf("State: Vial_to_Output\t");
        // code for Vial_to_Output state

        if(getScara().isFinished()) {
            
            // unterscheifung, last Vial oder nicht
            if(kurzVorEnde) {
                getScara().grab(p_LidOpen);
                state = PM4State::Lid_from_holder;
            } else {
                getScara().grab(p_Desedimentation);
                state = PM4State::Vial_from_Desedimentation;
            }
        }
        break;

    case PM4State::Vial_from_Desedimentation:
        if(PRINTFACTIVE) printf("State: Vial_from_Desedimentation\t");
        // code for Vial_from_Desedimentation state

        if(getScara().isFinished()) {
            getScara().place(p_ENANTIOS);
            state = PM4State::Vial_to_ENANTIOS;

        }
        break;

    case PM4State::Vial_to_ENANTIOS:
        if(PRINTFACTIVE) printf("State: Vial_to_ENANTIOS\t");
        // code for Vial_to_ENANTIOS state

        if(getScara().isFinished()) {
            getScara().grab(p_LidOpen);
            state = PM4State::Lid_from_holder;

        }
        break;

    case PM4State::Lid_from_holder:
        if(PRINTFACTIVE) printf("State: Lid_from_holder\t");
        // code for Lid_from_holder state

        if(getScara().isFinished()) {
            getScara().place(p_LidClose);
            state = PM4State::Lid_to_ENANTIOS;

        }
        break;

    case PM4State::Lid_to_ENANTIOS:
        if(PRINTFACTIVE) printf("State: Lid_to_ENANTIOS\t");
        // code for Lid_to_ENANTIOS state

        if(getScara().isFinished()) {
            if(kurzVorEnde) {
                getScara().target = p_idle;
                state = PM4State::Ende;
            } else {
                if(light_overdose()){
                    error_from_state = PM4State::Lid_to_ENANTIOS;
                    state = PM4State::ERROR;
                    break;
                }
                
                if(isScanFinished()){
                    if(!hasNextVial_VialStation()) {
                    kurzVorEnde = true;
                    getScara().grab(p_LidClose);
                    state = PM4State::Lid_from_ENANTIOS;
                }else{
                    getScara().grab(vialPresent_VialStation());
                    state = PM4State::Vial_from_Station;
                }
                }else startScan();
            }
            
        }
        break;

    case PM4State::Ende:
        if(PRINTFACTIVE) printf("State: Ende\t");
        // code for Ende state

        break;

    case PM4State::ERROR: //ERROR unc default
    default:
        if(PRINTFACTIVE) printf("State: ERROR\t");
        getScara().stop();
        // code for ERROR state
        printf("Error in state: \t");
        switch (error_from_state) {
            case PM4State::Vial_from_ENANTIOS:
                printf("No more output places\t");
                break;
            case PM4State::Lid_to_ENANTIOS:
                printf("Licht zu hell\t");
                break;
            case PM4State::Zu_hell:
                printf("zu Warm\t");
                break;
            default:
                printf("Unknown\t");
                break;
        }
        break;
    }


    if(PRINTFACTIVE) printf("cycle end\n");
    return;
}


//#####################################################################################
// PM4 RESET-Funktion
//#####################################################################################
void pm4reset(void) {

    reset_desedimentation();

    if (PRINTFACTIVE) printf("* * * RESET * * *");
    
    getScara().reset();

    return;
}
