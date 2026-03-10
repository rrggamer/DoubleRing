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
#define RTC_DAT   27  // I/O pin
#define RTC_CLK   26  // SCLK pin
#define RTC_RST   25  // CE / RST pin

// Create objects for SD and RTC
SdFat sd;
SdFile dataFile;
ThreeWire myWire(RTC_DAT, RTC_CLK, RTC_RST);
RtcDS1302<ThreeWire> Rtc(myWire);

void setup() {
  Serial.begin(115200);
  delay(1000); 

  Serial.println("\n--- Starting Infiltrometer Log ---");

  // ==========================================
  // 1. INITIALIZE & READ THE RTC
  // ==========================================
  Rtc.Begin();

  // If the RTC lost power (or is brand new), set it to the time this code was compiled
  if (!Rtc.IsDateTimeValid()) {
    Serial.println("RTC lost power! Setting to computer compile time...");
    Rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
  }
  
  // Unlock the RTC if it is locked
  if (Rtc.GetIsWriteProtected()) {
    Rtc.SetIsWriteProtected(false);
  }
  // Start the clock ticking if it is paused
  if (!Rtc.GetIsRunning()) {
    Rtc.SetIsRunning(true);
  }

  // Grab the current time
  RtcDateTime now = Rtc.GetDateTime();
  
  // Format the time into a nice string: "YYYY/MM/DD HH:MM:SS"
  char timeString[25];
  snprintf(timeString, sizeof(timeString), "%04d/%02d/%02d %02d:%02d:%02d", 
           now.Year(), now.Month(), now.Day(), 
           now.Hour(), now.Minute(), now.Second());

  Serial.print("Current RTC Time: ");
  Serial.println(timeString);

  // ==========================================
  // 2. INITIALIZE & WRITE TO SD CARD
  // ==========================================
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

  if (!sd.begin(CS_PIN, SD_SCK_MHZ(4))) {
    Serial.println("SD Initialization failed!");
    return;
  }

  // Open the file (creates it if it doesn't exist)
  if (dataFile.open("infil_log.txt", O_RDWR | O_CREAT | O_AT_END)) {
    Serial.println("Writing data to SD...");
    
    // Write the formatted timestamp to the SD card
    dataFile.print("Log Entry at: ");
    dataFile.println(timeString);
    
    dataFile.close(); 
    Serial.println("Success! Data written and file closed.");
  } else {
    Serial.println("Error opening infil_log.txt");
  }
}

void loop() {
  // Nothing to do here for this test
}