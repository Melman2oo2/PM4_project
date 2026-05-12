// Scara.h
#include "Scara.h"

Scara::Scara(bool printActive):
       target(0, 0, LENGTH, 0),
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
          maximaleSpannung),
      anschlagZ(PC_0, PullUp),
      anschlagFirst(PC_1, PullUp),
      grabPlacePoint(0, 0, 0)
      {
        this->printActive = printActive;
        init();
      }


//##########################################################################################
// Initalisierung Scara
//##########################################################################################
//
// Einmaliges Aufrufen am Anfang
void Scara::init() {

    if(printActive) printf("beginn der Initialisierung\n");

    status = Status::InitialisierenZ;

    // Anschlagsschalter konfigurieren//

    anschlagZ.rise(callback(this, &Scara::Zanschlag_Handler_rise), 20ms);
    anschlagZ.fall(callback(this, &Scara::Zanschlag_Handler_fall), 20ms);

    anschlagFirst.rise(callback(this, &Scara::Firstanschlag_Handler_rise), 20ms);
    anschlagFirst.fall(callback(this, &Scara::Firstanschlag_Handler_fall), 20ms);

    zAnschlag = !(anschlagZ.read() == 0);
    firstAnschlag = !(anschlagFirst.read() == 0);

    //Servos konfigurieren//

    // min und max PWM second
    float minPWMsecond = 0.03f;
    float maxPWMsecond = 0.1115f;

    // min und max PWM gripper
    float minPWMgripper = 0.045f;
    float maxPWMgripper = 0.11f;

    // PWM Werte setzen
    servoSecond.calibratePulseMinMax(minPWMsecond, maxPWMsecond);
    servoGripper.calibratePulseMinMax(minPWMgripper, maxPWMgripper);

    // Motoren konfigurieren//

    enableMotionPlanner();
    setAcceleration(0.5f);
    setSpeed(0.5f);

    zMotor.resetPosition();
    zMotor.setVelocity(0.0f);
    zMotor.setRotation(0.0f);

    firstMotor.resetPosition();
    firstMotor.setVelocity(0.0f);
    firstMotor.setRotation(0.0f);

    printstate();

    if(printActive) printf ("Ende der Initialisierung\n");
}

