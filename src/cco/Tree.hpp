#pragma once
#include "Geometry.hpp"
#include <vector>

using namespace std;

struct Node {
    int id;
    Vec2 pos;
    int parent;
    vector<int> children;
    vector<int> segments;
    double flow;
    double radius;
    double pressure_in;
    double pressure_out;
    bool is_terminal;
    
    Node() : id(-1), pos(), parent(-1), flow(0), radius(0), pressure_in(0), pressure_out(0), is_terminal(false) {}
};

struct Segment {
    int id;
    int parent_node;
    int child_node;
    double length;
    double radius;
    double flow;
    
    Segment() : id(-1), parent_node(-1), child_node(-1), length(0), radius(0), flow(0) {}
};

class Tree {
public:
    Tree();
    
    int add_terminal_to_segment(const Vec2& terminal_pos, int segment_id);
    const vector<Node>& nodes() const;
    const vector<Segment>& segments() const;
    int nearest_segment_index(const Vec2& p) const;
    double segment_distance(int segment_id, const Vec2& p) const;
    void compute_lengths();
    void compute_flows_and_radii(double Q_perf, double gamma);
    void compute_pressures(double inlet_pressure, double viscosity, double terminal_pressure);
    int terminal_count() const;
    void initialize();

private:
    vector<Node> m_nodes;
    vector<Segment> m_segments;
    
    void add_edge(int parent, int child);
    double segment_distance_private(const Segment& s, const Vec2& p) const;
    Vec2 project_point_on_segment(const Segment& s, const Vec2& p) const;
};