#pragma once

// Phone joins this access point and opens http://192.168.4.1
#define WIFI_SSID "KHT-MPPT"
#define WIFI_PASS "mppt1234"

// Controller port: G = GND, T = controller TX, R = controller RX, V = supply.
// Wire T to MPPT_RX_PIN and G to ESP32 GND. Leave R and V unconnected.
#define MPPT_RX_PIN 16
#define MPPT_TX_PIN 17
#define MPPT_BAUD 9600

// The sheet labels every field "100 mV". Voltage matches that unit.
// Current is read as 0.1 A per count, power as 0.1 W per count.
#define MPPT_VOLT_STEP 0.1f
#define MPPT_AMP_STEP 0.1f
#define MPPT_WATT_STEP 0.1f

// Drop the link flag after this many milliseconds without a valid frame.
#define MPPT_LINK_TIMEOUT_MS 1000

// Onboard LED on most ESP32 DevKit boards. Set LED_ACTIVE_LOW to 1 if yours is inverted.
#define LED_PIN 2
#define LED_ACTIVE_LOW 0
