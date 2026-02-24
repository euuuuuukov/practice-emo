#include "field_searcher.h"
#include "math_model.h"
#include <cmath>
#include <algorithm>

using namespace std;

FieldSearcher::GridMarker::GridMarker(double step_,
                                       double xmin, double xmax,
                                       double ymin, double ymax,
                                       double zmin, double zmax)
    : step(step_), x0(xmin), y0(ymin), z0(zmin) {
    nx = static_cast<int>((xmax - xmin) / step) + 1;
    ny = static_cast<int>((ymax - ymin) / step) + 1;
    nz = static_cast<int>((zmax - zmin) / step) + 1;
    visited.assign(nx * ny * nz, false);
}

bool FieldSearcher::GridMarker::isVisited(double x, double y, double z) const {
    int ix = static_cast<int>((x - x0) / step + 0.5);
    int iy = static_cast<int>((y - y0) / step + 0.5);
    int iz = static_cast<int>((z - z0) / step + 0.5);
    if (ix < 0 || ix >= nx || iy < 0 || iy >= ny || iz < 0 || iz >= nz)
        return true;
    return visited[ix * ny * nz + iy * nz + iz];
}

void FieldSearcher::GridMarker::mark(double x, double y, double z) {
    int ix = static_cast<int>((x - x0) / step + 0.5);
    int iy = static_cast<int>((y - y0) / step + 0.5);
    int iz = static_cast<int>((z - z0) / step + 0.5);
    if (ix >= 0 && ix < nx && iy >= 0 && iy < ny && iz >= 0 && iz < nz)
        visited[ix * ny * nz + iy * nz + iz] = true;
}

FieldSearcher::FieldSearcher(const Settings& settings) : set(settings) {}

bool FieldSearcher::findSeedPoint(EPoint& seed, const GridMarker& marker) {
    double step = set.coarseStep;
    while (step >= set.fineStep - 1e-9) {
        for (double x = set.Xmin; x <= set.Xmax; x += step) {
            for (double y = set.Ymin; y <= set.Ymax; y += step) {
                for (double z = set.Zmin; z <= set.Zmax; z += step) {
                    if (marker.isVisited(x, y, z)) continue;
                    double val = MathModel::computeE(x, y, z);
                    if (val > set.threshold) {
                        seed = EPoint(x, y, z, val);
                        return true;
                    }
                }
            }
        }
        step /= 2.0;
    }
    return false;
}

double FieldSearcher::findBoundary(const EPoint& start, double dx, double dy, double dz,
                                    double maxStep, double minStep) {
    double len = sqrt(dx*dx + dy*dy + dz*dz);
    if (len < 1e-12) return 0.0;
    dx /= len; dy /= len; dz /= len;

    double step = maxStep;
    EPoint inside = start;
    EPoint outside = start;

    bool outsideFound = false;
    const int MAX_ITER = 100;
    int iter = 0;

    while (!outsideFound && iter < MAX_ITER) {
        EPoint test;
        test.x = outside.x + dx * step;
        test.y = outside.y + dy * step;
        test.z = outside.z + dz * step;

        if (test.x < set.Xmin || test.x > set.Xmax ||
            test.y < set.Ymin || test.y > set.Ymax ||
            test.z < set.Zmin || test.z > set.Zmax) {
            outside = test;
            outsideFound = true;
            break;
        }

        test.E = MathModel::computeE(test.x, test.y, test.z);
        if (test.E > set.threshold) {
            inside = test;
            step *= 1.5;
        } else {
            outside = test;
            outsideFound = true;
        }
        ++iter;
    }

    if (!outsideFound) {
        return inside.x * dx + inside.y * dy + inside.z * dz;
    }

    while (sqrt((outside.x - inside.x)*(outside.x - inside.x) +
                (outside.y - inside.y)*(outside.y - inside.y) +
                (outside.z - inside.z)*(outside.z - inside.z)) > minStep) {
        EPoint mid;
        mid.x = (inside.x + outside.x) * 0.5;
        mid.y = (inside.y + outside.y) * 0.5;
        mid.z = (inside.z + outside.z) * 0.5;
        mid.E = MathModel::computeE(mid.x, mid.y, mid.z);

        if (mid.E > set.threshold) {
            inside = mid;
        } else {
            outside = mid;
        }
    }

    if (fabs(dx) > 0.99) return inside.x;
    if (fabs(dy) > 0.99) return inside.y;
    return inside.z;
}

ERegion FieldSearcher::growRegion(const EPoint& seed, GridMarker& marker) {
    ERegion reg;

    reg.x_min = findBoundary(seed, -1, 0, 0, set.borderCoarse, set.borderFine);
    reg.x_max = findBoundary(seed,  1, 0, 0, set.borderCoarse, set.borderFine);
    reg.y_min = findBoundary(seed, 0, -1, 0, set.borderCoarse, set.borderFine);
    reg.y_max = findBoundary(seed, 0,  1, 0, set.borderCoarse, set.borderFine);
    reg.z_min = findBoundary(seed, 0, 0, -1, set.borderCoarse, set.borderFine);
    reg.z_max = findBoundary(seed, 0, 0,  1, set.borderCoarse, set.borderFine);

    reg.step = set.detailStep;
    reg.nx = static_cast<size_t>((reg.x_max - reg.x_min) / reg.step) + 1;
    reg.ny = static_cast<size_t>((reg.y_max - reg.y_min) / reg.step) + 1;
    reg.nz = static_cast<size_t>((reg.z_max - reg.z_min) / reg.step) + 1;

    reg.field.resize(reg.nx * reg.ny * reg.nz);

    for (size_t ix = 0; ix < reg.nx; ++ix) {
        double x = reg.x_min + ix * reg.step;
        for (size_t iy = 0; iy < reg.ny; ++iy) {
            double y = reg.y_min + iy * reg.step;
            for (size_t iz = 0; iz < reg.nz; ++iz) {
                double z = reg.z_min + iz * reg.step;
                double val = MathModel::computeE(x, y, z);
                reg.field[ix * reg.ny * reg.nz + iy * reg.nz + iz] = val;
                marker.mark(x, y, z);
            }
        }
    }

    return reg;
}

vector<ERegion> FieldSearcher::findAllRegions() {
    vector<ERegion> result;

    GridMarker marker(set.coarseStep, set.Xmin, set.Xmax, set.Ymin, set.Ymax, set.Zmin, set.Zmax);

    EPoint seed;
    while (findSeedPoint(seed, marker)) {
        ERegion region = growRegion(seed, marker);
        result.push_back(region);
    }

    return result;
}