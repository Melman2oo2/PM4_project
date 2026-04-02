// Servoansteuerung.h

#ifndef PM2_HEBEMECHANISMUS_H
#define PM2_HEBEMECHANISMUS_H

// Header hier einfügen

void initHebemechanismus();
void cycleHebemechanismus();
// Befehle
void GewichtAbsenken();
void GewichtAnheben();
void setBeschleunigungHebemechanismus(float beschleunigung);
void StopHebemechanismus();
void enableHebemechanismus();
void disableHebemechanismus();

void GewichtSetPositionRechts(float position);
void GewichtSetPositionLinks(float position);
// Rückmeldungen
bool GewichtUnten();
bool GewichtOben();
bool GewichtInBewegung();
float getBeschleunigungHebemechanismus();
bool isEnabledHebemechanismus();
bool isDisabledHebemechanismus();

bool isCloseto(float a, float b, float delta);
#endif
