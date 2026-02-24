#pragma once

#include <vector>

struct ERegion {
    double x_min, x_max;
    double y_min, y_max;
    double z_min, z_max;
    double step;

    std::vector<double> field;
    size_t nx, ny, nz;

    ERegion()
        : x_min(0), x_max(0), y_min(0), y_max(0), z_min(0), z_max(0), step(0), nx(0), ny(0), nz(0) {}
};