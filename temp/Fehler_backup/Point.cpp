// Point.cpp
#include "Point.h"

Point::Point(double xValue, double yValue, double zValue) {
    x = xValue;
    y = yValue;
    z = zValue;
}

Point::Point(double theta_1, double theta_2, double length, double zValue) {
    double x1 = length * std::cos(theta_1);
    double y1 = length * std::sin(theta_1);

    double x2 = length * std::cos(theta_1 + theta_2);
    double y2 = length * std::sin(theta_1 + theta_2);

    x = x1 + x2;
    y = y1 + y2;
    z = zValue;
}

Cartesian Point::getCartesian() const {
    return {x, y, z};
}

Cylindrical Point::getCylindrical() const {
    Cylindrical c;

    c.r = std::hypot(x, y);
    c.phi = std::atan2(y, x);
    c.z = z;

    return c;
}

Pscara Point::getScara(double length) const {
    Pscara p;

    double r = std::hypot(x, y);
    double phi = std::atan2(y, x);

    if(r * 0.95 > 2.0 * length) {
        p.impossible = true;
        return p;
    }

    double alpha = std::acos(r / (2.0 * length));

    p.theta_1 = phi - alpha;
    p.theta_2 = 2.0 * alpha;
    p.z = z;
    p.impossible = false;

    return p;
}

void Point::setCartesian(double xValue, double yValue, double zValue) {
    x = xValue;
    y = yValue;
    z = zValue;
}

void Point::setCylindrical(double r, double phi, double zValue) {
    x = r * std::cos(phi);
    y = r * std::sin(phi);
    z = zValue;
}

void Point::setScara(double theta_1, double theta_2, double length, double zValue) {
    double x1 = length * std::cos(theta_1);
    double y1 = length * std::sin(theta_1);

    double x2 = length * std::cos(theta_1 + theta_2);
    double y2 = length * std::sin(theta_1 + theta_2);

    x = x1 + x2;
    y = y1 + y2;
    z = zValue;
}