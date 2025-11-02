#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include "cco/CCOEngine.hpp"

using namespace std;

map<string, double> load_params(const string &filename) {
    map<string, double> out;
    ifstream f(filename);
    
    if (!f.is_open()) {
        return out;
    }
    
    string line;
    while (getline(f, line)) {
        size_t q1 = line.find('"');
        if (q1 == string::npos) {
            continue;
        }
        
        size_t q2 = line.find('"', q1 + 1);
        if (q2 == string::npos) {
            continue;
        }
        
        string key = line.substr(q1 + 1, q2 - q1 - 1);
        size_t colon = line.find(':', q2);
        
        if (colon == string::npos) {
            continue;
        }
        
        string val = line.substr(colon + 1);
        
        for (char &c : val) {
            if (c == ',') {
                c = ' ';
            }
        }
        
        stringstream ss(val);
        double v;
        
        if (ss >> v) {
            out[key] = v;
        }
    }
    
    return out;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "Usage: cco_tp1 params.json out.csv out.svg\n";
        return 1;
    }
    
    auto data = load_params(argv[1]);
    Params p;
    
    p.Q_perf = data.count("Q_perf") ? data["Q_perf"] : 8.33e-6;
    p.inlet_pressure = data.count("pressao_inlet") ? data["pressao_inlet"] : 13300.0;
    p.terminal_pressure = data.count("pressao_terminal") ? data["pressao_terminal"] : 7980.0;
    p.viscosity = data.count("viscosidade") ? data["viscosidade"] : 0.0036;
    p.Nterm = data.count("Nterm") ? static_cast<int>(data["Nterm"]) : 64;
    p.gamma = data.count("gamma") ? data["gamma"] : 3.0;
    p.domain_radius = data.count("domain_radius") ? data["domain_radius"] : 0.05;
    p.seed = data.count("seed") ? static_cast<int>(data["seed"]) : 42;
    
    CCOEngine engine(p);
    engine.run(argv[2], argv[3]);
    
    return 0;
}