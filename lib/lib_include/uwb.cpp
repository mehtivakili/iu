#include <Arduino.h> // Include Arduino base library
#include <WiFi.h> // Include WiFi library
#include <WiFiUdp.h> // Include WiFi UDP library
#include <ArduinoJson.h> // Include ArduinoJson library
#include "DW1000Ranging.h" // Include DW1000Ranging library
#include "uwb.h" // Include UWB header file
#include "link.h" // Include link header file
#include "globals.h" // Include globals header file

void newRange() {
  uint32_t stamp = micros(); // Get the current time in microseconds
  Serial.write(USB_Packet.buffer, sizeof(USB_Packet.buffer)); // Send USB packet data
  update_link(uwb_data, DW1000Ranging.getDistantDevice()->getShortAddress(), 
              DW1000Ranging.getDistantDevice()->getRange(), 
              DW1000Ranging.getDistantDevice()->getRXPower()); // Update link data
}

void newDevice(DW1000Device *device) {
  add_link(uwb_data, device->getShortAddress()); // Add the new device to the link
}

void inactiveDevice(DW1000Device *device) {
  delete_link(uwb_data, device->getShortAddress()); // Remove the inactive device from the link
}

void setupWiFi() {
  WiFi.mode(WIFI_STA); // Set WiFi mode to station
  WiFi.setSleep(false); // Disable WiFi sleep mode
  WiFi.begin("ssid", "password"); // Connect to WiFi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Wait for connection
    Serial.print(".");
  }
  Serial.println("Connected");
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP()); // Print local IP address
  delay(500); // Delay for 500 milliseconds
}

void sendJSON(struct MyLink *p, WiFiUDP &udp, const char *shortAddress, const char *host, uint16_t portNum) {
  StaticJsonDocument<500> doc; // Create a JSON document
  doc["id"] = shortAddress; // Add short address to JSON document
  JsonArray links = doc.createNestedArray("links"); // Create a nested array for links
  struct MyLink *temp = p;
  while (temp->next != NULL) {
    temp = temp->next;
    JsonObject obj1 = links.createNestedObject(); // Create a nested object for each link
    obj1["a"] = temp->anchor_addr; // Add anchor address to JSON object
    char range[5];
    sprintf(range, "%.2f", temp->range[0]); // Format range as string
    obj1["r"] = range; // Add range to JSON object
  }
  udp.beginPacket(host, portNum); // Start UDP packet
  serializeJson(doc, udp); // Serialize JSON document to UDP packet
  udp.println(); // End JSON document
  udp.endPacket(); // End UDP packet
}
