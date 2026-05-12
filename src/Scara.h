// Scara.h
#ifndef SCARA_H
#define SCARA_H

#include "mbed.h"
#include "PESBoardPinMap.h"
#include "FastPWM.h"
#include "DCMotor.h"
#include "Servo.h"
#include "Point.h"
#include "DebounceIn.h"

class Scara {
public:
    Point target;
    Point current;

    bool doStop;


private:
    bool printActive;
    bool doeshomeing = false;

    static constexpr float DELTA = 0.001f;
    static constexpr float LENGTH = 20.0f;
    static constexpr float GEWINDESTEIGUNG = -8.0f;

    static constexpr float OFFEN = 0.9f;
    static constexpr float GESCHLOSSEN = 0.0f;

    static constexpr float maximaleSpannung = 12.0f;
    static constexpr float uebersetzungsverhaeltnisZ = 31.0f;
    static constexpr float uebersetzungsverhaeltnisFirst = 488.0f;
    static constexpr float motorkonstante = 180.0f / 12.0f;

    volatile bool zAnschlag = false;
    volatile bool firstAnschlag = false;
    
    Point grabPlacePoint;

    enum class Status {
        InitialisierenZ,
        InitialisierenSecond,
        InitialisierenFirst,
        InitialisierenWeg,
        VialGreifen0,
        VialGreifenXY,
        VialGreifenZ,
        VialPositionieren0,
        VialPositionierenXY,
        VialPositionierenZ,
        VialPositionierenWeg,
        Idle,
        Reset
    };
    Status status = Status::InitialisierenZ;

    DCMotor firstMotor;
    Servo servoSecond;
    Servo servoGripper;

    DigitalOut enableMotors;
    DCMotor zMotor;

    DebounceIn anschlagZ;
    DebounceIn anschlagFirst;

public:
    Scara(bool printActive = false);


    void init();
    void cycle();
    void reset();

    void grab(Point p);
    void place(Point p);

    void setAcceleration(float acceleration);
    void setSpeed(float speed);

    void stop();
    void enable();
    void disable();

    void enableMotionPlanner();
    void disableMotionPlanner();

    bool isEnabled();
    bool isDisabled();

    float actualSpeed();

    bool isMoving();
    bool isMovingXY();
    bool isMovingZ();

    bool isFinished();

    float getAcceleration();
    bool positionReached();

    void printstate();

private:

    // Handler für Anschläge
    void Zanschlag_Handler_rise();
    void Zanschlag_Handler_fall();
    void Firstanschlag_Handler_rise();
    void Firstanschlag_Handler_fall();

    bool updateTarget();
    void updateCurrent();

    void openGripper();
    void closeGripper();

    bool isGripperOpen();
    bool isGripperClose();
    
    bool isCloseTo(double a, double b, float delta);

};

#endif
