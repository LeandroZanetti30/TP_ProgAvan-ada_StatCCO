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
    int attempts = 0;
    const int max_attempts = m_p.Nterm * 100;
    
    m_tree.initialize();
    
    cout << "Starting CCO with " << m_p.Nterm << " terminals, domain radius: " << m_p.domain_radius << endl;
    
    while (inserted < m_p.Nterm && attempts < max_attempts) {
        // Gera pontos em TODO o domínio, mas com mais peso na parte inferior
        double r = sqrt(dist(rng)) * m_p.domain_radius;
        double theta = dist(rng) * 2.0 * 3.141592653589793;
        double x = r * cos(theta);
        double y = r * sin(theta);
        
        // Dá preferência para parte inferior, mas não restringe totalmente
        if (dist(rng) > 0.3 && y > 0) {
            y = -y; // 70% de chance de inverter se estiver positivo
        }
        
        Vec2 terminal_pos(x, y);
        int nearest_seg = m_tree.nearest_segment_index(terminal_pos);
        
        if (nearest_seg >= 0) {
            double distance = m_tree.segment_distance(nearest_seg, terminal_pos);
            
            // REDUZI a distância mínima para algo mais razoável
            if (distance > 0.05) { // De 0.15 para 0.05
                int result = m_tree.add_terminal_to_segment(terminal_pos, nearest_seg);
                if (result != -1) {
                    inserted++;
                    if (inserted % 10 == 0) {
                        cout << "Progress: " << inserted << "/" << m_p.Nterm << " terminals" << endl;
                    }
                }
            }
        }
        attempts++;
        
        if (attempts % 1000 == 0) {
            cout << "Attempts: " << attempts << ", Inserted: " << inserted << endl;
        }
    }
    
    cout << "Final result: " << inserted << "/" << m_p.Nterm << " terminals inserted after " << attempts << " attempts." << endl;
    
    if (inserted < m_p.Nterm) {
        cout << "Try increasing domain_radius or decreasing Nterm for better results." << endl;
    }
    
    m_tree.compute_lengths();
    m_tree.compute_flows_and_radii(m_p.Q_perf, m_p.gamma);
    m_tree.compute_pressures(m_p.inlet_pressure, m_p.viscosity, m_p.terminal_pressure);
    
    bool ok = save_tree_csv(m_tree, out_csv);
    if (!ok) cerr << "Failed to save CSV: " << out_csv << endl;
    else cout << "Saved CSV: " << out_csv << endl;
    
    bool ok2 = save_tree_svg(m_tree, out_svg, 800, 800);
    if (!ok2) cerr << "Failed to save SVG: " << out_svg << endl;
    else cout << "Saved SVG: " << out_svg << endl;
}