//##########################################################################################
// Wiederholung Scara
//##########################################################################################
//
// Wiederhohlte Aufrufe (50Hz)
void Scara::cycle() {
    if(printActive) printf("\nbeginn cycle:\t");

    if(!isEnabled()) enable();

    openGripper();

    if (doStop) {
        stop();
        updateCurrent();
        return;
    }

    switch (status) {
        case Status::InitialisierenZ:
            if(printActive) printf("init_z\t");
            zMotor.setVelocity(0.1f);

            if (zAnschlag){
                zMotor.setVelocity(0.0f);
                zMotor.resetPosition();
                zMotor.setRotation(0.0f);

                status = Status::InitialisierenSecond;
            }
            break;

        case Status::InitialisierenSecond:
            if(printActive) printf("init_second\t");

            servoSecond.setPulseWidth(0.0f);
            if (isCloseTo(servoSecond.getCurrentPulseWidth(), 0.0f, DELTA)){
                status = Status::InitialisierenFirst;
            }
            break;

        case Status::InitialisierenFirst:
            if(printActive) printf("init_first\t");
            firstMotor.setVelocity(0.01f);

            if (firstAnschlag){
                firstMotor.setVelocity(0.0f);
                firstMotor.resetPosition();
                firstMotor.setRotation(0.0f);

                status = Status::Idle;
            }
            break;

        case Status::VialGreifen0:
            if(printActive) printf("vial Greiffen\t");

            // Greiffer nach oben fahren und öffnen
            target.setZ(-1.0f);
            openGripper();

            updateTarget();
            updateCurrent();

            if(positionReached()) {
                status = Status::VialGreifenXY;
            }
            break;

        case Status::VialGreifenXY:
            if(printActive) printf("vial Greiffen XY\t");

            // Greiffer über das Vial fahren
            target = grabPlacePoint;
            target.setZ(-1.0f);

            updateTarget();
            updateCurrent();

            if(positionReached() && isGripperOpen()) {
                status = Status::VialGreifenZ;
            }
            break;
        
        case Status::VialGreifenZ:
            if(printActive) printf("vial Greiffen Z\t");

            // Zum Vial fahren
            target = grabPlacePoint;

            updateTarget();
            updateCurrent();

            if(positionReached()) {
                closeGripper();
                status = Status::Idle;
            }

            break;

        case Status::VialPositionieren0:
            if(printActive) printf("vial positionieren\t");

            // Greiffer nach oben und schliessen
            target.setZ(-1.0f);
            closeGripper();
            
            updateTarget();
            updateCurrent();

            if (positionReached()) {
                status = Status::VialPositionierenXY;
            }
            break;

        case Status::VialPositionierenXY:
            if(printActive) printf("vial positionieren XY\t");

            // Greiffer über Ziel fahren
            target = grabPlacePoint;
            target.setZ(-1.0f);

            updateTarget();
            updateCurrent();

            if(positionReached() && isGripperClose()) {
                status = Status::VialPositionierenZ;
            }
            break;

        case Status::VialPositionierenZ:
            if(printActive) printf("vial positionieren Z\t");

            // Zum Ziel fahren und öffnen
            target = grabPlacePoint;

            updateTarget();
            updateCurrent();

            if(positionReached()) {
                openGripper();
                status = Status::VialPositionierenWeg;
            }
            break;

        case Status::VialPositionierenWeg:
            if(printActive) printf("vial positionieren weg\t");

            // Greiffer nach oben fahren
            target.setZ(-1.0f);

            updateTarget();
            updateCurrent();

            if(positionReached()) {
                status = Status::Idle;
            }
            break;

        case Status::Idle:
            if(printActive) printf("idle\t");

            updateTarget();
            updateCurrent();
            break;

        case Status::Reset:
            if(printActive) printf("reset\t");
            stop();
            break;
    }

    if(printActive) printf("ende cycle\n");
}

