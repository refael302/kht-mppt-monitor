#include <DNSServer.h>
#include <WebServer.h>
#include <WiFi.h>

#include <stdio.h>

#include "config.h"
#include "mppt_link.h"
#include "web_page.h"

namespace {

HardwareSerial MpptSerial(2);
MpptLink mppt;
WebServer server(80);
DNSServer dnsServer;

const char* statusText(uint8_t status) {
  switch (status & 0x0F) {
    case 1:
      return "בטעינה";
    case 2:
      return "הטעינה הסתיימה";
    case 4:
      return "יש כניסה, אין טעינה";
    default:
      return "מצב לא מזוהה";
  }
}

const char* statusClass(uint8_t status) {
  switch (status & 0x0F) {
    case 1:
      return "ok";
    case 2:
      return "done";
    case 4:
      return "warn";
    default:
      return "bad";
  }
}

void formatTenths(char* out, size_t size, uint16_t raw) {
  // config.h scales are 0.1 per count, so the display is raw / 10.
  snprintf(out, size, "%u.%u", raw / 10u, raw % 10u);
}

void sendPage() {
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/html; charset=utf-8", INDEX_HTML);
}

void sendApi() {
  MpptReading reading = mppt.snapshot(millis());
  char voltage[16];
  char current[16];
  char power[16];
  char external[16];
  formatTenths(voltage, sizeof voltage, reading.voltage_raw);
  formatTenths(current, sizeof current, reading.current_raw);
  formatTenths(power, sizeof power, reading.power_raw);
  if (reading.temp_external_valid) {
    snprintf(external, sizeof external, "%u", reading.temp_external_c);
  } else {
    snprintf(external, sizeof external, "null");
  }

  char body[768];
  snprintf(body, sizeof body,
           "{\"link\":%s,\"status\":%u,\"status_text\":\"%s\",\"status_class\":\"%s\","
           "\"voltage_v\":%s,\"current_a\":%s,\"power_w\":%s,"
           "\"temp_internal_c\":%u,\"temp_external_c\":%s,"
           "\"age_ms\":%lu,\"frames_ok\":%lu,\"frames_bad\":%lu,\"bytes_seen\":%lu,"
           "\"raw\":\"%s\"}",
           reading.link ? "true" : "false", reading.status, statusText(reading.status),
           statusClass(reading.status), voltage, current, power, reading.temp_internal_c,
           external, (unsigned long)reading.age_ms, (unsigned long)reading.frames_ok,
           (unsigned long)reading.frames_bad, (unsigned long)reading.bytes_seen, reading.raw_hex);

  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", body);
}

void startAccessPoint() {
  WiFi.mode(WIFI_AP);
  WiFi.setSleep(false);
  WiFi.softAP(WIFI_SSID, WIFI_PASS);
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.on("/", HTTP_GET, sendPage);
  server.on("/api", HTTP_GET, sendApi);
  server.on("/favicon.ico", HTTP_GET, []() { server.send(204, "text/plain", ""); });
  server.onNotFound(sendPage);
  server.begin();
}

}  // namespace

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_ACTIVE_LOW ? HIGH : LOW);
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("KHT MPPT monitor");
  if (!MpptLink::exampleDecodes()) {
    Serial.println("MPPT frame self-check failed");
  }
  MpptSerial.begin(MPPT_BAUD, SERIAL_8N1, MPPT_RX_PIN, MPPT_TX_PIN);
  startAccessPoint();
  Serial.printf("AP %s  password %s\n", WIFI_SSID, WIFI_PASS);
  Serial.print("Open http://");
  Serial.println(WiFi.softAPIP());
  Serial.printf("UART2 RX=GPIO%d TX=GPIO%d %d 8N1\n", MPPT_RX_PIN, MPPT_TX_PIN, MPPT_BAUD);
  Serial.println("Wire controller T to GPIO16 and G to GND. Leave V unconnected.");
}

void loop() {
  while (MpptSerial.available() > 0) {
    mppt.push((uint8_t)MpptSerial.read());
  }
  MpptReading reading = mppt.snapshot(millis());
  bool led_on = reading.link;
  if (LED_ACTIVE_LOW) {
    led_on = !led_on;
  }
  digitalWrite(LED_PIN, led_on ? HIGH : LOW);
  dnsServer.processNextRequest();
  server.handleClient();
}
