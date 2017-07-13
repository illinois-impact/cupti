#include "api_record.hpp"

#include <cassert>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;
using boost::property_tree::write_json;

const ApiRecord::id_type ApiRecord::noid =
    reinterpret_cast<ApiRecord::id_type>(nullptr);

void ApiRecord::add_input(const Value::id_type &id) {
  assert(Values::noid != id);
  inputs_.push_back(id);
}

void ApiRecord::add_output(const Value::id_type &id) {
  assert(Values::noid != id);
  outputs_.push_back(id);
}

static ptree to_json(const std::vector<Values::id_type> &v) {
  ptree array;
  for (const auto &e : v) {
    ptree elem;
    elem.put("", e);
    array.push_back(std::make_pair("", elem));
  }
  return array;
}

std::string ApiRecord::json() const {
  ptree pt;
  pt.put("api.id", std::to_string(Id()));
  pt.put("api.name", name_);
  pt.add_child("api.inputs", to_json(inputs_));
  pt.add_child("api.outputs", to_json(outputs_));
  std::ostringstream buf;
  write_json(buf, pt, false);
  return buf.str();
}

std::ostream &operator<<(std::ostream &os, const ApiRecord &r) {
  os << r.json();
  return os;
}