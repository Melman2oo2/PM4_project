
#include "Scara.h"

Scara::Scara()
    : target(0, 0, LENGTH, 0),
      current(0, 0, 0),
      doStop(false),
      firstMotor(
          PB_PWM_M1,
          PB_ENC_A_M1,
          PB_ENC_B_M1,
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
          maximaleSpannung) {}


//##########################################################################################
// Initalisierung Scara
//##########################################################################################
//
// Einmaliges Aufrufen am Anfang
void Scara::init() {
    // min und max PWM second
    float minPWMsecond = 0.011f;
    float maxPWMsecond = 0.115f;

    // min und max PWM gripper
    float minPWMgripper = 0.011f;
    float maxPWMgripper = 0.115f;

    // PWM Werte setzen
    servoSecond.calibratePulseMinMax(minPWMsecond, maxPWMsecond);
    servoGripper.calibratePulseMinMax(minPWMgripper, maxPWMgripper);

    enable();

    enableMotionPlanner();
    setAcceleration(zMotor.getMaxAcceleration() * 0.5f);
}

//##########################################################################################
// Wiederholung Scara
//##########################################################################################
//
// Wiederhohlte Aufrufe (50Hz)
void Scara::cycle() {
    if (doStop) {
        stop();
    }

    updateTarget();
    updateCurrent();
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

    firstMotor.setRotation(pscara.theta_1/2.0f*M_PI);
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
void Scara::setAcceleration(uint32_t acceleration) {
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
void Scara::setSpeed(uint32_t speed) {
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

    if (!servoSecond.isEnabled()) {
        servoSecond.enable();
    }

    if (!servoGripper.isEnabled()) {
        servoGripper.enable();
    }
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
// abfragen, ob zwei double nache sind
//##########################################################################################
//
// return       bool        true = nache beieinander
bool Scara::isCloseTo(double a, double b, float delta) {
    delta = fabs(delta);

    return (a >= (b - delta) && a <= (b + delta));
}