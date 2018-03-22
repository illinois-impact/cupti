#ifndef CPROF_VALUE_HPP
#define CPROF_VALUE_HPP

#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include <nlohmann/json.hpp>

#include "cprof/address_space.hpp"
#include "util/extent.hpp"

namespace cprof {

class Value : public Extent {

  using json = nlohmann::json;

private:
  friend class Allocations;
  friend class Allocation;
  size_t allocation_;
  AddressSpace addressSpace_;
  bool initialized_;
  size_t id_;

public:
  Value(size_t id, const uintptr_t pos, const size_t size,
        const size_t &allocation, const AddressSpace &addressSpace,
        const bool initialized)
      : Extent(pos, size), allocation_(allocation), addressSpace_(addressSpace),
        initialized_(initialized), id_(id) {}
  Value() : Value(0, 0, 0, 0, AddressSpace::Unknown(), false) {}

  json to_json() const;
  std::string to_json_string() const;

  explicit operator bool() const noexcept { return id_ != 0; }
  size_t id() const { return id_; }
  const AddressSpace &address_space() const noexcept;
  void set_size(const size_t size);
  bool initialized() const { return initialized_; }
  bool operator==(const Value &rhs) const;
  bool operator!=(const Value &rhs) const;
};

void to_json(nlohmann::json &j, const Value &v);
} // namespace cprof

std::ostream &operator<<(std::ostream &os, const cprof::Value &dt);

#endif
