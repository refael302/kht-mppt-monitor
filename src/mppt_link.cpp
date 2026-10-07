#include "mppt_link.h"

#include "config.h"

#include <stdio.h>
#include <string.h>

namespace {

uint16_t readLe(const uint8_t* p) {
  return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

bool checksumOk(const uint8_t frame[12]) {
  if (frame[0] != 0x5A || frame[11] != 0xFB) {
    return false;
  }
  uint8_t bcc = 0x5A;
  for (int i = 1; i <= 9; i++) {
    bcc ^= frame[i];
  }
  return bcc == frame[10];
}

void writeHex(const uint8_t frame[12], char* out, size_t size) {
  size_t used = 0;
  out[0] = '\0';
  for (int i = 0; i < 12; i++) {
    int wrote = snprintf(out + used, size - used, i ? " %02X" : "%02X", frame[i]);
    if (wrote < 0 || (size_t)wrote >= size - used) {
      break;
    }
    used += (size_t)wrote;
  }
}

}  // namespace

void MpptLink::push(uint8_t byte) {
  bytes_seen_++;
  if (filled_ < 12) {
    window_[filled_++] = byte;
  } else {
    memmove(window_, window_ + 1, 11);
    window_[11] = byte;
  }
  if (filled_ < 12) {
    return;
  }
  if (window_[0] != 0x5A || window_[11] != 0xFB) {
    return;
  }
  if (!checksumOk(window_)) {
    frames_bad_++;
    return;
  }
  accept(window_, millis());
}

void MpptLink::accept(const uint8_t frame[12], uint32_t now_ms) {
  frames_ok_++;
  latest_.valid = true;
  latest_.updated_ms = now_ms;
  latest_.status = frame[1];
  latest_.voltage_raw = readLe(&frame[2]);
  latest_.current_raw = readLe(&frame[4]);
  latest_.power_raw = readLe(&frame[6]);
  latest_.temp_external_valid = frame[8] != 0;
  latest_.temp_external_c = frame[8];
  latest_.temp_internal_c = frame[9];
  writeHex(frame, latest_.raw_hex, sizeof latest_.raw_hex);
}

MpptReading MpptLink::snapshot(uint32_t now_ms) const {
  MpptReading copy = latest_;
  copy.frames_ok = frames_ok_;
  copy.frames_bad = frames_bad_;
  copy.bytes_seen = bytes_seen_;
  if (copy.valid) {
    copy.age_ms = now_ms - copy.updated_ms;
    copy.link = copy.age_ms < MPPT_LINK_TIMEOUT_MS;
  } else {
    copy.age_ms = 0;
    copy.link = false;
  }
  return copy;
}

bool MpptLink::exampleDecodes() {
  // Numbered bytes on the KHT sheet. BCC is 0x5A XOR bytes 1..9.
  static const uint8_t example[12] = {
      0x5A, 0x81, 0x93, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x1C, 0x55, 0xFB};
  MpptLink link;
  for (uint8_t byte : example) {
    link.push(byte);
  }
  MpptReading reading = link.snapshot(0);
  return reading.valid && reading.frames_ok == 1 && reading.frames_bad == 0 &&
         reading.status == 0x81 && reading.voltage_raw == 0x0093 &&
         reading.current_raw == 0 && reading.power_raw == 1 &&
         !reading.temp_external_valid && reading.temp_internal_c == 0x1C;
}
