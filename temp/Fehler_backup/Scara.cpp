// Scara.h
#include "Scara.h"

Scara::Scara()
    : target(0, 0, LENGTH, 0),
      current(0, 0, 0),
      doStop(false),
      firstMotor(
          PB_PWM_M3,
          PB_ENC_A_M3,
          PB_ENC_B_M3,
          uebersetzungsverhaeltnisFirst,
          motorkonstante,
          maximaleSpannung),
      servoSecond(PB_D1),
      servoGripper(PB_D2),
      enableMotors(PB_ENABLE_DCMOTORS),
      zMotor(
          PB_PWM_M2,
          PB_ENC_A_M2,
          PB_ENC_B_M2,
          uebersetzungsverhaeltnisZ,
          motorkonstante,
          maximaleSpannung),
      anschlagZ(PA_8),
      anschlagFirst(PA_9) {
        init();
      }


//##########################################################################################
// Initalisierung Scara
//##########################################################################################
//
// Einmaliges Aufrufen am Anfang
void Scara::init() {

    printf("beginn der Initialisierung\n");

    // anschlagZ.mode(PullUp);
    // anschlagFirst.mode(PullUp);

    // status = Status::InitialisierenZ;

    // // min und max PWM second
    // float minPWMsecond = 0.011f;
    // float maxPWMsecond = 0.115f;

    // float servo_D1_ang_min = 0.03f;
    // float servo_D1_ang_max = 0.1115f;

    // // min und max PWM gripper
    // float minPWMgripper = 0.011f;
    // float maxPWMgripper = 0.115f;

    // // PWM Werte setzen
    // servoSecond.calibratePulseMinMax(minPWMsecond, maxPWMsecond);
    // servoGripper.calibratePulseMinMax(minPWMgripper, maxPWMgripper);

    // enableMotionPlanner();
    // setAcceleration(zMotor.getMaxAcceleration() * 0.5f);
    // setSpeed(zMotor.getMaxPhysicalVelocity() * 0.5f);

    // zMotor.resetPosition();
    // zMotor.setRotation(0.0f);
    // zMotor.setVelocity(0.0f);

    // enable();

    // zMotor.resetPosition();
    // zMotor.setVelocity(0.0f);
    // zMotor.setRotation(0.0f);

    // printstate();

    printf("vel: %f targetVel: %f\n",
       zMotor.getVelocity(),
       zMotor.getVelocityTarget());


    zMotor.setMaxVelocity(zMotor.getMaxPhysicalVelocity()*0.5f);

    zMotor.enableMotionPlanner();
    zMotor.setMaxAcceleration(zMotor.getMaxAcceleration());

    enableMotors = 0;  // zuerst aus

    zMotor.resetPosition();
    zMotor.setRotation(1.0f);

    enable();          // erst danach einschalten
    printf("PWM: %f\n", zMotor.getPWM());


    printf("Ende der Initialisierung\n");
while(true){
    printf("current rot: %f\t", zMotor.getRotation());
    printf("target rot: %f\t", zMotor.getRotationTarget());
    printf("PWM: %f\n", zMotor.getPWM());
}
}

//##########################################################################################
// Wiederholung Scara
//##########################################################################################
//
// Wiederhohlte Aufrufe (50Hz)
void Scara::cycle() {
    printf("beginn cycle:\t");

    zAnschlag = (anschlagZ.read() == 0);
    firstAnschlag = (anschlagFirst.read() == 0);

    printf("SchalterZ: %1d\n", zAnschlag);

    if (doStop) {
        stop();
        updateCurrent();
        return;
    }

    switch (status) {
        case Status::InitialisierenZ:
            printf("init_z\n");
            zMotor.setVelocity(0.1f);
            if (zAnschlag){
                zMotor.setVelocity(0.0f);
                zMotor.resetPosition();
                zMotor.setRotation(0.0f);

                status = Status::InitialisierenSecond;
            }
            break;

        case Status::InitialisierenSecond:
            printf("init_second\n");

            servoSecond.setPulseWidth(0.0f);
            if (isCloseTo(servoSecond.getCurrentPulseWidth(), 0.0f, DELTA)){
                status = Status::InitialisierenFirst;
            }
            break;

        case Status::InitialisierenFirst:
            printf("init_first\n");    

            firstMotor.setVelocity(0.01f);
            if (firstAnschlag){
                firstMotor.setVelocity(0.0f);
                firstMotor.resetPosition();
                firstMotor.setRotation(0.0f);

                status = Status::Idle;
            }
            break;

        case Status::VialGreifen:
            printf("vial Greiffen\n");

            updateTarget();
            updateCurrent();
            break;

        case Status::VialPositionieren:
            printf("vial positionieren\n");
            
            updateTarget();
            updateCurrent();
            break;

        case Status::Idle:
            printf("idle\n");

            updateTarget();
            updateCurrent();
            break;

        case Status::Reset:
            printf("reset\n");

            break;
}

}

