#include <iosfwd>

#include <vector>

struct SvgImage {
  struct Point {
    int x, y;
  };

  struct Line {
    Point a, b;
  };

  std::vector<Line> lines;
};

void WriteSvg(const SvgImage& image, std::ostream& out);
