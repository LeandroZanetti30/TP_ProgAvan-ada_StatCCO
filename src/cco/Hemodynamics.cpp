#include "Hemodynamics.hpp"
#include <cmath>

using namespace std;

double resistance_poisseuille(double viscosity, double length, double radius) {
    const double pi = 3.141592653589793;
    double r = max(radius, 1e-12);
    double L = max(length, 1e-12);
    return (8.0 * viscosity * L) / (pi * pow(r, 4));
}