// Point.h
#ifndef POINT_H
#define POINT_H

#include <cmath>

struct Cartesian {
    float x;
    float y;
    float z;
};

struct Cylindrical {
    float r;
    float phi;
    float z;
};

struct Pscara {
    float theta_1;
    float theta_2;
    float z;
    bool impossible;
};

class Point {
private:
    double x;
    double y;
    double z;

public:
    Point();
    Point(float xValue, float yValue, float zValue = 0.0);
    Point(float theta_1, float theta_2, float length, float zValue);

    Cartesian getCartesian() const;
    Cylindrical getCylindrical() const;
    Pscara getScara(float length) const;

    void setCartesian(float xValue, float yValue, float zValue = 0.0);
    void setCartesian(Cartesian cartesian);
    void setCylindrical(float r, float phi, float zValue = 0.0);
    void setCylindrical(Cylindrical cylindrical);
    void setScara(float theta_1, float theta_2, float length, float zValue = 0.0);
    void setScara(Pscara pscara, float length);

    void setX(float xValue);
    void setY(float yValue);
    void setZ(float zValue);
};

#endif