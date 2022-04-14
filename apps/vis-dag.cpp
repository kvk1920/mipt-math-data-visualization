#include <datavis/common.hpp>
#include <datavis/graphml.hpp>
#include <datavis/svg.hpp>
#include <alglib/optimization.h>

#include <algorithm>
#include <fstream>
#include <memory>


datavis::Graph LoadGraph(const std::string& path) {
  std::ifstream input(path);
  VERIFY(input.is_open());
  return datavis::ParseGraphML(input);
}

class DAG {
 public:
  struct Node {
    std::vector<Node*> in, out;
    int layer, pos;
    bool dummy{false};
  };

  explicit DAG(datavis::Graph g) {
    nodes_.resize(g.num_nodes);
    for (auto [source, target] : g.edges) {
      nodes_[source].out.push_back(&nodes_[target]);
      nodes_[target].in.push_back(&nodes_[source]);
    }
    poses_.resize(nodes_.size(), 0);
  }

  void CoffmanGrahem(int w) {
    int n = static_cast<int>(nodes_.size());
    std::vector<int> label(n);
    std::vector<std::vector<int>> parent_labels(n);

    auto comparer = [&](int i, int j) -> bool {
      int x = static_cast<int>(parent_labels[i].size()) - 1;
      int y = static_cast<int>(parent_labels[j].size()) - 1;
      while (x >= 0 && y >= 0) {
        if (parent_labels[i][x] == parent_labels[j][y]) {
          --x;
          --y;
          continue;
        }
        return parent_labels[i][x] > parent_labels[j][y];
      }
      return x >= 0;
    };

    // Phase 1
    {
      std::vector<int> heap;
      heap.reserve(n);
      for (int i = 0; i < n; ++i) {
        if (nodes_[i].in.empty()) {
          heap.push_back(i);
          std::push_heap(heap.begin(), heap.end(), comparer);
        }
      }

      for (int i = 1; i <= n; ++i) {
        std::pop_heap(heap.begin(), heap.end(), comparer);
        int v = heap.back();
        heap.pop_back();
        label[v] = i;
        for (auto *nxt : nodes_[v].out) {
          int u = GetId(nxt);
          parent_labels[u].push_back(i);
          if (parent_labels[u].size() == nxt->in.size()) {
            heap.push_back(u);
            std::push_heap(heap.begin(), heap.end(), comparer);
          }
        }
      }
    }

    // Phase 2
    {
      std::vector<std::pair<int, int>> heap;
      std::vector<int> num_nxt_placed(n);
      heap.reserve(n);
      for (int i = 0; i < n; ++i) {
        if (nodes_[i].out.empty()) {
          heap.emplace_back(label[i], i);
          std::push_heap(heap.begin(), heap.end());
        }
      }

      int layer = 0, cnt = 0;
      auto& pos = poses_;
      for (int i = 0; i < n; ++i) {
        VERIFY(!heap.empty());
        std::pop_heap(heap.begin(), heap.end());
        int v = heap.back().second;
        heap.pop_back();
        bool need_new_layer = false;
        for (auto* u : nodes_[v].out) {
          if (u->layer == layer) {
            need_new_layer = true;
            break;
          }
        }
        if (need_new_layer) {
          ++layer;
          cnt = 0;
        }
        nodes_[v].layer = layer;
        nodes_[v].pos = pos[layer]++;
        ++cnt;
        for (auto* prv : nodes_[v].in) {
          int u = GetId(prv);
          if (++num_nxt_placed[u] == prv->out.size()) {
            heap.emplace_back(label[u], u);
            std::push_heap(heap.begin(), heap.end());
          }
        }
        if (cnt == w) {
          ++layer;
          cnt = 0;
        }
      }
    }
    AddDummies();
  }

