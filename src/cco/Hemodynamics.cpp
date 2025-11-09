#include "Hemodynamics.hpp"
#include <cmath>

using namespace std;

// Calcula a resistência hidrodinâmica usando a lei de Poiseuille
double resistance_poisseuille(double viscosity, double length, double radius) {
    const double pi = 3.141592653589793;
    double r = max(radius, 1e-12);   // evita divisão por zero (raio mínimo)
    double L = max(length, 1e-12);   // evita divisão por zero (comprimento mínimo)
    return (8.0 * viscosity * L) / (pi * pow(r, 4)); // fórmula da resistência
}
