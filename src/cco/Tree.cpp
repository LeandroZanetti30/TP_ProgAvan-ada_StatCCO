#include "Tree.hpp"
#include <limits>
#include <cmath>
#include <algorithm>

using namespace std;

Tree::Tree() {
    Node root;
    root.id = 0;
    root.pos = Vec2(0.0, 1.0);
    root.parent = -1;
    root.is_terminal = false;
    m_nodes.push_back(root);
}

void Tree::initialize() {
    m_nodes[0].pos = Vec2(0.0, 1.0);
    
    Node first_terminal;
    first_terminal.id = (int)m_nodes.size();
    first_terminal.pos = Vec2(0.0, -0.9);
    first_terminal.parent = 0;
    first_terminal.is_terminal = true;
    m_nodes.push_back(first_terminal);
    
    Segment s;
    s.id = (int)m_segments.size();
    s.parent_node = 0;
    s.child_node = 1;
    s.length = (first_terminal.pos - m_nodes[0].pos).norm();
    m_segments.push_back(s);
    
    m_nodes[0].children.push_back(first_terminal.id);
    m_nodes[0].segments.push_back(s.id);
    m_nodes[first_terminal.id].segments.push_back(s.id);
}

int Tree::add_terminal_to_segment(const Vec2& terminal_pos, int segment_id) {
    if (segment_id < 0 || segment_id >= (int)m_segments.size()) {
        return -1;
    }
    
    Segment& old_segment = m_segments[segment_id];
    int old_parent_id = old_segment.parent_node;
    int old_child_id = old_segment.child_node;

    Vec2 connection_point = project_point_on_segment(old_segment, terminal_pos);
    
    // REDUZI as distâncias mínimas
    double min_distance = 0.03; // De 0.1 para 0.03
    
    // Verifica se terminal_pos está muito perto de qualquer nó existente
    for (const auto& node : m_nodes) {
        double dist = (node.pos - terminal_pos).norm();
        if (dist < min_distance) {
            return -1;
        }
    }
    
    // Verifica se connection_point está muito perto de qualquer nó existente
    for (const auto& node : m_nodes) {
        double dist = (node.pos - connection_point).norm();
        if (dist < min_distance) {
            return -1;
        }
    }
    
    double distance_to_segment = (terminal_pos - connection_point).norm();
    if (distance_to_segment < min_distance) {
        return -1;
    }
    
    // SÓ CRIA SE PASSAR EM TODAS AS VERIFICAÇÕES
    
    // 1. Cria nó preto
    Node black_node;
    black_node.id = (int)m_nodes.size();
    black_node.pos = connection_point;
    black_node.parent = old_parent_id;
    black_node.is_terminal = false;
    m_nodes.push_back(black_node);
    int black_node_id = black_node.id;

    // 2. Cria nó azul
    Node blue_node;
    blue_node.id = (int)m_nodes.size();
    blue_node.pos = terminal_pos;
    blue_node.parent = black_node_id;
    blue_node.is_terminal = true;
    m_nodes.push_back(blue_node);
    int blue_node_id = blue_node.id;

    // 3. Atualiza segmento antigo
    old_segment.child_node = black_node_id;
    old_segment.length = (connection_point - m_nodes[old_parent_id].pos).norm();

    // 4. Reconecta filho antigo se existir
    if (old_child_id != -1) {
        Segment seg_to_old_child;
        seg_to_old_child.id = (int)m_segments.size();
        seg_to_old_child.parent_node = black_node_id;
        seg_to_old_child.child_node = old_child_id;
        seg_to_old_child.length = (m_nodes[old_child_id].pos - connection_point).norm();
        m_segments.push_back(seg_to_old_child);
        
        m_nodes[old_child_id].parent = black_node_id;
        m_nodes[black_node_id].children.push_back(old_child_id);
        m_nodes[black_node_id].segments.push_back(seg_to_old_child.id);
        m_nodes[old_child_id].segments.push_back(seg_to_old_child.id);
    }

    // 5. Cria segmento para o novo azul
    Segment seg_to_blue;
    seg_to_blue.id = (int)m_segments.size();
    seg_to_blue.parent_node = black_node_id;
    seg_to_blue.child_node = blue_node_id;
    seg_to_blue.length = distance_to_segment;
    m_segments.push_back(seg_to_blue);

    // 6. Atualiza conexões
    m_nodes[old_parent_id].children.push_back(black_node_id);
    m_nodes[black_node_id].children.push_back(blue_node_id);

    m_nodes[black_node_id].segments.push_back(old_segment.id);
    m_nodes[black_node_id].segments.push_back(seg_to_blue.id);
    m_nodes[blue_node_id].segments.push_back(seg_to_blue.id);

    // Remove conexão antiga
    auto& parent_children = m_nodes[old_parent_id].children;
    auto it = find(parent_children.begin(), parent_children.end(), old_child_id);
    if (it != parent_children.end()) {
        parent_children.erase(it);
    }

    return blue_node_id;
}

