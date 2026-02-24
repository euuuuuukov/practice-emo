#pragma once

struct EPoint {
    double x, y, z;
    double E;

    EPoint() : x(0), y(0), z(0), E(0) {}
    EPoint(double x1, double y1, double z1, double E1)
        : x(x1), y(y1), z(z1), E(E1) {}
};