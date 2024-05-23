#include <Arduino.h>          // Base library for Arduino functions
#include <SPI.h>              // Library for SPI communication
#include <WiFi.h>             // Library for WiFi functions
#include <WiFiUdp.h>          // Library for WiFi UDP communication
#include <WiFiClient.h>       // Library for WiFi client functions
#include <ArduinoJson.h>      // Library for JSON parsing and serialization
#include "DW1000Ranging.h"    // Library for DW1000 ranging
#include "lcdgfx.h"           // Library for LCD graphics
#include "imu.h"              // Custom library for IMU functions
#include "uwb.h"              // Custom library for UWB functions
#include "display.h"          // Custom library for display functions
#include "ota.h"              // Custom library for OTA updates
#include "config.h"           // Configuration file with network and device settings
#include "globals.h"          // Global variables used in the project
#include "link.h"             // Custom library for link management
#include "networking.h"       // Custom library for networking functions

// GLOBALS
DisplaySSD1306_128x64_I2C display(-1); // Initialize the display object

#if DEVICE_TYPE == IS_TAG
MyLink *uwb_data;                       // Pointer to UWB data
unsigned long lastUpdateTime = 0;       // Last update time for sending data
unsigned int updateInterval = 200;      // Update interval in milliseconds
#endif

char shortAddress[6];                   // Short address buffer

// Callback function for new range measurements
void newRange() {
  uint32_t stamp = micros(); // Get the current time in microseconds
  Serial.write(USB_Packet.buffer, sizeof(USB_Packet.buffer)); // Send USB packet
  update_link(uwb_data, DW1000Ranging.getDistantDevice()->getShortAddress(), 
              DW1000Ranging.getDistantDevice()->getRange(), 
              DW1000Ranging.getDistantDevice()->getRXPower()); // Update link data
}

// Callback function for new device detection
void newDevice(DW1000Device *device) {
  add_link(uwb_data, device->getShortAddress()); // Add the new device to the link
}

// Callback function for inactive device detection
void inactiveDevice(DW1000Device *device) {
  delete_link(uwb_data, device->getShortAddress()); // Remove the inactive device from the link
}

// Function to set up WiFi
void setupWiFi() {
  WiFi.mode(WIFI_STA);           // Set WiFi mode to station
  WiFi.setSleep(false);          // Disable WiFi sleep
  WiFi.begin(ssid, password);    // Connect to the WiFi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP()); // Print the local IP address
  delay(500);
}

// Arduino setup function, runs once at startup
void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud
  Serial.println(__FILE__ " " __DATE__); // Print file name and date

  SPI.begin(18, 19, 23); // Initialize SPI with specified pins
  DW1000Ranging.initCommunication(27, 5, 34); // Initialize DW1000 communication
  DW1000Ranging.attachNewRange(newRange); // Attach new range callback
  DW1000Ranging.attachNewDevice(newDevice); // Attach new device callback
  DW1000Ranging.attachInactiveDevice(inactiveDevice); // Attach inactive device callback

  display.setFixedFont(ssd1306xled_font6x8); // Set font for display
  display.begin(); // Initialize display
  display.clear(); // Clear display

#if DEVICE_TYPE == IS_TAG
  DW1000Ranging.startAsTag("02:00:00:00:00:00:02:02", DW1000.MODE_LONGDATA_RANGE_ACCURACY, true); // Start as a tag
  display.printFixed(0, 0, "TAG", STYLE_NORMAL); // Display "TAG" on the screen
  uwb_data = init_link(); // Initialize UWB data
#else
  DW1000Ranging.startAsAnchor("02:00:00:00:00:00:02:02", DW1000.MODE_LONGDATA_RANGE_ACCURACY, true); // Start as an anchor
  display.printFixed(0, 0, "ANCHOR", STYLE_NORMAL); // Display "ANCHOR" on the screen
#endif

  setupWiFi(); // Set up WiFi

  byte* currentShortAddress = DW1000Ranging.getCurrentShortAddress(); // Get current short address
  sprintf(shortAddress, "%02X%02X", currentShortAddress[1], currentShortAddress[0]); // Format short address
  display.printFixed(0, 8, "02:00:00:00:00:00:02:02", STYLE_NORMAL); // Display full address
  display.printFixed(0, 16, shortAddress, STYLE_NORMAL); // Display short address

  Serial.println("Setup complete");

  initIMU(); // Initialize IMU
  setupOTA(); // Initialize OTA updates
  setupNetworking(); // Initialize networking
}

// Arduino loop function, runs repeatedly
void loop() {
#if DEVICE_TYPE == IS_TAG
  DW1000Ranging.loop(); // Process DW1000 ranging
  readAndPrintIMUData(); // Read and print IMU data
  handleOTA(); // Handle OTA updates

  if ((millis() - lastUpdateTime) > updateInterval) { // Check if it's time to send data
    sendJSON(uwb_data, shortAddress, host, portNum); // Send data as JSON
    lastUpdateTime = millis(); // Update last update time
  }
#endif
}
