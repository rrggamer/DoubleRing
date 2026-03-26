#include <Arduino.h>

#define BATTERY_PIN 34 


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("--- Battery Voltage Test ---");
  
  analogReadResolution(12); 
}

void loop() {
  int rawValue = analogRead(BATTERY_PIN);

  // Formula: (Raw / 4095.0) * 3.3V reference * 2.0 (to reverse the half-cut)
  float voltage = (rawValue / 4095.0) * 3.3 * 2.0;

  Serial.print("Raw ADC Value: ");
  Serial.print(rawValue);
  Serial.print("  |  Calculated Battery: ");
  Serial.print(voltage);
  Serial.println(" V");

  delay(1000); 
}