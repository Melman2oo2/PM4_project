// Point.cpp
#include "Point.h"

Point::Point() {
    x = 0.0;
    y = 0.0;
    z = 0.0;
}

Point::Point(float xValue, float yValue, float zValue) {
    x = xValue;
    y = yValue;
    z = zValue;
}

Point::Point(float theta_1, float theta_2, float length, float zValue) {
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

Pscara Point::getScara(float length) const {
    Pscara p;

    double r = std::hypot(x, y);

    if(r > 1.9 * length || r < 70.0) {
        p.impossible = true;
        return p;
    }

    double phi = std::atan2(y, x);

    double cos_theta_2 =
        (r * r - 2.0 * length * length) /
        (2.0 * length * length);

    if(cos_theta_2 > 1.0) {
        cos_theta_2 = 1.0;
    }

    if(cos_theta_2 < -1.0) {
        cos_theta_2 = -1.0;
    }

    // andere IK Lösung
    double theta_2 = -std::acos(cos_theta_2);

    double beta =
        std::atan2(
            length * std::sin(theta_2),
            length + length * std::cos(theta_2)
        );

    double theta_1 = phi - beta;

    // Definition beibehalten:
    // 0 = gestreckt
    // PI = eingeklappt
    p.theta_1 = theta_1;
    p.theta_2 = -theta_2;

    p.z = z;
    p.impossible = false;

    return p;
}

// Pscara Point::getScara(float length) const {
//     Pscara p;

//     double r = std::hypot(x, y);
//     double phi = std::atan2(y, x);

//     if(r < 70.0) {
//         p.impossible = true;
//         return p;
//     }

//     if(r * 0.95 > 2.0 * length) {
//         p.impossible = true;
//         return p;
//     }

//     double alpha = std::acos(r / (2.0 * length));

//     p.theta_1 = -(phi + alpha);

//     p.theta_2 = M_PI - 2.0 * alpha;

//     p.z = z;
//     p.impossible = false;

//     return p;
// }

void Point::setCartesian(float xValue, float yValue, float zValue) {
    x = xValue;
    y = yValue;
    z = zValue;
}

void Point::setCartesian(Cartesian cartesian) {
    x = cartesian.x;
    y = cartesian.y;
    z = cartesian.z;
}

void Point::setCylindrical(float r, float phi, float zValue) {
    x = r * std::cos(phi);
    y = r * std::sin(phi);
    z = zValue;
}

void Point::setCylindrical(Cylindrical cylindrical) {
    x = cylindrical.r * std::cos(cylindrical.phi);
    y = cylindrical.r * std::sin(cylindrical.phi);
    z = cylindrical.z;
}

// void Point::setScara(float theta_1, float theta_2, float length, float zValue) {

//     double alpha = (M_PI - theta_2) / 2.0;

//     double phi = -theta_1 - alpha;

//     double r = 2.0 * length * std::cos(alpha);

//     x = r * std::cos(phi);
//     y = r * std::sin(phi);
//     z = zValue;
// }

void Point::setScara(float theta_1,
                     float theta_2,
                     float length,
                     float zValue) {

    double x1 =
        length * std::cos(theta_1);

    double y1 =
        length * std::sin(theta_1);

    double x2 =
        length * std::cos(theta_1 - theta_2);

    double y2 =
        length * std::sin(theta_1 - theta_2);

    x = x1 + x2;
    y = y1 + y2;
    z = zValue;
}

void Point::setScara(Pscara pscara, float length) {
    if(pscara.impossible) {
        return;
    }

    setScara(
        pscara.theta_1,
        pscara.theta_2,
        length,
        pscara.z
    );
}

void Point::setX(float xValue) {
    x = xValue;
}

void Point::setY(float yValue) {
    y = yValue;
}

void Point::setZ(float zValue) {
    z = zValue;
}