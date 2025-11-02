#pragma once
#include "Geometry.hpp"
#include <vector>

using namespace std;

struct Node {
    int id;
    Vec2 pos;
    int parent;
    vector<int> children;
    double flow;
    double radius;
    double pressure_in;
    double pressure_out;
    
    Node() : id(-1), pos(), parent(-1), flow(0), radius(0), pressure_in(0), pressure_out(0) {}
};

struct Segment {
    int id;
    int parent_node;
    int child_node;
    double length;
    double radius;
    double flow;
};

class Tree {
public:
    Tree();
    int add_node(const Vec2& p, int parent);
    void add_edge(int parent, int child);
    const vector<Node>& nodes() const;
    const vector<Segment>& segments() const;
    int nearest_node_index(const Vec2& p) const;
    void compute_lengths();
    void compute_flows_and_radii(double Q_perf, double gamma);
    void compute_pressures(double inlet_pressure, double viscosity, double terminal_pressure);
    int terminal_count() const;

private:
    vector<Node> m_nodes;
    vector<Segment> m_segments;
};