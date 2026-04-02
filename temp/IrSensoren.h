// Irsensoren.h

#ifndef PM2_IRSENSOREN_H
#define PM2_IRSENSOREN_H

void cyclicIrSensoren();
bool sensorVorGewichtBodenErkannt();
bool sensorHinterGewichtBodenErkannt();
float sensorDistanzNachVorneInCm();
float irSensorDistanzErmitteln(float, float, float);




#endif
