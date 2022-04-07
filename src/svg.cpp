#include "svg.hpp"

#include <pugixml.hpp>

#include <algorithm>

class MinAggregator {
 public:
  void Update(int value) {
    value_ = std::min(value_, value);
  }

  int Value() const {
    return value_;
  }
 private:
  int value_{std::numeric_limits<int>::max()};
};

class MaxAggregator {
 public:
  void Update(int value) {
    value_ = std::max(value_, value);
  }

  int Value() const {
    return value_;
  }
 private:
  int value_{std::numeric_limits<int>::min()};
};

constexpr int kPadding = 5;
constexpr int kScaleX = 10;
constexpr int kScaleY = 20;

void WriteSvg(const SvgImage& image, std::ostream& out) {
  pugi::xml_document doc;
  auto svg = doc.append_child("svg");
  svg.append_attribute("xmlns").set_value("http://www.w3.org/2000/svg");
  svg.append_attribute("version").set_value("1.0");
  MinAggregator min_x, min_y;
  MaxAggregator max_x, max_y;
  for (auto line : image.lines) {
    min_x.Update(line.a.x * kScaleX);
    min_x.Update(line.b.x * kScaleX);

    max_x.Update(line.a.x * kScaleX);
    max_x.Update(line.b.x * kScaleX);

    min_y.Update(line.a.y * kScaleY);
    min_y.Update(line.b.y * kScaleY);

    max_y.Update(line.a.y * kScaleY);
    max_y.Update(line.b.y * kScaleY);
  }
  auto background = svg.append_child("rect");
  background.append_attribute("x").set_value(min_x.Value());
  background.append_attribute("width").set_value(max_x.Value() - min_x.Value() + kPadding * 2);
  background.append_attribute("y").set_value(min_y.Value());
  background.append_attribute("height").set_value(max_y.Value() - min_y.Value() + kPadding * 2);
  background.append_attribute("fill").set_value("white");
  for (auto line : image.lines) {
    auto record = svg.append_child("line");
    record.append_attribute("x1").set_value(kPadding + line.a.x * kScaleX);
    record.append_attribute("x2").set_value(kPadding + line.b.x * kScaleX);
    record.append_attribute("y1").set_value(kPadding + line.a.y * kScaleY);
    record.append_attribute("y2").set_value(kPadding + line.b.y * kScaleY);
    record.append_attribute("stroke-width").set_value("0.1");
    record.append_attribute("stroke").set_value("black");
    for (auto p : {line.a, line.b}) {
      auto circle = svg.append_child("circle");
      circle.append_attribute("cx").set_value(kPadding + p.x * kScaleX);
      circle.append_attribute("cy").set_value(kPadding + p.y * kScaleY);
      circle.append_attribute("r").set_value("2");
      circle.append_attribute("fill").set_value("red");
      circle.append_attribute("stroke").set_value("black");
      circle.append_attribute("stroke-width").set_value("0.2");
    }
  }
  svg.append_attribute("width").set_value(max_x.Value() + 2 * kPadding);
  svg.append_attribute("height").set_value(max_y.Value() + 2 * kPadding);
  doc.save(out);
}
