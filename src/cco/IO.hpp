#pragma once
#include "Tree.hpp"
#include <string>

using namespace std;

bool save_tree_csv(const Tree& tree, const string& filename);
bool save_tree_svg(const Tree& tree, const string& filename, int img_w = 800, int img_h = 800);