#include "svg.hpp"

#include <pugixml.hpp>

#include <algorithm>

namespace datavis {

void SvgImage::Write(std::ostream& out) const {
  pugi::xml_document doc;
  auto svg = doc.append_child("svg");
  svg.append_attribute("xmlns").set_value("http://www.w3.org/2000/svg");
  svg.append_attribute("version").set_value("1.0");
  auto min_x = std::numeric_limits<double>::infinity();
  auto min_y = min_x;
  auto max_x = -min_x;
  auto max_y = -min_y;
  bool use_arrow = false;

  auto update_minmax = [&](SvgImage::Point p) {
    min_x = std::min(min_x, p.x);
    max_x = std::max(max_x, p.x);
    min_y = std::min(min_y, p.y);
    max_y = std::max(max_y, p.y);
  };

  for (auto line : lines) {
    update_minmax(line.a);
    update_minmax(line.b);
    use_arrow |= line.with_arrow;
  }

  if (use_arrow) {
    // https://www.thenewcode.com/1068/Making-Arrows-in-SVG
    auto arrow = svg.append_child("defs").append_child("marker");
    arrow.append_attribute("id").set_value("arrowhead");
    arrow.append_attribute("markerWidth").set_value(20);
    arrow.append_attribute("markerHeight").set_value(20);
    arrow.append_attribute("refX").set_value(40);
    arrow.append_attribute("refY").set_value(10);
    arrow.append_attribute("orient").set_value("auto");
    auto polygon = arrow.append_child("polygon");
    polygon.append_attribute("points").set_value("0 0, 20 10, 0 20");
  }

  for (auto circle : circles) {
    update_minmax({circle.c.x, circle.c.y});
  }

  auto background = svg.append_child("rect");
  background.append_attribute("x").set_value(min_x * scale.x);
  background.append_attribute("width").set_value((max_x - min_x) * scale.x + padding * 2);
  background.append_attribute("y").set_value(min_y * scale.y);
  background.append_attribute("height").set_value((max_y - min_y) * scale.y + padding * 2);
  background.append_attribute("fill").set_value("white");

  for (auto line : lines) {
    auto record = svg.append_child("line");
    record.append_attribute("x1").set_value(padding + line.a.x * scale.x);
    record.append_attribute("x2").set_value(padding + line.b.x * scale.x);
    record.append_attribute("y1").set_value(padding + line.a.y * scale.y);
    record.append_attribute("y2").set_value(padding + line.b.y * scale.y);
    record.append_attribute("stroke-width").set_value(0.1);
    record.append_attribute("stroke").set_value("black");
    if (line.with_arrow) {
      record.append_attribute("marker-end").set_value("url(#arrowhead)");
    }
  }

  for (auto circle : circles) {
    auto record = svg.append_child("circle");
    record.append_attribute("cx").set_value(padding + circle.c.x * scale.x);
    record.append_attribute("cy").set_value(padding + circle.c.y * scale.y);
    record.append_attribute("r").set_value(2);
    record.append_attribute("fill").set_value("red");
    record.append_attribute("stroke").set_value("black");
    record.append_attribute("stroke-width").set_value(0.2);
  }

  svg.append_attribute("width").set_value((max_x - min_x) * scale.x + 2 * padding);
  svg.append_attribute("height").set_value((max_y - min_y) * scale.y + 2 * padding);
  doc.save(out);
}

}