void Scara::reset() {
    status = Status::Reset;
    cycle();
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
void Scara::grab(Point p) {
    // wenn am Initialisieren, Greifen oder Platzieren die Aktion ignorieren
        switch(status) {
        case Status::InitialisierenZ:
        case Status::InitialisierenSecond:
        case Status::InitialisierenFirst:
        case Status::VialGreifen0:
        case Status::VialGreifenXY:
        case Status::VialGreifenZ:
        case Status::VialPositionieren0:
        case Status::VialPositionierenXY:
        case Status::VialPositionierenZ:
            return;
        default:
            break;
    }

    // grab Vial ausführen
    grabPlacePoint = p;
    status = Status::VialGreifen0;
}

//##########################################################################################
// platziert das Vial
//##########################################################################################
//
// platziert das Vial basierend auf der Zielhöhe des Vials
void Scara::place(Point p) {
    // wenn am Initialisieren, Greifen oder Platzieren die Aktion ignorieren
    switch(status) {
        case Status::InitialisierenZ:
        case Status::InitialisierenSecond:
        case Status::InitialisierenFirst:
        case Status::VialGreifen0:
        case Status::VialGreifenXY:
        case Status::VialGreifenZ:
        case Status::VialPositionieren0:
        case Status::VialPositionierenXY:
        case Status::VialPositionierenZ:
            return;
        default:
            break;
    }

    // place Vial ausführen
    grabPlacePoint = p;
    status = Status::VialPositionieren0;
}

//##########################################################################################
// setzt die max. Beschleunigung
//##########################################################################################
//
// setzt die max. Servo und DC Beschleunigung
void Scara::setAcceleration(float ratio) {
    firstMotor.setMaxAcceleration(firstMotor.getMaxAcceleration() * ratio);
    servoSecond.setMaxAcceleration(servoSecond.getMaxAcceleration() * ratio);
    servoGripper.setMaxAcceleration(servoGripper.getMaxAcceleration() * ratio);

    zMotor.setMaxAcceleration(zMotor.getMaxAcceleration() * ratio);
}

//##########################################################################################
// setzt die max. Geschwindigkeit
//##########################################################################################
//
// setzt die max. Servo und DC Geschwindigkeit
void Scara::setSpeed(float ratio) {
    firstMotor.setMaxVelocity(firstMotor.getMaxVelocity() * ratio);
    servoSecond.setMaxVelocity(ratio);
    servoGripper.setMaxVelocity(ratio);

    zMotor.setMaxVelocity(zMotor.getMaxVelocity() * ratio);
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
float Scara::actualSpeed() {
    float first = fabs(firstMotor.getVelocity());
    float second = fabs(servoSecond.getCurrentVelocity());
    float z = fabs(zMotor.getVelocity());

    return (fmax(first, fmax(second, z)));
}

//##########################################################################################
// zustandsabfrage, ob bewegung passiert
//##########################################################################################
//
// return       bool        true = Scara bewegt sich
bool Scara::isMoving() {

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
// Statusabfrage, ob Bewegung abgeschlossen ist
//##########################################################################################
//
// return       bool            true = Bewegung abgeschlossen
bool Scara::isFinished() {
    if(status==Status::Idle && positionReached()) {
        return true;
    }
    return false;
}

//##########################################################################################
// Erhalte die Max. Beschleunigung
//##########################################################################################
//
// return       uint32_t    max. Beschleunigung
float Scara::getAcceleration() {
    if (isCloseTo(firstMotor.getMaxAcceleration() * 2,
                  servoSecond.getMaxAcceleration(),
                  DELTA)) {
        return servoSecond.getMaxAcceleration();
    }

    return -1.0f;
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

    if(!printActive) return;

    printf("PWM: %f\t", servoGripper.getCurrentPulseWidth());
    printf("state offen: %1d\tstate geschlossen: %1d\n", isGripperOpen(), isGripperClose());

    // printf("Z Anschlag: %1d\tFirst Anschlag: %1d\n", zAnschlag, firstAnschlag);

    // printf("current rot: %f\t", zMotor.getRotation());
    // printf("target rot: %f\t", zMotor.getRotationTarget());
    // printf("PWM: %f\n", zMotor.getPWM());
    // updateCurrent();
    // Pscara Pcur = current.getScara(LENGTH);
    // Pscara Ptarget = target.getScara(LENGTH);

    // printf("\n\nrot: %8f\tvel: %8f\tvolt: %8f\tpwm: %8f\trot_target: %8f\tenc: %ld\n\n",
    //        zMotor.getRotation(),
    //        zMotor.getVelocity(),
    //        zMotor.getVoltage(),
    //        zMotor.getPWM(),
    //        zMotor.getRotationTarget(),
    //        zMotor.getEncoderCount());

    // printf("ENC M1: %ld\tM2: %ld\n", firstMotor.getEncoderCount(), zMotor.getEncoderCount());
    // printf("pos.: M1  cur:%8f\ttarget:%8f\tM2  cur:%8f\ttarget:%8f\t\timpossible:%1d\n", Pcur.theta_1, Ptarget.theta_1, Pcur.z, Ptarget.z, Ptarget.impossible);
    // printf("rot.   M1: %8f\tM2: %8f\t\t velocity     M1: %5f\tM2: %5f\n", firstMotor.getRotation(), zMotor.getRotation(), firstMotor.getMaxVelocity(), zMotor.getMaxVelocity());
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


//##########################################################################################
// debounce Funktionen für die Anschläge
//##########################################################################################
//
// return       void        bool korrekt gesetzt
void Scara::Zanschlag_Handler_rise() {
    zAnschlag = true;
    return;
}

void Scara::Zanschlag_Handler_fall() {
    zAnschlag = false;
    return;
}

void Scara::Firstanschlag_Handler_rise() {
    firstAnschlag = true;
    return;
}

void Scara::Firstanschlag_Handler_fall() {
    firstAnschlag = false;
    return;
}