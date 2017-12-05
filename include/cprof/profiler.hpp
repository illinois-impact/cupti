#ifndef CPROF_PROFILER_HPP
#define CPROF_PROFILER_HPP

#include <ostream>

#include "cprof/cupti_subscriber.hpp"
#include "cprof/model/driver.hpp"
#include "cprof/model/hardware.hpp"
#include "util/environment_variable.hpp"
#include "util/logging.hpp"

namespace cprof {

class Profiler {
  friend model::Hardware &hardware();
  friend model::Driver &driver();
  friend Allocations &allocations();
  friend Values &values();

public:
  ~Profiler();

  /* \brief Initialize the profiler
   *
   */
  void init();
  static Profiler &instance();

  const std::string &zipkin_host() { return zipkinHost_; }
  const uint32_t &zipkin_port() { return zipkinPort_; }

  CuptiSubscriber *manager_; // FIXME: make this private and add an accessor

private:
  Profiler();
  model::Hardware hardware_;
  model::Driver driver_;
  Allocations allocations_;
  Values values_;

  bool useCuptiCallback_;
  bool useCuptiActivity_;
  std::string zipkinHost_;
  uint32_t zipkinPort_;

  bool isInitialized_;
};

inline model::Hardware &hardware() { return Profiler::instance().hardware_; }
inline model::Driver &driver() { return Profiler::instance().driver_; }
inline std::ostream &out() { return logging::out(); }
inline void atomic_out(const std::string &s) { logging::atomic_out(s); }
inline std::ostream &err() { return logging::err(); }
inline Allocations &allocations() { return Profiler::instance().allocations_; }
inline Values &values() { return Profiler::instance().values_; }

/* \brief Runs Profiler::init() at load time
 */
class ProfilerInitializer {
public:
  ProfilerInitializer() {
    Profiler &p = Profiler::instance();
    p.init();
  }
};

} // namespace cprof

#endif