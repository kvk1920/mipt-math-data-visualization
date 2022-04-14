#include <iosfwd>

#include <vector>

namespace datavis {

struct SvgImage {
  struct Point {
    double x, y;
  };

  struct Line {
    Point a, b;
    bool with_arrow{true};
  };

  struct Circle {
    Point c;
    double r{2};
  };

  std::vector<Line> lines;
  std::vector<Circle> circles;
  double padding{5};
  Point scale{10, 20};

  void Write(std::ostream& out) const;
};

void WriteSvg(const SvgImage &image, std::ostream &out);

}  // namespace datavis
