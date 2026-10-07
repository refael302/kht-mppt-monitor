#pragma once

#include <Arduino.h>
#include <stdint.h>

struct MpptReading {
  bool valid;
  bool link;
  uint32_t updated_ms;
  uint32_t age_ms;
  uint16_t voltage_raw;
  uint16_t current_raw;
  uint16_t power_raw;
  uint8_t temp_internal_c;
  bool temp_external_valid;
  uint8_t temp_external_c;
  uint8_t status;
  uint32_t frames_ok;
  uint32_t frames_bad;
  uint32_t bytes_seen;
  char raw_hex[40];
};

class MpptLink {
 public:
  void push(uint8_t byte);
  MpptReading snapshot(uint32_t now_ms) const;
  static bool exampleDecodes();

 private:
  void accept(const uint8_t frame[12], uint32_t now_ms);

  uint8_t window_[12] = {};
  uint8_t filled_ = 0;
  MpptReading latest_ = {};
  uint32_t frames_ok_ = 0;
  uint32_t frames_bad_ = 0;
  uint32_t bytes_seen_ = 0;
};
