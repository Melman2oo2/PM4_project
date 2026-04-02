// Scara.h

#ifndef SCARA_H
#define SCARA_H

    void init_Scara();
    void cycle_Scara();

    // Befehle
    void gotoPosition_Scara(uint32_t X, uint32_t Y, uint32_t Z);
    void grabVial_Scara(uint32_t height);
    void placeVial_Scara(uint32_t height);
    void setAcceleration_Scara(uint32_t acceleration);
    void setSpeed_Scara(uint32_t speed);
    void stop_Scara();
    void enable_Scara();
    void disable_Scara();
    void enableMotionPlaner_Scara();
    void disableMotionPlaner_Scara();

    //Rueckmeldungen
    bool IsEnabled_Scara();
    bool Isdiabled_Scara();
    uint32_t actualSpeed_Scara();
    uint32_t getAcceleration_Scara();
    bool positionErreich_Scara();
    uint32_t GetPosition_Scara(uint32_t axis);

#endif
