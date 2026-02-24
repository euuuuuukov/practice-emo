#include "math_model.h"
#include <cmath>

using namespace std;

namespace MathModel {
    double computeE(double x, double y, double z) {
        double dx1 = x - 10.0;
        double dy1 = y;
        double dz1 = z - 5.0;
        double r1_sq = (dx1*dx1) / (12.0*12.0) + (dy1*dy1) / (8.0*8.0) + (dz1*dz1) / (8.0*8.0);
        double E1 = 200.0 * exp(-r1_sq);

        double dx2 = x + 20.0;
        double dy2 = y - 10.0;
        double dz2 = z + 15.0;
        double r2_sq = (dx2*dx2 + dy2*dy2 + dz2*dz2) / (10.0*10.0);
        double E2 = 150.0 * exp(-r2_sq);

        return E1 + E2;
    }
}