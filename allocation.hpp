#ifndef ALLOCATION_HPP
#define ALLOCATION_HPP

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>

#include "extent.hpp"
#include "location.hpp"

class Allocation : public Extent {
public:
  enum class Type { Pinned, Pageable, Unknown };
  typedef uintptr_t id_type;

private:
  Location location_;
  Type type_;

public:
  friend std::ostream &operator<<(std::ostream &os, const Allocation &v);
  Allocation(uintptr_t pos, size_t size, Location loc, Type type)
      : Extent(pos, size), location_(loc), type_(type) {}

  std::string json() const;

  bool overlaps(const Allocation &other) {
    return location_.overlaps(other.location_) && Extent::overlaps(other);
  }

  bool contains(const Allocation &other) {
    return location_.overlaps(other.location_) && Extent::contains(other);
  }

  id_type Id() const { return reinterpret_cast<id_type>(this); }
  Location location() const { return location_; }
};

#endif
