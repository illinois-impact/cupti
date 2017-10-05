#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "kernel_time.hpp"

using namespace std::chrono;

std::map<uint32_t, time_points_t> KernelCallTime::tid_to_time;
std::map<uint32_t, const char *> KernelCallTime::correlation_to_function;

KernelCallTime &KernelCallTime::instance() {
  static KernelCallTime a;
  return a;
}

KernelCallTime::KernelCallTime() {}

void KernelCallTime::kernel_start_time(const CUpti_CallbackData *cbInfo) {

  uint64_t startTimeStamp;
  cuptiDeviceGetTimestamp(cbInfo->context, &startTimeStamp);
  time_points_t time_point;
  time_point.start_time = startTimeStamp;
  this->tid_to_time.insert(
      std::pair<uint32_t, time_points_t>(cbInfo->correlationId, time_point));
  this->correlation_to_function.insert(std::pair<uint32_t, const char *>(
      cbInfo->correlationId, cbInfo->symbolName));

  std::cout << cbInfo->correlationId << " - The start timestamp is "
            << startTimeStamp << std::endl;
}

void KernelCallTime::kernel_end_time(const CUpti_CallbackData *cbInfo) {
  uint64_t endTimeStamp;
  cuptiDeviceGetTimestamp(cbInfo->context, &endTimeStamp);
  auto time_point_iterator = this->tid_to_time.find(cbInfo->correlationId);
  time_point_iterator->second.end_time = endTimeStamp;

  std::cout << cbInfo->correlationId << " - The end timestamp is "
            << endTimeStamp << std::endl;
}

void KernelCallTime::write_to_file() {
  for (auto iter = this->tid_to_time.begin(); iter != this->tid_to_time.end();
       iter++) {
    std::cout << this->correlation_to_function.find(iter->first)->second
              << " -Time- " << iter->second.end_time - iter->second.start_time
              << "ns\n";
  }
}