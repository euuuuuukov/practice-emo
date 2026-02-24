#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include "field_searcher.h"

using namespace std;

template<typename T>
bool readValues(const string& prompt, T& val1, T& val2) {
    cout << prompt;
    string line;
    getline(cin, line);
    if (line.empty()) return false;
    istringstream iss(line);
    if (iss >> val1 >> val2) return true;
    return false;
}

template<typename T>
bool readValue(const string& prompt, T& val) {
    cout << prompt;
    string line;
    getline(cin, line);
    if (line.empty()) return false;
    istringstream iss(line);
    if (iss >> val) return true;
    return false;
}

int main() {
    FieldSearcher::Settings cfg;

    cout << "Введите параметры поиска областей ЭМО:\n";

    while (!readValues("Глобальные границы Xmin Xmax: ", cfg.Xmin, cfg.Xmax)) {
        cout << "Ошибка ввода. Повторите.\n";
    }
    while (!readValues("Глобальные границы Ymin Ymax: ", cfg.Ymin, cfg.Ymax)) {
        cout << "Ошибка ввода. Повторите.\n";
    }
    while (!readValues("Глобальные границы Zmin Zmax: ", cfg.Zmin, cfg.Zmax)) {
        cout << "Ошибка ввода. Повторите.\n";
    }

    while (!readValue("Порог E_threshold: ", cfg.threshold)) {
        cout << "Ошибка ввода. Повторите.\n";
    }

    while (!readValues("Шаги поиска (coarseStep fineStep): ", cfg.coarseStep, cfg.fineStep)) {
        cout << "Ошибка ввода. Повторите.\n";
    }

    while (!readValues("Шаги границы (borderCoarse borderFine): ", cfg.borderCoarse, cfg.borderFine)) {
        cout << "Ошибка ввода. Повторите.\n";
    }

    while (!readValue("Шаг детального расчёта (detailStep): ", cfg.detailStep)) {
        cout << "Ошибка ввода. Повторите.\n";
    }

    cout << "\nЗапуск поиска...\n";
    FieldSearcher searcher(cfg);
    vector<ERegion> regions = searcher.findAllRegions();

    cout << "\nНайдено областей: " << regions.size() << "\n\n";
    for (size_t i = 0; i < regions.size(); ++i) {
        const ERegion& r = regions[i];
        cout << "Область " << i+1 << ":\n";
        cout << fixed << setprecision(3);
        cout << "  X: [" << r.x_min << ", " << r.x_max << "]\n";
        cout << "  Y: [" << r.y_min << ", " << r.y_max << "]\n";
        cout << "  Z: [" << r.z_min << ", " << r.z_max << "]\n";
        cout << "  Сетка: " << r.nx << " x " << r.ny << " x " << r.nz
             << " = " << r.field.size() << " точек\n";
        cout << endl;
    }

    return 0;
}