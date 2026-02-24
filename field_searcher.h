#pragma once

#include "point.h"
#include "region.h"
#include <vector>

class FieldSearcher {
public:
    struct Settings {
        double Xmin, Xmax;
        double Ymin, Ymax;
        double Zmin, Zmax;
        double threshold;    // порог по E

        double coarseStep;   // максимальный шаг поиска
        double fineStep;     // минимальный шаг поиска (для уточнения)

        double borderCoarse; // шаг для грубого поиска границы
        double borderFine;   // точность определения границы

        double detailStep;   // шаг детального заполнения тензора
    };

    FieldSearcher(const Settings& settings);
    std::vector<ERegion> findAllRegions();

private:
    Settings set;

    struct GridMarker {
        double step;
        double x0, y0, z0;
        int nx, ny, nz;
        std::vector<bool> visited;

        GridMarker(double step, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
        bool isVisited(double x, double y, double z) const;
        void mark(double x, double y, double z);
    };

    bool findSeedPoint(EPoint& seed, const GridMarker& marker);
    double findBoundary(const EPoint& start, double dx, double dy, double dz, double maxStep, double minStep);
    ERegion growRegion(const EPoint& seed, GridMarker& marker);
};