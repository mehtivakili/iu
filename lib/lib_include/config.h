#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h> // Include the standard integer types

// WiFi configuration
extern const char *ssid;
extern const char *password;
extern const char *host;
extern const uint16_t portNum;

// Define communication methods
#define COMM_METHOD_SERIAL 0
#define COMM_METHOD_UDP 1
#define COMM_METHOD_TCP 2

// Set the communication method
#define COMM_METHOD COMM_METHOD_UDP // Change to COMM_METHOD_SERIAL or COMM_METHOD_TCP as needed

// Define device type
#define IS_TAG 1
#define IS_ANCHOR 0

// Set the device type
#define DEVICE_TYPE IS_TAG // Change to IS_ANCHOR as needed

#endif // CONFIG_H