// Befehle
//##########################################################################################
// Scara geht zu dieser Position
//##########################################################################################
//
// Scara Ziel: Kartesische Position
bool Scara::updateTarget() {
    Pscara pscara = target.getScara(LENGTH);

    if (pscara.impossible) {
        return false;
    }

    firstMotor.setRotation(pscara.theta_1/(2.0f*M_PI));
    servoSecond.setPulseWidth(pscara.theta_2/M_PI);

    zMotor.setRotation(pscara.z / GEWINDESTEIGUNG);

    return true;
}

//##########################################################################################
// greifft das Vial
//##########################################################################################
//
// greifft die Vial basierend auf der höhe des Vials
void Scara::grabVial(uint32_t height) {
}

//##########################################################################################
// platziert das Vial
//##########################################################################################
//
// platziert das Vial basierend auf der Zielhöhe des Vials
void Scara::placeVial(uint32_t height) {
}

//##########################################################################################
// setzt die max. Beschleunigung
//##########################################################################################
//
// setzt die max. Servo und DC Beschleunigung
void Scara::setAcceleration(float acceleration) {
    firstMotor.setMaxAcceleration(acceleration / 2);
    servoSecond.setMaxAcceleration(acceleration);
    servoGripper.setMaxAcceleration(acceleration);

    zMotor.setMaxAcceleration(acceleration * 2);
}

//##########################################################################################
// setzt die max. Geschwindigkeit
//##########################################################################################
//
// setzt die max. Servo und DC Geschwindigkeit
void Scara::setSpeed(float speed) {
    firstMotor.setMaxVelocity(speed / 2);
    servoSecond.setMaxVelocity(speed);
    servoGripper.setMaxVelocity(speed);

    zMotor.setMaxVelocity(speed * 2);
}

//##########################################################################################
// Stoppt den Scara
//##########################################################################################
//
// Stoppt alle Motoren des Scaras
void Scara::stop() {
    doStop = true;

    target = current;

    if (!isMovingXY()) {
        doStop = false;
    }
}

//##########################################################################################
// enabled den Scara
//##########################################################################################
//
// enabled die Motoren des Scaras
void Scara::enable() {
    enableMotors = 1;

    // if (!servoSecond.isEnabled()) {
    //     servoSecond.enable();
    // }

    // if (!servoGripper.isEnabled()) {
    //     servoGripper.enable();
    // }
}

//##########################################################################################
// disabled den Scara
//##########################################################################################
//
// disabled die Motoren des Scaras
void Scara::disable() {
    enableMotors = 0;

    if (servoSecond.isEnabled()) {
        servoSecond.disable();
    }

    if (servoGripper.isEnabled()) {
        servoGripper.disable();
    }
}

//##########################################################################################
// enabled den Motion Planer
//##########################################################################################
//
// enabled den Motion Planer
void Scara::enableMotionPlanner() {
    firstMotor.enableMotionPlanner();
    zMotor.enableMotionPlanner();

}

//##########################################################################################
// disabled den Motion Planer
//##########################################################################################
//
// disabled den Motion Planer
void Scara::disableMotionPlanner() {
    firstMotor.disableMotionPlanner();
    zMotor.disableMotionPlanner();
}

//##########################################################################################
// öffnet den Gripper
//##########################################################################################
//
// setzt PWM des grippers auf geschlossen
void Scara::openGripper() {
    servoGripper.setPulseWidth(OFFEN);
}

//##########################################################################################
// Schliesst den Gripper
//##########################################################################################
//
// setzt PWM des grippers auf geschlossen
void Scara::closeGripper() {
    servoGripper.setPulseWidth(GESCHLOSSEN);
}






// Rückmeldungen

//##########################################################################################
// Statusabfrage, ob alle Motoren enabled
//##########################################################################################
//
// return       bool        true = alle Motoren Enabled
bool Scara::isEnabled() {
    if (!servoSecond.isEnabled()) return false;
    if (!servoGripper.isEnabled()) return false;
    if (!enableMotors.read()) return false;

    return true;
}

//##########################################################################################
// Statusabfrage, ob alle Motoren disabled
//##########################################################################################
//
// return       bool        true = alle Motoren disabled
bool Scara::isDisabled() {
    if (servoSecond.isEnabled()) return false;
    if (servoGripper.isEnabled()) return false;
    if (enableMotors.read()) return false;

    return true;
}

//##########################################################################################
// Erhalde die aktuelle Geschwindigkeit
//##########################################################################################
//
// return       uint32_t    akt. Geschwindigkeit
uint32_t Scara::actualSpeed() {
    float first = fabs(firstMotor.getVelocity());
    float second = fabs(servoSecond.getCurrentVelocity());
    float z = fabs(zMotor.getVelocity());

    return static_cast<uint32_t>(fmax(first, fmax(second, z)));
}

