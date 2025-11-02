#include "CCOEngine.hpp"
#include <random>
#include <iostream>
#include "IO.hpp"

using namespace std;

CCOEngine::CCOEngine(const Params& p) : m_p(p) {}

void CCOEngine::run(const string& out_csv, const string& out_svg) {
    mt19937 rng(m_p.seed);
    uniform_real_distribution<double> dist(0.0, 1.0);
    int inserted = 0;
    
    while (inserted < m_p.Nterm) {
        double r = sqrt(dist(rng)) * m_p.domain_radius;
        double theta = dist(rng) * 2.0 * 3.141592653589793;
        double x = r * cos(theta);
        double y = r * sin(theta);
        int near = m_tree.nearest_node_index({x, y});
        m_tree.add_node({x, y}, near);
        inserted++;
    }
    
    m_tree.compute_lengths();
    m_tree.compute_flows_and_radii(m_p.Q_perf, m_p.gamma);
    m_tree.compute_pressures(m_p.inlet_pressure, m_p.viscosity, m_p.terminal_pressure);
    
    bool ok = save_tree_csv(m_tree, out_csv);
    if (!ok) {
        cerr << "Failed to save CSV: " << out_csv << endl;
    } else {
        cout << "Saved CSV: " << out_csv << endl;
    }
    
    bool ok2 = save_tree_svg(m_tree, out_svg, 800, 800);
    if (!ok2) {
        cerr << "Failed to save SVG: " << out_svg << endl;
    } else {
        cout << "Saved SVG: " << out_svg << endl;
    }
}