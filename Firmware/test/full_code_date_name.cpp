#include <Arduino.h>
#include <SPI.h>
#include "SdFat.h"
#include <ThreeWire.h>  
#include <RtcDS1302.h>

// --- Pin Definitions ---
// SD Card SPI
#define SCK_PIN   18
#define MISO_PIN  19
#define MOSI_PIN  23
#define CS_PIN    5

// DS1302 RTC
#define RTC_DAT   27  
#define RTC_CLK   26  
#define RTC_RST   14  

// Ultrasonic Sensor
#define TRIG_PIN  16
#define ECHO_PIN  17

// Safety/Status Indicator
#define WARN_LED_PIN 2 

// --- Configuration ---
// How long the ESP32 sleeps between readings (in minutes)
#define SLEEP_MINUTES 0.1

const float SOUND_SPEED = 0.0343; // cm per microsecond
const uint64_t uS_TO_S_FACTOR = 1000000ULL; // Conversion factor for microseconds to seconds

// --- Hardware Objects ---
SdFat sd;
SdFile dataFile;
ThreeWire myWire(RTC_DAT, RTC_CLK, RTC_RST);
RtcDS1302<ThreeWire> Rtc(myWire);

void setup() {
  Serial.begin(115200);
  delay(500); // Short delay to let hardware stabilize on wake-up
  
  Serial.println("\n--- ESP32 Waking Up ---");

  // 1. Initialize Basic Pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(WARN_LED_PIN, OUTPUT);
  digitalWrite(WARN_LED_PIN, LOW); // Ensure LED is off

  // 2. Read the RTC Timestamp
  Rtc.Begin();
  RtcDateTime now = Rtc.GetDateTime();
  
  char timeStr[25];
  snprintf(timeStr, sizeof(timeStr), "%04d/%02d/%02d %02d:%02d:%02d", 
           now.Year(), now.Month(), now.Day(), 
           now.Hour(), now.Minute(), now.Second());

  // Create the daily filename (Format: YYYYMMDD.csv, e.g., 20260310.csv)
  char filename[16];
  snprintf(filename, sizeof(filename), "%04d%02d%02d.csv", now.Year(), now.Month(), now.Day());

  // 3. Read the Ultrasonic Sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  float distance_cm = 0.0;
  char csvRow[64];
  
  if (duration > 0) {
    distance_cm = (duration * SOUND_SPEED) / 2.0;
    snprintf(csvRow, sizeof(csvRow), "%s,%.2f", timeStr, distance_cm);
  } else {
    snprintf(csvRow, sizeof(csvRow), "%s,ERROR", timeStr);
  }

  // 4. Initialize SD Card & Save Data
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  
  if (sd.begin(CS_PIN, SD_SCK_MHZ(4))) {
    
    // TURN ON WARNING LED: DANGER ZONE BEGINS
    digitalWrite(WARN_LED_PIN, HIGH);
    
    // Open today's specific file
    if (dataFile.open(filename, O_RDWR | O_CREAT | O_AT_END)) {
      
      // If it is a brand new day/file, write the header row first
      if (dataFile.fileSize() == 0) {
        dataFile.println("Timestamp,Water_Level_cm"); 
      }
      
      // Write the actual sensor data
      dataFile.println(csvRow);
      dataFile.close(); // LOCK THE DATA SAFELY
      
      Serial.print("Successfully saved to ");
      Serial.print(filename);
      Serial.print(": ");
      Serial.println(csvRow);
    } else {
      Serial.println("Error: Could not open file for writing.");
    }
    
    // TURN OFF WARNING LED: SAFE TO UNPLUG
    digitalWrite(WARN_LED_PIN, LOW);
    
  } else {
    Serial.println("CRITICAL ERROR: SD Card Mount Failed!");
  }

  // 5. Go to Deep Sleep
  Serial.println("Going to sleep now...");
  Serial.flush(); // Ensure all serial messages finish printing before power down
  
  // Calculate sleep time in microseconds
  uint64_t sleep_time_us = SLEEP_MINUTES * 60 * uS_TO_S_FACTOR;
  esp_sleep_enable_timer_wakeup(sleep_time_us);
  
  esp_deep_sleep_start();
}

void loop() {
}