// Scara.h

#ifndef SCARA_H
#define SCARA_H

    #include <cmath> 

    void init_Scara();
    void cycle_Scara();

    // Befehle
    bool updatescara_target();
    void grabVial_Scara(uint32_t height);
    void placeVial_Scara(uint32_t height);
    void setAcceleration_Scara(uint32_t acceleration);
    void setSpeed_Scara(uint32_t speed);
    void stop_Scara();
    void stop_Scara_x_y();
    void stop_Scara_z();
    void enable_Scara();
    void disable_Scara();
    void enableMotionPlaner_Scara_z();
    void disableMotionPlaner_Scara_z();

    //Rueckmeldungen
    bool isEnabled_Scara();
    bool isdiabled_Scara();
    //uint32_t actualSpeed_Scara_x_y();
    bool isMoving_Scara();
    bool isMoving_Scara_x_y();
    bool isMoving_Scara_z();
    uint32_t getAcceleration_Scara();
    //bool positionErreich_Scara();
    void updatescara_curr();

    bool isCloseto_Scara(double a, double b, float delta);

#endif
