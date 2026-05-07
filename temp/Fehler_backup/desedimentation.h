// desedimentation.h

#ifndef DESEDIMENTATION_H
#define DESEDIMENTATION_H

    void init_desedimentation();
    void cycle_desedimentation();

    // Befehle
    void start_desedimentation();
    void stop_desedimentation();
    void start_timer(float time);
    void stop_desedimentation();
    void enable_desedimentation();
    void disable_desedimentation();

    // Rueckmeldungen
    bool isEnabled_desedimentation();
    bool isDisabled_desedimentation();
    bool isVialOben_desedimentation();
    bool timerFinished();

#endif
