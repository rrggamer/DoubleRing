#include <Arduino.h>
#include <SPI.h>
#include "SdFat.h"
#include <ThreeWire.h>  
#include <RtcDS1302.h>

// --- SD Card SPI Pins ---
#define SCK_PIN   18
#define MISO_PIN  19
#define MOSI_PIN  23
#define CS_PIN    5

// --- DS1302 RTC Pins ---
#define RTC_DAT   27  
#define RTC_CLK   26  
#define RTC_RST   14  

// --- Ultrasonic Pins ---
#define TRIG_PIN  16
#define ECHO_PIN  17

// --- Hardware Objects ---
SdFat sd;
SdFile dataFile;
ThreeWire myWire(RTC_DAT, RTC_CLK, RTC_RST);
RtcDS1302<ThreeWire> Rtc(myWire);

const float SOUND_SPEED = 0.0343; // cm per microsecond

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n--- Infiltrometer CSV Logger Starting ---");

  // 1. Initialize Ultrasonic Pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // 2. Initialize the RTC Clock
  Rtc.Begin();
  if (!Rtc.IsDateTimeValid()) {
    Serial.println("Warning: RTC time is invalid. Check battery!");
  }
  if (Rtc.GetIsWriteProtected()) Rtc.SetIsWriteProtected(false);
  if (!Rtc.GetIsRunning()) Rtc.SetIsRunning(true);

  // 3. Initialize the SD Card
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  if (!sd.begin(CS_PIN, SD_SCK_MHZ(4))) {
    Serial.println("CRITICAL ERROR: SD Card Mount Failed!");
    return; // Stop the program if the SD card fails
  }

  // 4. Create the CSV File and Write the Header Row
  if (dataFile.open("infiltrometer.csv", O_RDWR | O_CREAT | O_AT_END)) {
    // Only print the header if the file is completely empty (0 bytes)
    if (dataFile.fileSize() == 0) {
      dataFile.println("Timestamp,Water_Level_cm"); 
      Serial.println("New CSV created. Header row written.");
    }
    dataFile.close();
  } else {
    Serial.println("Error: Failed to open CSV file on startup.");
  }
}

void loop() {
  // ==========================================
  // A. READ THE ULTRASONIC SENSOR
  // ==========================================
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  float distance_cm = 0.0;
  
  if (duration > 0) {
    distance_cm = (duration * SOUND_SPEED) / 2.0;
  }

  // ==========================================
  // B. READ THE RTC TIMESTAMP
  // ==========================================
  RtcDateTime now = Rtc.GetDateTime();
  char timeStr[25];
  snprintf(timeStr, sizeof(timeStr), "%04d/%02d/%02d %02d:%02d:%02d", 
           now.Year(), now.Month(), now.Day(), 
           now.Hour(), now.Minute(), now.Second());

  // ==========================================
  // C. FORMAT THE CSV ROW
  // ==========================================
  char csvRow[64];
  if (duration > 0) {
    // If we got a good reading, format it with 2 decimal places
    snprintf(csvRow, sizeof(csvRow), "%s,%.2f", timeStr, distance_cm);
  } else {
    // If the sensor glitched or the water is out of range, write "ERROR"
    snprintf(csvRow, sizeof(csvRow), "%s,ERROR", timeStr);
  }

  // ==========================================
  // D. WRITE ROW TO SD CARD
  // ==========================================
  if (dataFile.open("infiltrometer.csv", O_RDWR | O_CREAT | O_AT_END)) {
    dataFile.println(csvRow);
    dataFile.close(); // You MUST close it to save the data
    
    Serial.print("Saved to SD: ");
    Serial.println(csvRow);
  } else {
    Serial.println("Error: Could not open CSV for writing.");
  }

  // ==========================================
  // E. WAIT BEFORE NEXT READING
  // ==========================================
  // Set to 5000ms (5 seconds) so you can test it quickly right now.
  // Change this to 300000 (5 minutes) for your real field tests!
  delay(5000); 
}