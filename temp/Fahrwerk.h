// Fahrwerk.h

#ifndef PM2_FAHRWERK_H
#define PM2_FAHRWERK_H


// Header hier einfügen

void initFahrwerk();
void cycleFahrwerk();
// Befehle
// void setGeschwindigkeit1(float geschwindigkeit, float kurvenanteil);
void setGeschwindigkeit(float geschwindigkeit, float kurvenanteil);
void setBeschleunigungFahrwerk(float beschleunigung);
void stopFahrwerk();
void enableMotoren();
void disableMotoren();
void enableMotionPlannerFahrwerk();
void disableMotionPlannerFahrwerk();
void FolgeDerLinie();
// Rückmeldungen
float getAktuelleGeschwindigkeit();
float getBeschleunigungFahrwerk();
float getLinienposition();
bool isEnabledMotoren();
bool isDisabledMotoren();


// LineFollower-Geschwindigkeitsvariablen
extern float motorSpeedLinks;
extern float motorSpeedRechts;

#endif
