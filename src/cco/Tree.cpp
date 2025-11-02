#include "Tree.hpp"
#include <limits>
#include <cmath>

using namespace std;

Tree::Tree() {
    Node root;
    root.id = 0;
    root.pos = Vec2(0, 0);
    root.parent = -1;
    m_nodes.push_back(root);
}

int Tree::add_node(const Vec2& p, int parent) {
    Node n;
    n.id = (int)m_nodes.size();
    n.pos = p;
    n.parent = parent;
    m_nodes.push_back(n);
    add_edge(parent, n.id);
    return n.id;
}

void Tree::add_edge(int parent, int child) {
    m_nodes[parent].children.push_back(child);
    Segment s;
    s.id = (int)m_segments.size();
    s.parent_node = parent;
    s.child_node = child;
    s.length = (m_nodes[child].pos - m_nodes[parent].pos).norm();
    s.radius = 0;
    s.flow = 0;
    m_segments.push_back(s);
}

const vector<Node>& Tree::nodes() const {
    return m_nodes;
}

const vector<Segment>& Tree::segments() const {
    return m_segments;
}

int Tree::nearest_node_index(const Vec2& p) const {
    double best = 1e300;
    int bi = 0;
    for (size_t i = 0; i < m_nodes.size(); ++i) {
        double d = (m_nodes[i].pos - p).norm();
        if (d < best) {
            best = d;
            bi = (int)i;
        }
    }
    return bi;
}

void Tree::compute_lengths() {
    for (auto &s : m_segments) {
        const Node &pn = m_nodes[s.parent_node];
        const Node &cn = m_nodes[s.child_node];
        s.length = (cn.pos - pn.pos).norm();
    }
}

int Tree::terminal_count() const {
    int c = 0;
    for (auto &n : m_nodes) {
        if (n.children.empty()) {
            c++;
        }
    }
    return c;
}

void Tree::compute_flows_and_radii(double Q_perf, double gamma) {
    for (auto &n : m_nodes) {
        n.flow = 0;
    }
    
    int terminals = terminal_count();
    if (terminals == 0) return;
    
    double Q_term = Q_perf / terminals;
    for (auto &n : m_nodes) {
        if (n.children.empty()) {
            n.flow = Q_term;
        }
    }
    
    for (int i = (int)m_nodes.size() - 1; i >= 0; --i) {
        Node &node = m_nodes[i];
        double sum = 0;
        for (int c : node.children) {
            sum += m_nodes[c].flow;
        }
        if (!node.children.empty()) {
            node.flow = sum;
        }
    }
    
    const double k = 0.02;
    for (auto &s : m_segments) {
        double flow = m_nodes[s.child_node].flow;
        s.flow = flow;
        s.radius = max(1e-6, k * pow(max(flow, 1e-12), 1.0 / gamma));
        m_nodes[s.child_node].radius = s.radius;
    }
    
    double root_pow = 0;
    for (int c : m_nodes[0].children) {
        root_pow += pow(m_nodes[c].radius, gamma);
    }
    m_nodes[0].radius = pow(root_pow, 1.0 / gamma);
}

void Tree::compute_pressures(double inlet_pressure, double viscosity, double terminal_pressure) {
    const double pi = 3.141592653589793;
    vector<double> R(m_segments.size());
    
    for (size_t i = 0; i < m_segments.size(); ++i) {
        auto &s = m_segments[i];
        double r = max(s.radius, 1e-9);
        double L = max(s.length, 1e-9);
        R[i] = (8.0 * viscosity * L) / (pi * pow(r, 4));
    }
    
    m_nodes[0].pressure_in = inlet_pressure;
    
    for (size_t i = 0; i < m_segments.size(); ++i) {
        auto &s = m_segments[i];
        const Node &pn = m_nodes[s.parent_node];
        Node &cn = m_nodes[s.child_node];
        cn.pressure_in = pn.pressure_in - s.flow * R[i];
        cn.pressure_out = cn.pressure_in;
    }
}