  void Save(const char* file) const {
    std::ofstream out(file);
    VERIFY(out.is_open());
    datavis::SvgImage image;
    for (auto& v : nodes_) {
      for (auto* nxt : v.out) {
        image.lines.push_back({{
          static_cast<double>(v.pos),
          static_cast<double>(v.layer)
          }, {
          static_cast<double>(nxt->pos), static_cast<double>(nxt->layer)
          }, !nxt->dummy});
      }
    }
    for (auto& v : dummy_nodes_) {
      for (auto* nxt : v->out) {
        image.lines.push_back({{
                                   static_cast<double>(v->pos),
                                   static_cast<double>(v->layer)
                               }, {
                                   static_cast<double>(nxt->pos), static_cast<double>(nxt->layer)
                               }, !nxt->dummy});
      }
    }
    for (const auto& v : nodes_) {
      image.circles.push_back({{static_cast<double>(v.pos), static_cast<double>(v.layer)}});
    }
    image.Write(out);
  }

  void MinimizeDummyNodes() {
    alglib::minlpstate state;
    // Last dimension is constant
    alglib::minlpcreate(static_cast<long>(nodes_.size()), state);
    int n = static_cast<int>(nodes_.size());
    {
      alglib::real_1d_array cost;
      cost.setlength(n);
      for (int i = 0; i < n; ++i) {
        cost[i] = 0;
      }
      for (int i = 0; i < n; ++i) {
        for (auto* j : nodes_[i].out) {
          cost[GetId(j)] += 1;
          cost[i] -= 1;
          alglib::integer_1d_array idx;
          idx.setlength(2);
          idx[0] = i;
          idx[1] = GetId(j);
          alglib::real_1d_array coef;
          coef.setlength(2);
          coef[0] = -1;
          coef[1] = 1;
          alglib::minlpaddlc2(state, idx, coef, 2, 1.01, alglib::fp_posinf);
        }
      }
      alglib::minlpsetcost(state, cost);
    }
    alglib::minlpsetbcall(state, 0, n);
    alglib::minlpoptimize(state);
    alglib::real_1d_array res;
    alglib::minlpreport rep;
    alglib::minlpresults(state, res, rep);
    auto& pos = poses_;
    int min_layer = n;
    for (int i = 0; i < n; ++i) {
      int layer = static_cast<int>(std::round(res[i]));
      min_layer = std::min(min_layer, layer);
      nodes_[i].layer = layer;
      nodes_[i].pos = pos[layer]++;
    }
    for (auto& v : nodes_) {
      v.layer -= min_layer;
    }
    InvertLayers();
    AddDummies();
  }

 private:
  int GetId(Node* v) {
    return static_cast<int>(v - nodes_.data());
  }

  Node* CreateDummy() {
    return dummy_nodes_.emplace_back(std::make_unique<Node>()).get();
  }

  void InvertLayers() {
    int max_layer = 0;
    for (auto& v : nodes_) {
      max_layer = std::max(v.layer, max_layer);
    }
    for (auto& v : nodes_) {
      v.layer = max_layer - v.layer;
    }
  }

  void AddDummies() {
    int n = static_cast<int>(nodes_.size());
    for (int v = 0; v < n; ++v) {
      for (auto &nxt : nodes_[v].out) {
        int nxt_layer = nxt->layer;
        for (int dummy_layer = nxt_layer + 1; dummy_layer < nodes_[v].layer; ++dummy_layer) {
          auto dummy = CreateDummy();
          dummy->layer = dummy_layer;
          dummy->dummy = true;
          dummy->pos = poses_[dummy_layer]++;
          dummy->out = {nxt};
          nxt = dummy;
        }
      }
    }
  }

 private:
  std::vector<Node> nodes_;
  std::vector<int> poses_;
  std::vector<std::unique_ptr<Node>> dummy_nodes_;
};

int main(int argc, char** argv) {
  VERIFY(argc == 4 || argc == 3);
  DAG g(LoadGraph(argv[1]));
  if (argc == 4) {
    g.CoffmanGrahem(std::stoi(argv[3]));
  } else {
    g.MinimizeDummyNodes();
  }
  g.Save(argv[2]);
}
