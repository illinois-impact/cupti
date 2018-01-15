#ifndef CUPTI_SUBSCRIBER_HPP
#define CUPTI_SUBSCRIBER_HPP

#include <iostream>

#include "cprof/util_cupti.hpp"
#include "kernel_time.hpp"

#define BUFFER_SIZE 100000

class CuptiSubscriber {
private:
  CUpti_SubscriberHandle subscriber_;
  bool enableActivityAPI_; ///< gather info through the CUPTI activity API
  bool enableCallbackAPI_; ///< gather info from the CUPTI callback API
  bool enableZipkin_;      ///< send traces to zipkin

public:
  zipkin::ZipkinOtTracerOptions options;
  zipkin::ZipkinOtTracerOptions memcpy_tracer_options;
  zipkin::ZipkinOtTracerOptions launch_tracer_options;

  std::shared_ptr<opentracing::Tracer> tracer;
  std::shared_ptr<opentracing::Tracer> memcpy_tracer;
  std::shared_ptr<opentracing::Tracer> launch_tracer;
  span_t parent_span;

  CuptiSubscriber(const bool enableActivityAPI, const bool enableCallbackAPI,
                  const bool enableZipkin);
  void init();
  ~CuptiSubscriber();

  bool enable_zipkin() const { return enableZipkin_; }
  static void CUPTIAPI cuptiCallbackFunction(void *userdata,
                                             CUpti_CallbackDomain domain,
                                             CUpti_CallbackId cbid,
                                             const CUpti_CallbackData *cbInfo);

  static void CUPTIAPI cuptiActivityBufferCompleted(CUcontext ctx,
                                                    uint32_t streamId,
                                                    uint8_t *buffer,
                                                    size_t size,
                                                    size_t validSize);
  static void CUPTIAPI cuptiActivityBufferRequested(uint8_t **buffer,
                                                    size_t *size,
                                                    size_t *maxNumRecords);
  static CuptiSubscriber *singleton;
};

#endif