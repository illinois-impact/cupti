#include <cassert>

#include "cprof/allocations.hpp"

using cprof::model::Location;
using cprof::model::Memory;

namespace cprof {

Allocation Allocations::unsafe_find(uintptr_t pos, size_t size,
                                    const AddressSpace &as) {
  assert(pos && "No allocations at null pointer");
  auto allocationsIter = addrSpaceAllocs_.find(as);
  if (allocationsIter != addrSpaceAllocs_.end()) {
    auto &allocations = allocationsIter->second;

    // std::cerr << "(unsafe_find) looking for {" << as.str() << "}[" << pos
    //           << ", +" << size << ")\n";
    // for (auto &e : allocations) {
    //   std::cerr << e.first.lower() << " " << e.first.upper() << "\n";
    // }
    auto ai = allocations.find(pos, size);
    if (ai != allocations.end()) {
      // std::cerr << "matching allocation at " << ai->second.pos() << "\n";
      return Allocation(*ai);
    } else {
      // std::cerr << "no matching alloc\n";
      return Allocation();
    }
  } else {
    // std::cerr << "no matching AS\n";
    return Allocation();
  }
}

size_t Allocations::free(uintptr_t pos, const AddressSpace &as) {
  assert(pos && "No allocations at null pointer");
  std::lock_guard<std::mutex> guard(access_mutex_);
  auto allocationsIter = addrSpaceAllocs_.find(as);
  if (allocationsIter != addrSpaceAllocs_.end()) {
    auto &allocations = allocationsIter->second;
    return allocations.erase(pos);
  } else {
    assert(0 && "Expected address space");
  }
  return 0;
}

Allocation Allocations::insert(const AllocationRecord &ar) {

  auto &allocs = addrSpaceAllocs_[ar.address_space()];
  auto i = allocs.insert_join(ar).first;
  logging::atomic_out(i->second->to_json_string() + "\n");
  return Allocation(*i);
}

Allocation Allocations::new_allocation(uintptr_t pos, size_t size,
                                       const AddressSpace &as, const Memory &am,
                                       const Location &al) {
  if (size == 0) {
    logging::err() << "WARN: creating size 0 allocation" << std::endl;
  }
  std::lock_guard<std::mutex> guard(access_mutex_);

  // Create a new AllocationRecord with the right properties
  AllocationRecord ar(pos, size, as, am, al);

  // Get back an allocation
  auto newAlloc = insert(ar);

  // dump the value in this allocation
  auto val = newAlloc.new_value(pos, size, false);
  logging::debug() << "Emitting value during new allocation\n";
  logging::atomic_out(val.to_json_string() + "\n");
  return newAlloc;
}

Value Allocations::find_value(const uintptr_t pos, const size_t size,
                              const AddressSpace &as) {
  std::lock_guard<std::mutex> guard(access_mutex_);
  logging::err() << "INFO: Looking for value @ {" << as.str() << "}[" << pos
                 << ", +" << size << ")" << std::endl;

  const auto &alloc = unsafe_find(pos, size, as);
  if (!alloc) {
    logging::err() << "DEBU: Didn't find alloc while looking for value @ ["
                   << pos << ", +" << size << ")" << std::endl;
    return Value();
  } else {
    logging::err() << "DEBU: Found alloc [" << alloc.pos() << ", +"
                   << alloc.size() << ") while looking for value @ [" << pos
                   << ", +" << size << ")" << std::endl;
  }
  const auto val = alloc.value(pos);
  return val;
}

Value Allocations::new_value(const uintptr_t pos, const size_t size,
                             const AddressSpace &as, const bool initialized) {
  std::lock_guard<std::mutex> guard(access_mutex_);

  // Find the allocation
  auto alloc = unsafe_find(pos, size, as);
  assert(alloc && "Allocation should be valid");

  return alloc.new_value(pos, size, initialized);
}

} // namespace cprof
