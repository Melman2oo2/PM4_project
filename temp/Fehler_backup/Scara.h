// Scara.h
#ifndef SCARA_H
#define SCARA_H

#include "mbed.h"
#include "PESBoardPinMap.h"
#include "FastPWM.h"
#include "DCMotor.h"
#include "Servo.h"
#include "Point.h"
#include "DigitalIn.h"

class Scara {
public:
    Point target;
    Point current;

    bool doStop;


private:
    static constexpr float SCARA_FIRST_MAX = 1.0f;
    static constexpr float SCARA_FIRST_MIN = 0.0f;

    static constexpr float SCARA_SECOND_MAX = 0.0f;
    static constexpr float SCARA_SECOND_MIN = 1.0f;

    static constexpr float SCARA_GRIPPER_MAX = 0.0f;
    static constexpr float SCARA_GRIPPER_MIN = 1.0f;

    static constexpr float DELTA = 0.001f;
    static constexpr float LENGTH = 20.0f;
    static constexpr float GEWINDESTEIGUNG = -8.0f;

    static constexpr float OFFEN = 1.0f;
    static constexpr float GESCHLOSSEN = 0.0f;

    static constexpr float maximaleSpannung = 12.0f;
    static constexpr float uebersetzungsverhaeltnisZ = 31.0f;
    static constexpr float uebersetzungsverhaeltnisFirst = 488.0f;
    static constexpr float motorkonstante = 28.0f / 12.0f;

    volatile bool zAnschlag = false;
    volatile bool firstAnschlag = false;

    enum class Status {
        InitialisierenZ,
        InitialisierenSecond,
        InitialisierenFirst,
        VialGreifen,
        VialPositionieren,
        Idle,
        Reset
    };
    Status status = Status::InitialisierenZ;

    DCMotor firstMotor;
    Servo servoSecond;
    Servo servoGripper;

    DigitalOut enableMotors;
    DCMotor zMotor;

    DigitalIn anschlagZ;
    DigitalIn anschlagFirst;

public:
    Scara();


    void init();
    void cycle();

    void grabVial(uint32_t height);
    void placeVial(uint32_t height);

    void setAcceleration(float acceleration);
    void setSpeed(float speed);

    void stop();
    void enable();
    void disable();

    void enableMotionPlanner();
    void disableMotionPlanner();

    bool isEnabled();
    bool isDisabled();

    uint32_t actualSpeed();

    bool isMoving();
    bool isMovingXY();
    bool isMovingZ();

    uint32_t getAcceleration();
    bool positionReached();

    void printstate();

private:
    bool updateTarget();
    void updateCurrent();

    void openGripper();
    void closeGripper();

    bool isGripperOpen();
    bool isGripperClose();
    
    bool isCloseTo(double a, double b, float delta);

};

#endif