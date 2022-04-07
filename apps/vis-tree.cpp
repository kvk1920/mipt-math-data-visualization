#include <common.hpp>
#include <graphml.hpp>
#include <svg.hpp>

#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

struct Node {
  std::vector<Node*> children;
  int leftmost_x;
  int rightmost_x;
  int x;
  int y;
};

void CalculateTreePlacement(Node& node, int x_offset = 0, int level = 0) {
  node.y = level;
  node.leftmost_x = x_offset;
  if (node.children.empty()) {
    node.rightmost_x = x_offset;
    node.x = x_offset;
    return;
  }

  for (auto* child : node.children) {
    CalculateTreePlacement(*child, x_offset, level + 1);
    x_offset = child->rightmost_x + 2;
  }

  node.rightmost_x = node.children.back()->rightmost_x;
  node.x = (node.leftmost_x + node.rightmost_x) / 2;
}

int main(int argc, char** argv) {
  Graph g;
  VERIFY(argc == 3);
  {
    std::ifstream file(argv[1]);
    VERIFY(file.is_open());
    g = ParseGraphML(file);
  }
  std::vector<Node> nodes(g.num_nodes);
  std::unordered_set<int> maybe_root;
  for (int i = 0; i < g.num_nodes; ++i) {
    maybe_root.insert(i);
  }
  for (auto [source, target] : g.edges) {
    maybe_root.erase(target);
    nodes[source].children.push_back(&nodes[target]);
  }
  VERIFY(maybe_root.size() == 1);
  int root = *maybe_root.begin();
  CalculateTreePlacement(nodes[root]);

  std::ofstream out(argv[2]);
  SvgImage result;
  for (auto& node : nodes) {
    for (auto* child : node.children) {
      result.lines.push_back({{node.x, node.y},
                              {child->x, child->y}});
    }
  }
  std::ofstream result_file(argv[2]);
  WriteSvg(result, result_file);
}
