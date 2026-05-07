// Point.h
#ifndef POINT_H
#define POINT_H

#include <cmath>

struct Cartesian {
    double x;
    double y;
    double z;
};

struct Cylindrical {
    double r;
    double phi;
    double z;
};

struct Pscara {
    double theta_1;
    double theta_2;
    double z;
    bool impossible;
};

class Point {
private:
    double x;
    double y;
    double z;

public:
    Point(double xValue, double yValue, double zValue = 0.0);
    Point(double theta_1, double theta_2, double length, double zValue);

    Cartesian getCartesian() const;
    Cylindrical getCylindrical() const;
    Pscara getScara(double length) const;

    void setCartesian(double xValue, double yValue, double zValue = 0.0);
    void setCylindrical(double r, double phi, double zValue = 0.0);
    void setScara(double theta_1, double theta_2, double length, double zValue = 0.0);
};

#endif