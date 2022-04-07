#pragma once

#include <iosfwd>
#include <vector>

struct Graph {
  struct Edge {
    int source;
    int target;
  };

  int num_nodes;
  std::vector<Edge> edges;
};

Graph ParseGraphML(std::istream& in);
