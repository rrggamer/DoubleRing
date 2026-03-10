#include <Arduino.h>

#define POWER_PIN  12 // ESP32 pin that provides power to the sensor
#define SIGNAL_PIN 26 // ESP32 pin that reads the analog signal

int value = 0; // variable to store the sensor value

void setup() {
  Serial.begin(115200);
  pinMode(POWER_PIN, OUTPUT);   // configure power pin as output
  digitalWrite(POWER_PIN, LOW); // turn power off initially
}

void loop() {
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10ms for power to settle
  value = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  Serial.print("Water level value: ");
  Serial.println(value);

  delay(1000); // take a reading every second
}


