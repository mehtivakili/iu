#include <Arduino.h> // Include Arduino base library
#include <SPI.h>     // Include SPI library
#include "imu.h"     // Include the IMU header file

SPIClass mySPI(HSPI);          // Create an SPIClass object
Bmi088Accel accel(SPI, 32);    // Create an accelerometer object
Bmi088Gyro gyro(SPI, 25);      // Create a gyroscope object

void initIMU() {
  int status;
  while (!Serial) {} // Wait for serial connection
  status = accel.begin(); // Initialize accelerometer
  if (status < 0) {
    Serial.println("Accel Initialization Error"); // Print error if initialization fails
    Serial.println(status); // Print status code
    while (1) {} // Halt execution
  }
  status = gyro.begin(); // Initialize gyroscope
  if (status < 0) {
    Serial.println("Gyro Initialization Error"); // Print error if initialization fails
    Serial.println(status); // Print status code
    while (1) {} // Halt execution
  }
}

void readAndPrintIMUData() {
  accel.readSensor(); // Read accelerometer data
  gyro.readSensor(); // Read gyroscope data
  Serial.print(accel.getAccelX_mss()); // Print X-axis acceleration
  Serial.print("\t");
  Serial.print(accel.getAccelY_mss()); // Print Y-axis acceleration
  Serial.print("\t");
  Serial.print(accel.getAccelZ_mss()); // Print Z-axis acceleration
  Serial.print("\t");
  Serial.print(gyro.getGyroX_rads()); // Print X-axis gyroscope data
  Serial.print("\t");
  Serial.print(gyro.getGyroY_rads()); // Print Y-axis gyroscope data
  Serial.print("\t");
  Serial.print(gyro.getGyroZ_rads()); // Print Z-axis gyroscope data
  Serial.print("\t");
  Serial.print(accel.getTemperature_C()); // Print temperature
  Serial.print("\n");
  delay(20); // Delay for 20 milliseconds
}