//##########################################################################################
// zustandsabfrage, ob bewegung passiert
//##########################################################################################
//
// return       bool        true = Scara bewegt sich
bool Scara::isMoving() {
    if (doStop) return true;
    if (isMovingXY()) return true;
    if (isMovingZ()) return true;

    return false;
}

//##########################################################################################
// zustandsabfrage, ob bewegung in X oder Y Achse passiert
//##########################################################################################
//
// return       bool        true = Scara bewegt sich in X oder Y Ebene
bool Scara::isMovingXY() {
    if (!isCloseTo(firstMotor.getVelocity(), 0.0f, DELTA)) {
        return true;
    }

    if (!isCloseTo(servoSecond.getCurrentVelocity(), 0.0f, DELTA)) {
        return true;
    }

    return false;
}

//##########################################################################################
// zustandsabfrage, ob bewegung in Z Achse passiert
//##########################################################################################
//
// return       bool        true = Scara bewegt sich in Z Ebene
bool Scara::isMovingZ() {
    if (!isCloseTo(zMotor.getVelocity(), 0.0f, DELTA)) {
        return true;
    }

    return false;
}

//##########################################################################################
// Erhalte die Max. Beschleunigung
//##########################################################################################
//
// return       uint32_t    max. Beschleunigung
uint32_t Scara::getAcceleration() {
    if (isCloseTo(firstMotor.getMaxAcceleration() * 2,
                  servoSecond.getMaxAcceleration(),
                  DELTA)) {
        return servoSecond.getMaxAcceleration();
    }

    return static_cast<uint32_t>(-1);
}

//##########################################################################################
// Statusabfrage, ob die Position erreich ist
//##########################################################################################
//
// return       bool            true = position erreicht
bool Scara::positionReached() {
    if (doStop) return true;

    Cartesian targetCartesian = target.getCartesian();
    Cartesian currentCartesian = current.getCartesian();

    if (!isCloseTo(targetCartesian.x, currentCartesian.x, DELTA)) return false;
    if (!isCloseTo(targetCartesian.y, currentCartesian.y, DELTA)) return false;
    if (!isCloseTo(targetCartesian.z, currentCartesian.z, DELTA)) return false;

    return true;
}
//##########################################################################################
// Setzt die Aktuelle Position korrekt
//##########################################################################################
//
// return       void        aktuelle Position update
void Scara::updateCurrent() {
    current.setScara(
        firstMotor.getRotation()*2.0f*M_PI,
        servoSecond.getCurrentPulseWidth()*M_PI,
        LENGTH,
        zMotor.getRotation() * GEWINDESTEIGUNG);
}

//##########################################################################################
// Gibt diverse Daten aus, zur Fehlersuche
//##########################################################################################
//
// return       printf
void Scara::printstate(){
    updateCurrent();
    Pscara Pcur = current.getScara(LENGTH);
    Pscara Ptarget = target.getScara(LENGTH);

    printf("\n\nrot: %8f\tvel: %8f\tvolt: %8f\tpwm: %8f\trot_target: %8f\tenc: %ld\n\n",
           zMotor.getRotation(),
           zMotor.getVelocity(),
           zMotor.getVoltage(),
           zMotor.getPWM(),
           zMotor.getRotationTarget(),
           zMotor.getEncoderCount());

    printf("ENC M1: %ld\tM2: %ld\n", firstMotor.getEncoderCount(), zMotor.getEncoderCount());
    printf("pos.: M1  cur:%8f\ttarget:%8f\tM2  cur:%8f\ttarget:%8f\t\timpossible:%1d\n", Pcur.theta_1, Ptarget.theta_1, Pcur.z, Ptarget.z, Ptarget.impossible);
    printf("rot.   M1: %8f\tM2: %8f\t\t velocity     M1: %5f\tM2: %5f\n", firstMotor.getRotation(), zMotor.getRotation(), firstMotor.getMaxVelocity(), zMotor.getMaxVelocity());
    return;
}


//##########################################################################################
// abfragen, ob zwei double nache sind
//##########################################################################################
//
// return       bool        true = nache beieinander
bool Scara::isCloseTo(double a, double b, float delta) {
    delta = fabs(delta);

    return (a >= (b - delta) && a <= (b + delta));
}

//##########################################################################################
// abfragen, ob Gripper offen ist
//##########################################################################################
//
// return       bool        true = gripper offen
bool Scara::isGripperOpen() {
    return isCloseTo(servoGripper.getCurrentPulseWidth(), OFFEN, DELTA);
}


//##########################################################################################
// abfragen, ob Gripper geschlossen ist
//##########################################################################################
//
// return       bool        true = gripper gesclossen
bool Scara::isGripperClose() {
    return isCloseTo(servoGripper.getCurrentPulseWidth(), GESCHLOSSEN, DELTA);
}