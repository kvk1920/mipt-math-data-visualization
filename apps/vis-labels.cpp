#include "datavis/common.hpp"
#include "datavis/svg.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <tuple>

constexpr int kSide = 500;

[[noreturn]]
void Exit(std::string msg) {
  std::cout << msg << std::endl;
  exit(EXIT_FAILURE);
}

struct Rectangle {
  int x, y, width, height;

  bool operator==(const Rectangle& that) const {
    return std::tie(x, y, width, height) == std::tie(that.x, that.y, that.width, that.height);
  }

  bool Check() const {
    return x >= 0 && y >= 0 && x <= kSide && y <= kSide;
  }
};

using Label = std::array<Rectangle, 2>;

std::vector<Label> ReadInput(const char* filename) {
  std::vector<Label> labels;
  std::ifstream in(filename);
  Rectangle rect{};
  int left_up, left_down, right_up, right_down;
  while (in >> rect.x >> rect.y >> rect.width >> rect.height >> left_up >>
         left_down >> right_up >> right_down) {
    VERIFY(left_up + left_down + right_up + right_down <= 2);
    VERIFY(left_up + left_down + right_up + right_down > 0);
    Label label;
    int pos = 0;
    auto add_rect = [&](Rectangle r) {
      if (r.Check()) {
        label[pos++] = r;
      }
    };
    if (left_up) {
      add_rect({rect.x - rect.width, rect.y - rect.height, rect.width, rect.height});
    }
    if (left_down) {
      add_rect({rect.x - rect.width, rect.y, rect.width, rect.height});
    }
    if (right_up) {
      add_rect({rect.x, rect.y - rect.height, rect.width, rect.height});
    }
    if (right_down) {
      add_rect(rect);
    }
    if (pos == 0) {
      Exit("Placement is unreachable");
    }
    if (pos == 1) {
      label[1] = label[0];
    }
    labels.push_back(label);
  }
  return labels;
}

std::vector<bool> Solve2Sat(int n, std::vector<std::pair<int, int>> rules) {
  class Dsu {
   public:
    explicit Dsu(int n) : p_(n), h_(n) {
      for (int i = 0; i < n; ++i) {
        p_[i] = i;
      }
    }

    int Find(int v) {
      while (p_[v] != v) {
        v = p_[v] = p_[p_[v]];
      }
      return v;
    }

    void Unite(int u, int v) {
      u = Find(u);
      v = Find(v);
      if (h_[u] < h_[v]) {
        std::swap(u, v);
      }
      p_[v] = u;
      if (h_[u] == h_[v]) {
        ++h_[u];
      }
    }

   private:
    std::vector<int> p_, h_;
  };

  Dsu dsu(2 * n);

  struct Node {
    int index = -1;
    int low_link = -1;
    int visited_children = 0;
    bool on_stack = false;
    int order;
    std::vector<int> e;
  };
  std::vector<Node> g(n * 2);
  for (auto [x, y] : rules) {
    g[x].e.push_back(y);
    g[y].e.push_back(x);
  }
  std::vector<int> stack;
  int index = 0;
  int order = 0;
  for (int i = 0; i < 2 * n; ++i) {
    if (g[i].index != -1) {
      continue;
    }
    stack.push_back(i);
    while (!stack.empty()) {
      int v = stack.back();
      if (g[v].visited_children == 0) {
        g[v].low_link = index;
        g[v].index = index++;
        g[v].on_stack = true;
      }
      if (g[v].visited_children < g[v].e.size()) {
        int u = g[v].e[g[v].visited_children++];
        if (g[u].index == -1) {
          stack.push_back(u);
        } else if (g[u].on_stack) {
          g[v].low_link = std::min(g[v].low_link, g[u].low_link);
        }
      } else {
        stack.pop_back();
        if (g[v].index == g[v].low_link) {

        }
        if (!stack.empty()) {
          int p = stack.back();
          g[p].low_link = std::min(g[p].low_link, g[v].low_link);
          if (g[v].index != g[v].low_link) {
            dsu.Unite(v, p);
          }
        }
        g[v].order = order++;
      }
    }
  }
  std::vector<bool> ans(n);
  for (int i = 0; i < n; ++i) {
    if (dsu.Find(i * 2) == dsu.Find(i * 2 + 1)) {
      Exit("Placement unreachable");
    }
    ans[i] = g[dsu.Find(i * 2)].order < g[dsu.Find(i * 2 + 1)].order;
  }
  return ans;
}

bool Intersects(const Rectangle& a, const Rectangle& b) {
  return !(a.x >= b.x + b.width
        || a.x + a.width <= b.x
        || a.y >= b.y + b.height
        || a.y + a.height <= b.y);
}

int main(int argc, char* argv[]) {
  VERIFY(argc == 3);
  auto labels = ReadInput(argv[1]);
  std::vector<std::pair<int, int>> rules;
  int n = static_cast<int>(labels.size());
  for (int i = 0; i < n; ++i) {
    if (labels[i][0] == labels[i][1]) {
      rules.emplace_back(i * 2, i * 2);
    }
    for (int j = i + 1; j < n; ++j) {
      for (int k : {0, 1}) {
        if (k && labels[i][0] == labels[i][1]) {
          continue;
        }
        for (int l : {0, 1}) {
          if (l && labels[j][0] == labels[j][1]) {
            continue;
          }
          if (Intersects(labels[i][k], labels[j][l])) {
            rules.emplace_back(i * 2 + !k, j * 2 + !l);
          }
        }
      }
    }
  }
  std::cout << std::boolalpha;
  int idx = 0;

  datavis::SvgImage image;
  image.fixed_size = {kSide, kSide};
  image.scale = {1, 1};
  for (auto x : Solve2Sat(n, rules)) {
    auto& rect = labels[idx++][!x];
    image.rects.push_back(datavis::SvgImage::Rect{
      {double(rect.x), double(rect.y)},
      {double(rect.width), double(rect.height)}});
  }
  std::ofstream result(argv[2]);
  image.Write(result);
}
