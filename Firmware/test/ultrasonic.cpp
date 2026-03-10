#include <Arduino.h>

#define TRIG 5
#define ECHO 18

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() {

  // send trigger pulse
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // read echo pulse time
  long duration = pulseIn(ECHO, HIGH);

  Serial.print("Echo pulse duration: ");
  Serial.println(duration);

  delay(500);
}