// ... (o resto das funções permanece igual)
const vector<Node>& Tree::nodes() const {
    return m_nodes;
}

const vector<Segment>& Tree::segments() const {
    return m_segments;
}

double Tree::segment_distance_private(const Segment& s, const Vec2& p) const {
    const Vec2& a = m_nodes[s.parent_node].pos;
    const Vec2& b = m_nodes[s.child_node].pos;
    Vec2 ab = b - a;
    Vec2 ap = p - a;
    
    double t = (ap.x * ab.x + ap.y * ab.y) / (ab.x * ab.x + ab.y * ab.y);
    t = max(0.0, min(1.0, t));
    
    Vec2 projection = a + ab * t;
    return (p - projection).norm();
}

double Tree::segment_distance(int segment_id, const Vec2& p) const {
    if (segment_id < 0 || segment_id >= (int)m_segments.size()) {
        return 1e300;
    }
    return segment_distance_private(m_segments[segment_id], p);
}

Vec2 Tree::project_point_on_segment(const Segment& s, const Vec2& p) const {
    const Vec2& a = m_nodes[s.parent_node].pos;
    const Vec2& b = m_nodes[s.child_node].pos;
    Vec2 ab = b - a;
    Vec2 ap = p - a;
    
    double t = (ap.x * ab.x + ap.y * ab.y) / (ab.x * ab.x + ab.y * ab.y);
    t = max(0.0, min(1.0, t));
    
    return a + ab * t;
}

int Tree::nearest_segment_index(const Vec2& p) const {
    double best_dist = 1e300;
    int best_segment = 0;
    
    for (const auto& s : m_segments) {
        double d = segment_distance_private(s, p);
        if (d < best_dist) {
            best_dist = d;
            best_segment = s.id;
        }
    }
    
    return best_segment;
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
        if (n.is_terminal) c++;
    }
    return c;
}

void Tree::compute_flows_and_radii(double Q_perf, double gamma) {
    for (auto &n : m_nodes) n.flow = 0;

    int terminals = terminal_count();
    if (terminals == 0) return;

    double Q_term = Q_perf / terminals;

    for (auto &n : m_nodes)
        if (n.is_terminal) n.flow = Q_term;

    for (int i = (int)m_nodes.size() - 1; i >= 0; --i) {
        Node &node = m_nodes[i];
        double sum = 0;
        for (int c : node.children)
            sum += m_nodes[c].flow;
        if (!node.children.empty())
            node.flow = sum;
    }

    const double k = 0.02;
    for (auto &s : m_segments) {
        double flow = m_nodes[s.child_node].flow;
        s.flow = flow;
        s.radius = max(1e-6, k * pow(max(flow, 1e-12), 1.0 / gamma));
        m_nodes[s.child_node].radius = s.radius;
    }

    double root_pow = 0;
    for (int c : m_nodes[0].children)
        root_pow += pow(m_nodes[c].radius, gamma);
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