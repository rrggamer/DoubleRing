#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 BNO055 Orientation Sensor Test"); Serial.println("");

  // Initialize the sensor
  if (!bno.begin()) {
    // There was a problem detecting the BNO055 ... check your connections
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1); // Halt
  }

  delay(1000);

  // Use the external crystal for better accuracy
  bno.setExtCrystalUse(true);
}

void loop() {
  // Get a new sensor event
  sensors_event_t event;
  bno.getEvent(&event);

  // Display the floating point data (Euler angles in degrees)
  Serial.print("X (Heading): ");
  Serial.print(event.orientation.x, 2);
  Serial.print("\tY (Roll): ");
  Serial.print(event.orientation.y, 2);
  Serial.print("\tZ (Pitch): ");
  Serial.print(event.orientation.z, 2);

  // Get and display the calibration status
  // Calibration values map from 0 (uncalibrated) to 3 (fully calibrated)
  uint8_t system, gyro, accel, mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  
  Serial.print("\t|| Calib Status: Sys=");
  Serial.print(system, DEC);
  Serial.print(" Gyro=");
  Serial.print(gyro, DEC);
  Serial.print(" Accel=");
  Serial.print(accel, DEC);
  Serial.print(" Mag=");
  Serial.println(mag, DEC);

  delay(BNO055_SAMPLERATE_DELAY_MS);
}