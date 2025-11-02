#include "IO.hpp"
#include <fstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

using namespace std;

static void map_coord(double x, double y, double R, int W, int H, double &ox, double &oy) {
    double sx = (x + R) / (2.0 * R);
    double sy = (y + R) / (2.0 * R);
    ox = sx * W;
    oy = H - (sy * H);
}

bool save_tree_csv(const Tree& tree, const string& filename) {
    ofstream f(filename);
    if (!f.is_open()) return false;
    
    f << "segment_id,parent_id,child_id,x_parent,y_parent,x_child,y_child,length,radius,flow,pressure_in,pressure_out,depth\n";
    
    auto &nodes = tree.nodes();
    auto &segs = tree.segments();
    vector<int> depth(nodes.size(), 0);
    
    for (size_t i = 1; i < nodes.size(); ++i) {
        int p = nodes[i].parent;
        depth[i] = (p >= 0) ? depth[p] + 1 : 0;
    }
    
    for (const auto &s : segs) {
        auto &pn = nodes[s.parent_node];
        auto &cn = nodes[s.child_node];
        
        f << s.id << "," << s.parent_node << "," << s.child_node << "," << fixed << setprecision(6)
          << pn.pos.x << "," << pn.pos.y << "," << cn.pos.x << "," << cn.pos.y << ","
          << s.length << "," << s.radius << "," << s.flow << "," << pn.pressure_in << "," << cn.pressure_in << "," << depth[s.child_node] << "\n";
    }
    
    return true;
}

bool save_tree_svg(const Tree& tree, const string& filename, int img_w, int img_h) {
    ofstream f(filename);
    if (!f.is_open()) return false;
    
    auto &nodes = tree.nodes();
    auto &segs = tree.segments();
    double R = 0;
    
    for (auto &n : nodes) R = max(R, abs(n.pos.x));
    for (auto &n : nodes) R = max(R, abs(n.pos.y));
    if (R < 1e-9) R = 0.05;
    
    f << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
    f << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"" << img_w << "\" height=\"" << img_h << "\">\n";
    f << "<rect width=\"100%\" height=\"100%\" fill=\"white\" />\n";
    
    for (const auto &s : segs) {
        auto &pn = nodes[s.parent_node];
        auto &cn = nodes[s.child_node];
        double x1, y1, x2, y2;
        map_coord(pn.pos.x, pn.pos.y, R, img_w, img_h, x1, y1);
        map_coord(cn.pos.x, cn.pos.y, R, img_w, img_h, x2, y2);
        
        double sw = max(0.3, s.radius * 80.0);
        f << "<line x1=\"" << x1 << "\" y1=\"" << y1 << "\" x2=\"" << x2 << "\" y2=\"" << y2 
          << "\" stroke=\"black\" stroke-width=\"" << sw << "\" stroke-linecap=\"round\" />\n";
    }
    
    for (const auto &n : nodes) {
        double x, y;
        map_coord(n.pos.x, n.pos.y, R, img_w, img_h, x, y);
        
        if (n.parent == -1) {
            f << "<circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"4\" fill=\"red\" />\n";
        } else if (n.children.empty()) {
            f << "<circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"3\" fill=\"blue\" />\n";
        } else {
            f << "<circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"2\" fill=\"black\" />\n";
        }
    }
    
    f << "</svg>\n";
    return true;
}