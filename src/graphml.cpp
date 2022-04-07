#include "graphml.hpp"

#include "common.hpp"

#include <pugixml.hpp>

#include <string_view>
#include <unordered_map>

using namespace std::literals;

Graph ParseGraphML(std::istream& in) {
  pugi::xml_document doc;
  VERIFY(doc.load(in));
  auto root = doc.child("graphml");
  VERIFY(!root.empty());
  auto graph = root.child("graph");
  VERIFY(graph.attribute("edgedefault").value() == "directed"sv);
  Graph result{0};
  std::unordered_map<std::string, int> name_to_id;
  for (const auto& record : graph.children()) {
    if (record.name() == "node"sv) {
      name_to_id[record.attribute("id").value()] = result.num_nodes++;
    } else if (record.name() == "edge"sv) {
      int source = name_to_id.at(record.attribute("source").value());
      int target = name_to_id.at(record.attribute("target").value());
      result.edges.push_back({source, target});
    } else {
      VERIFY(false);
    }
  }
  return result;
}
