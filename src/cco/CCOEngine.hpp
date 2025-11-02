#pragma once
#include "Tree.hpp"
#include <string>

using namespace std;

struct Params {
    double Q_perf;
    double inlet_pressure;
    double terminal_pressure;
    double viscosity;
    int Nterm;
    double gamma;
    double domain_radius;
    int seed;
};

class CCOEngine {
public:
    CCOEngine(const Params& p);
    void run(const string& out_csv, const string& out_svg);

private:
    Params m_p;
    Tree m_tree;
};